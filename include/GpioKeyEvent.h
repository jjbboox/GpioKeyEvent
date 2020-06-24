#ifndef _GPIO_BUTTON_H_
#define _GPIO_BUTTON_H_
#include <Arduino.h>

#define	DEF_ELIMINATING_JITTER_MS	20		// 默认消抖延时
#define DEF_LONG_PRESS_WAIT_MS  1000		// 默认长按延时
#define DEF_DB_PRESS_MS 300

class GpioButton {
    public:
    	// 构造函数，定义端口号，回调函数，初始化默认值
        GpioButton(uint8_t gpio_pin, void(*btn_press_event)()=nullptr) :
            GpioPin(gpio_pin), 
            ButtonPressEvent(btn_press_event), 
            LongPressWaitMS(DEF_LONG_PRESS_WAIT_MS), 
            ButtonLongPressEvent(nullptr),
            first_key_down_millis(0),
            first_key_up_millis(0),
            action_done(false),
            last_gpio_state(HIGH) {
                pinMode(GpioPin, INPUT_PULLUP);
                digitalWrite(GpioPin, HIGH);
        };
        // 绑定按键回调函数
        void BindBtnPress(void(*btn_press_event)()) {
        	ButtonPressEvent = btn_press_event;
        };
        // 绑定长按事件回调函数和长按的判定时长
        bool BindBtnLongPress(void(*btn_long_press_event)(), uint16_t wait_ms=DEF_LONG_PRESS_WAIT_MS) {
            if(wait_ms < DEF_LONG_PRESS_WAIT_MS) return false;
            ButtonLongPressEvent = btn_long_press_event;
            LongPressWaitMS = wait_ms;
            return true;
        };
        // 绑定双击回调函数
        void BindBtnDblPress(void(*btn_dbl_press_event)()) {
            ButtonDblPressEvent = btn_dbl_press_event;
        };
        // 轮询函数
        void loop(){
            
            uint8_t current_gpio_state = digitalRead(GpioPin);
            uint32_t current_millis = millis();
            
            // GPIO口状态未改变
            if(current_gpio_state == last_gpio_state) {
                if(current_gpio_state == LOW) {
                    if(first_key_down_millis && !first_key_up_millis && (current_millis - first_key_down_millis > LongPressWaitMS)) {
                        if(!action_done && ButtonLongPressEvent != nullptr) {
                            // Serial.print("current_millis:"); Serial.println(current_millis);
                            // Serial.print("first_key_down_millis:"); Serial.println(first_key_down_millis);
                            // Serial.print("ms:"); Serial.println(current_millis - first_key_up_millis);
                            
                            // Serial.println("Debug:Long Press Event.");
                            ButtonLongPressEvent();
                            action_done = true;
                        }
                    }
                }
                else {
                    if(first_key_up_millis && (current_millis - first_key_up_millis > DEF_DB_PRESS_MS)) {
                        if(!action_done && ButtonPressEvent != nullptr) {
                            // Serial.println("Debug:Press Event.");
                            ButtonPressEvent();
                            action_done = true;
                        }
                    }
                }
            }
            // GPIO口状态改变
            else {
                if(current_millis - last_jitter_millis > DEF_ELIMINATING_JITTER_MS) {
                    // 下降沿(key down)
                    if(current_gpio_state == LOW) {
                        // 检测周期内的初始按下
                        if(0 == first_key_down_millis) {
                            first_key_down_millis = current_millis;
                            first_key_up_millis = 0;
                            action_done = false;
                        }
                        // 检测周期内并非第一次按下
                        else {
                            // 如果已绑定双击事件
                            if(nullptr != ButtonDblPressEvent){
                                // 判定检测周期内本次按下与上次释放之间的间隔是否大于消抖时间间隔
                                if(	0 != first_key_up_millis // 按键检测周期内已存在按键释放
                                    && (current_millis - first_key_up_millis) > DEF_ELIMINATING_JITTER_MS) {	// 且当前时点距离按键释放已超过消抖时长
                                    // 判定双击是否有效
                                    if(	false == action_done // 按键周期内尚未执行过事件
                                        && current_millis - first_key_up_millis < DEF_DB_PRESS_MS) {	// 且再次按下按键时间距离上次释放按键时长小于双击判定时长
                                        // 调用双击事件回调函数
                                        // Serial.println("Debug:Double Press Event.");
                                        ButtonDblPressEvent();
                                        action_done = true;
                                    }
                                    
                                }
                            }
                        }
                    }
                    // 上升沿(key up)
                    else {
                        if(!action_done && first_key_down_millis && first_key_up_millis == 0) {
                            first_key_up_millis = current_millis;
                        }
                    }
                    // 保持现在的GPIO端口状态
                    last_gpio_state = current_gpio_state;
                    last_jitter_millis = current_millis;
                }
            }
            // 如果已执行过事件回调，则状态清零
            if(action_done && current_gpio_state == HIGH) {
                // Serial.println("Event Reset.");
                first_key_down_millis = 0;
                first_key_up_millis = 0;
                action_done = false;
            }
        };
    protected:
        uint8_t GpioPin;					// GPIO口编号
        void (*ButtonPressEvent)();			// 单次短按事件回调函数指针
        uint16_t LongPressWaitMS;			// 长按事件判定时长（毫秒）
        void (*ButtonLongPressEvent)();		// 长按事件回调函数指针
        void (*ButtonDblPressEvent)();		// 双击事件回调函数指针
        uint32_t first_key_down_millis;
        uint32_t first_key_up_millis;
        bool action_done;
        uint8_t last_gpio_state;
        uint32_t last_jitter_millis;
};

#endif
