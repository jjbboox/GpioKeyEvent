#ifndef _GPIO_BUTTON_H_
#define _GPIO_BUTTON_H_
#include <Arduino.h>

#define	DEF_ELIMINATING_JITTER_MS	20		// 消抖延时毫秒数
#define DEF_LONG_CLICK_MS           1000    // 默认单次长按事件触发毫秒数
#define DEF_DB_INTERVAL_MS          300     // 默认双击事件间隔毫秒数
#define DEF_LONG_PRESS_START_MS     DEF_LONG_CLICK_MS   // 长按循环触发事件的起始毫秒数
#define DEF_LONG_PRESS_INTERVAL_MS  500     // 长按循环触发触发事件的间隔毫秒数

#define DEF_KEY_UP                  HIGH
#define DEF_KEY_DOWN                LOW

typedef enum {
    KEY_DOWN,
    KEY_UP,
    NO_CHANGE
} KeyAction;

class GpioButton {
    public:
        GpioButton(uint8_t _pin) : BtnPin(_pin) {
            pinMode(BtnPin, INPUT);
        }
        
        void bindEventOnClick(void (*callback)()) {
            on_click = callback;
        };
        void bindEventOnDBClick(void (*callback)()) {
            on_db_click = callback;
        };
        void bindEventOnLongClick(void (*callback)()) {
            on_long_click = callback;
        };
        void bindEventOnLongPress(void (*callback)()) {
            on_long_press = callback;
        };

        void loop() {
            switch(getKeyAction()) {
                case    KEY_DOWN:
                    Serial.println("KEY_DOWN");
                    keyDownProc();
                    break;
                case    KEY_UP:
                    Serial.println("KEY_UP");
                    keyUpProc();
                    break;
                default:
                    keyNoChange();
                    break;
            }
        };
        
    private:
        uint8_t     BtnPin;
        uint8_t     KeyDown = DEF_KEY_DOWN;
        uint8_t     KeyUp = DEF_KEY_UP;

        uint16_t    EliminatingJitterMs = DEF_ELIMINATING_JITTER_MS;
        uint16_t    LongClickMS = DEF_DB_INTERVAL_MS;
        uint16_t    LongStartMS = DEF_LONG_PRESS_START_MS;
        uint16_t    LongIntervalMS = DEF_LONG_PRESS_INTERVAL_MS;
        uint32_t    LongPressNextMS = DEF_LONG_PRESS_START_MS;
        uint16_t    DblClickIntervalMS = DEF_DB_INTERVAL_MS;

        // 计时器
        uint32_t    KeyDownTimer = 0;
        uint32_t    LastKeyDownTimer = 0;
        uint32_t    KeyUpTimer = 0;
        uint8_t     KeyStatus = DEF_KEY_UP;
        bool        isDone = true;

        // event callback function ptr
        void (*on_click)() = nullptr;
        void (*on_db_click)() = nullptr;
        void (*on_long_click)() = nullptr;
        void (*on_long_press)() = nullptr;

        KeyAction getKeyAction() {
            uint8_t gpio_v = digitalRead(BtnPin);
            if(gpio_v == KeyStatus) return NO_CHANGE;
            KeyStatus = gpio_v;
            if(gpio_v == KeyDown) return KEY_DOWN;
            else return KEY_UP;
        };
        void keyDownProc() {
            LastKeyDownTimer = KeyDownTimer;
            KeyDownTimer = millis();
            isDone = false;
        };
        void keyUpProc() {
            KeyUpTimer = millis();
        };
        void keyNoChange() {
            uint32_t nowTimer = millis();
            uint32_t fromKeyDown = nowTimer - KeyDownTimer;
            // Serial.println("isDone=" + String(isDone));
            
            // 按键按下状态
            if(KeyStatus == KeyDown) {
                if(fromKeyDown < EliminatingJitterMs) return;
                if(!isDone && on_long_click && fromKeyDown > LongClickMS) {
                    // 触发on_long_click事件
                    on_long_click();
                    isDone = true;
                    return;
                }
                else if(on_long_press && fromKeyDown > LongPressNextMS) {
                    // 触发on_long_press事件
                    isDone = true;
                    on_long_press();
                    LongPressNextMS += LongIntervalMS;
                }
                else if(!isDone && on_db_click && LastKeyDownTimer && nowTimer < LastKeyDownTimer + DblClickIntervalMS) {
                    // 触发on_db_click事件
                    isDone = true;
                    on_db_click();
                }
            }
            // 按键释放状态
            else {
                Serial.println("KeyStatus=" + String(KeyStatus));
                
                uint32_t fromKeyUp = nowTimer - KeyUpTimer;
                if(fromKeyUp < EliminatingJitterMs) return;
                
                if(!isDone && on_click && nowTimer > KeyDownTimer + DblClickIntervalMS) {
                    isDone = true;
                    on_click();
                }
            }
        };
};

// class GpioButton {
//     public:
//         GpioButton(uint8_t gpio_pin, void(*btn_press_event)()=nullptr) :
//             GpioPin(gpio_pin), 
//             ButtonPressEvent(btn_press_event), 
//             LongPressWaitMS(DEF_LONG_PRESS_WAIT_MS), 
//             ButtonLongPressEvent(nullptr),
//             first_key_down_millis(0),
//             first_key_up_millis(0),
//             action_done(false),
//             last_gpio_state(HIGH) {
//                 pinMode(GpioPin, INPUT_PULLUP);
//                 digitalWrite(GpioPin, HIGH);
//         };
//         // bind click event CB function
//         void BindBtnPress(void(*btn_press_event)()) {
//         	ButtonPressEvent = btn_press_event;
//         };
//         // bind long key press CB function
//         bool BindBtnLongPress(void(*btn_long_press_event)(), uint16_t wait_ms=DEF_LONG_PRESS_WAIT_MS) {
//             if(wait_ms < DEF_LONG_PRESS_WAIT_MS) return false;
//             ButtonLongPressEvent = btn_long_press_event;
//             LongPressWaitMS = wait_ms;
//             return true;
//         };
//         // bind double click CB function
//         void BindBtnDblPress(void(*btn_dbl_press_event)()) {
//             ButtonDblPressEvent = btn_dbl_press_event;
//         };
//         // loop function
//         void loop(){
            
//             uint8_t current_gpio_state = digitalRead(GpioPin);
//             uint32_t current_millis = millis();
            
//             // gpio status no change
//             if(current_gpio_state == last_gpio_state) {
//                 if(current_gpio_state == LOW) {
//                     if(first_key_down_millis && !first_key_up_millis && (current_millis - first_key_down_millis > LongPressWaitMS)) {
//                         if(!action_done && ButtonLongPressEvent != nullptr) {
//                             ButtonLongPressEvent();
//                             action_done = true;
//                         }
//                     }
//                 }
//                 else {
//                     if(first_key_up_millis && (current_millis - first_key_up_millis > DEF_DB_PRESS_MS)) {
//                         if(!action_done && ButtonPressEvent != nullptr) {
//                             // Serial.println("Debug:Press Event.");
//                             ButtonPressEvent();
//                             action_done = true;
//                         }
//                     }
//                 }
//             }
//             // gpio status changed
//             else {
//                 if(current_millis - last_jitter_millis > DEF_ELIMINATING_JITTER_MS) {
//                     // key down
//                     if(current_gpio_state == LOW) {
//                         // is first keydown in cycle
//                         if(0 == first_key_down_millis) {
//                             first_key_down_millis = current_millis;
//                             first_key_up_millis = 0;
//                             action_done = false;
//                         }
//                         // is not first key down in cycle
//                         else {
//                             // has define double click CB function
//                             if(nullptr != ButtonDblPressEvent){
//                                 // key down mill - last key up mill > elimination jitter interval
//                                 if(	0 != first_key_up_millis // is release key in event cycle
//                                     && (current_millis - first_key_up_millis) > DEF_ELIMINATING_JITTER_MS) {	// skip eliminating jitter
//                                     // is double click?
//                                     if(	false == action_done // did in event cycle?
//                                         && current_millis - first_key_up_millis < DEF_DB_PRESS_MS) {	// and 2nd click is in interval
//                                         // call double click event function
//                                         // Serial.println("Debug:Double Press Event.");
//                                         ButtonDblPressEvent();
//                                         action_done = true;
//                                     }
                                    
//                                 }
//                             }
//                         }
//                     }
//                     // key up
//                     else {
//                         if(!action_done && first_key_down_millis && first_key_up_millis == 0) {
//                             first_key_up_millis = current_millis;
//                         }
//                     }
//                     // Keep gpio status
//                     last_gpio_state = current_gpio_state;
//                     last_jitter_millis = current_millis;
//                 }
//             }
            
//             if(action_done && current_gpio_state == HIGH) {
//                 // Serial.println("Event Reset.");
//                 first_key_down_millis = 0;
//                 first_key_up_millis = 0;
//                 action_done = false;
//             }
//         };
//     protected:
//         uint8_t GpioPin;					        // gpio pin of key
//         void (*ButtonPressEvent)();       // Click Event CB function
//         uint16_t LongPressWaitMS;			    // Long press ms
//         void (*ButtonLongPressEvent)();		// Long press Event CB function
//         void (*ButtonDblPressEvent)();		// Double click Event CB function
//         uint32_t first_key_down_millis;
//         uint32_t first_key_up_millis;
//         bool action_done;
//         uint8_t last_gpio_state;
//         uint32_t last_jitter_millis;
// };

#endif
