#include <GpioKeyEvent.h>

#define BUTTON_1_PIN 32
#define BUTTON_2_PIN 33

GpioButton Btn1(BUTTON_1_PIN);
GpioButton Btn2(BUTTON_2_PIN);

uint32_t start_time = 0;

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

static uint32_t to = 0;
typedef struct {
	uint32_t ms;
	uint8_t	status;
} btnActions;

btnActions actions[] = {
	{1000, DEF_KEY_DOWN},
	{100, DEF_KEY_UP},
	{100, DEF_KEY_UP},
	{100, DEF_KEY_UP},
};
int action_index = 0;
int action_cnt = 4;

void setup() {

    Serial.begin(115200);
	Serial.println("Start");
	// bind button callback event function
	// Btn1.BindBtnPress(btn_1_click_event);
	// Btn1.BindBtnDblPress(btn_1_db_click_event);
	// Btn1.BindBtnLongPress(btn_1_long_press_event, 1500);

	// Btn2.BindBtnPress(btn_2_click_event);
	// Btn2.BindBtnDblPress(btn_2_db_click_event);
	// Btn2.BindBtnLongPress(btn_2_long_press_event, 2000);

	Btn1.bindEventOnClick(btn_1_click_event);
	Btn1.bindEventOnDBClick(btn_1_db_click_event);
	Btn1.bindEventOnLongClick(btn_1_long_press_event);

	digitalWrite(BUTTON_1_PIN, DEF_KEY_UP);
	to = millis() + actions[action_index].ms;
}

void keyEventLoop() {
    Btn1.loop();
    // Btn2.loop();
}

void actionLoop() {
	if(action_index >= action_cnt) return;
	if(millis() > to) {
		Serial.println("TimeOut");
		Serial.println("Btn GPIO Status set to " + String(actions[action_index].status));
		Serial.println("action_index=" + String(action_index));
		digitalWrite(BUTTON_1_PIN, actions[action_index].status);
		action_index++;
		// action_index %= action_cnt;
		if(action_index >=action_cnt) return;

		to = millis() + actions[action_index].ms;
	}
	delay(50);
}

void loop(){
	actionLoop();
    // Key Event
    keyEventLoop();
		
	// ...
}
