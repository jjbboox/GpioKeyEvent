#include <GpioKeyEvent.h>

#define BUTTON_1_PIN 32
#define BUTTON_2_PIN 33

GpioButton Btn1(BUTTON_1_PIN);
GpioButton Btn2(BUTTON_2_PIN);

void btn_1_click_event() {
	Serial.println("Button_1 click Event.");
}

void btn_1_db_click_event() {
	Serial.println("Button_1 double click Event.");
}

void btn_1_long_press_event() {
	Serial.println("Button_1 long press Event.");
}

void btn_2_click_event() {
	Serial.println("Button_2 click Event.");
}

void btn_2_db_click_event() {
	Serial.println("Button_2 double click Event.");
}

void btn_2_long_press_event() {
	Serial.println("Button_2 long press Event.");
}

void setup() {

    Serial.begin(115200);
    
	// bind button callback event function
	Btn1.BindBtnPress(btn_1_click_event);
	Btn1.BindBtnDblPress(btn_1_db_click_event);
	Btn1.BindBtnLongPress(btn_1_long_press_event, 1500);

	Btn2.BindBtnPress(btn_2_click_event);
	Btn2.BindBtnDblPress(btn_2_db_click_event);
	Btn2.BindBtnLongPress(btn_2_long_press_event, 2000);
}

void keyEventLoop() {
    Btn1.loop();
    Btn2.loop();
}

void loop(){
    // Key Event
    keyEventLoop();
		
	// ...
}

