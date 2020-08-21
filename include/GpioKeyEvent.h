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
            pinMode(BtnPin, INPUT_PULLUP);
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
        void setEliminatingJitterMs(uint16_t _ms) {EliminatingJitterMs = _ms;};
        void setLongClickMS(uint16_t _ms) {LongClickMS = _ms;};
        void setLongStartMS(uint16_t _ms) {LongStartMS = _ms;};
        void setLongIntervalMS(uint16_t _ms) {LongIntervalMS = _ms;};
        void setLongPressNextTimeOut(uint32_t _to) {LongPressNextTimeOut = _to;};
        void setDblClickIntervalMS(uint16_t _ms) {DblClickIntervalMS = _ms;};

        uint16_t getEliminatingJitterMs() { return EliminatingJitterMs;};
        uint16_t getLongClickMS() {return LongClickMS;};
        uint16_t getLongStartMS() {return LongStartMS;};
        uint16_t getLongIntervalMS() {return LongIntervalMS;};
        uint32_t getLongPressNextTimeOut() {return LongPressNextTimeOut;};
        uint16_t getDblClickIntervalMS() {return DblClickIntervalMS;};

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
        uint32_t    LongPressNextTimeOut = 0;
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
            uint32_t tmpTimer = millis();
            if(tmpTimer > KeyDownTimer + EliminatingJitterMs) {
                LongPressNextTimeOut = tmpTimer + LongStartMS;
                LastKeyDownTimer = KeyDownTimer;
                KeyDownTimer = tmpTimer;
                isDone = false;
            }
        };
        void keyUpProc() {
            uint32_t tmpTimer = millis();
            if(tmpTimer > KeyUpTimer + EliminatingJitterMs) 
                KeyUpTimer = tmpTimer;
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
                else if(on_long_press && fromKeyDown > LongPressNextTimeOut) {
                    // 触发on_long_press事件
                    isDone = true;
                    on_long_press();
                    LongPressNextTimeOut += LongIntervalMS;
                }
                else if(!isDone && on_db_click && LastKeyDownTimer && nowTimer < LastKeyDownTimer + DblClickIntervalMS) {
                    // 触发on_db_click事件
                    isDone = true;
                    on_db_click();
                }
            }
            // 按键释放状态
            else {
                // Serial.println("KeyStatus=" + String(KeyStatus));
                
                uint32_t fromKeyUp = nowTimer - KeyUpTimer;
                if(fromKeyUp < EliminatingJitterMs) return;
                
                if(!isDone && on_click && nowTimer > KeyDownTimer + DblClickIntervalMS) {
                    isDone = true;
                    on_click();
                }
            }
        };
};

#endif