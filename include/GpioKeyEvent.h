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
        // bind long key press CB function
        bool BindBtnLongPress(void(*btn_long_press_event)(), uint16_t wait_ms=DEF_LONG_PRESS_WAIT_MS) {
            if(wait_ms < DEF_LONG_PRESS_WAIT_MS) return false;
            ButtonLongPressEvent = btn_long_press_event;
            LongPressWaitMS = wait_ms;
            return true;
        };
        // bind double click CB function
        void BindBtnDblPress(void(*btn_dbl_press_event)()) {
            ButtonDblPressEvent = btn_dbl_press_event;
        };
        // loop function
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
            // gpio status changed
            else {
                if(current_millis - last_jitter_millis > DEF_ELIMINATING_JITTER_MS) {
                    // key down
                    if(current_gpio_state == LOW) {
                        // is first keydown in cycle
                        if(0 == first_key_down_millis) {
                            first_key_down_millis = current_millis;
                            first_key_up_millis = 0;
                            action_done = false;
                        }
                        // is not first key down in cycle
                        else {
                            // has define double click CB function
                            if(nullptr != ButtonDblPressEvent){
                                // key down mill - last key up mill > elimination jitter interval
                                if(	0 != first_key_up_millis // is release key in event cycle
                                    && (current_millis - first_key_up_millis) > DEF_ELIMINATING_JITTER_MS) {	// skip eliminating jitter
                                    // is double click?
                                    if(	false == action_done // did in event cycle?
                                        && current_millis - first_key_up_millis < DEF_DB_PRESS_MS) {	// and 2nd click is in interval
                                        // call double click event function
                                        // Serial.println("Debug:Double Press Event.");
                                        ButtonDblPressEvent();
                                        action_done = true;
                                    }
                                    
                                }
                            }
                        }
                    }
                    // key up
                    else {
                        if(!action_done && first_key_down_millis && first_key_up_millis == 0) {
                            first_key_up_millis = current_millis;
                        }
                    }
                    // Keep gpio status
                    last_gpio_state = current_gpio_state;
                    last_jitter_millis = current_millis;
                }
            }
            
            if(action_done && current_gpio_state == HIGH) {
                // Serial.println("Event Reset.");
                first_key_down_millis = 0;
                first_key_up_millis = 0;
                action_done = false;
            }
        };
    protected:
        uint8_t GpioPin;					        // gpio pin of key
        void (*ButtonPressEvent)();       // Click Event CB function
        uint16_t LongPressWaitMS;			    // Long press ms
        void (*ButtonLongPressEvent)();		// Long press Event CB function
        void (*ButtonDblPressEvent)();		// Double click Event CB function
        uint32_t first_key_down_millis;
        uint32_t first_key_up_millis;
        bool action_done;
        uint8_t last_gpio_state;
        uint32_t last_jitter_millis;
};

#endif
