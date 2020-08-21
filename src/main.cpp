#include <GpioKeyEvent.h>

#define BUTTON_1_PIN D1

// 定义一个按键实体
GpioButton Btn1(BUTTON_1_PIN);

void btn_1_click_event() {
	Serial.println("<Event>Click");
}

void btn_1_db_click_event() {
	Serial.println("<Event>Double Click");
}

void btn_1_long_press_event() {
	Serial.println("<Event>Long Press Tick");
}

void btn_1_long_click_event() {
	Serial.println("<Event>Long Click");
}

void setup() {
	// 初始化串口用于输出调试信息
    Serial.begin(115200);
	Serial.println("Start");
	
	// 绑定事件回调函数
	Btn1.bindEventOnClick(btn_1_click_event);
	Btn1.bindEventOnDBClick(btn_1_db_click_event);
	Btn1.bindEventOnLongClick(btn_1_long_click_event);
	Btn1.bindEventOnLongPress(btn_1_long_press_event);

	// 直接新建回调函数
	Btn1.bindEventOnKeyDown([](){
		Serial.println("<Event>Key Down");
	});
	Btn1.bindEventOnKeyUp([](){
		Serial.println("<Event>Key Up");
	});

}

// 按键轮询函数
void keyEventLoop() {
    Btn1.loop();
}

void loop(){
    // Key Event
    keyEventLoop();
		
	// ...
}
