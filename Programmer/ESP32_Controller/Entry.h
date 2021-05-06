#ifndef ENTRY.H
#define ENTRY.H

#include "Oled_display.h"

Mqtt_callback mqtt_doorbell_message;

void Doorbell_show_request(byte answer);
void Doorbell_send_responce(int user, byte answer);

void Entry_doorbell(int user) {
	byte button_left = 5;
	byte button_right = 6;
	byte button_enter = 7;

	// int Buzzer;

	pinMode(button_left, INPUT_PULLUP);
	pinMode(button_right, INPUT_PULLUP);
	pinMode(button_enter, INPUT_PULLUP);

	// Ask user to open door
	bool wait_for_answer = true;
	byte answer = 1;
	Doorbell_show_request(answer);

	while (wait_for_answer) {
		if (digitalRead(button_left)){
			if (answer == 0) {
				answer++;
				Doorbell_show_request(answer);
			}
		}
		else if (digitalRead(button_right)) {
			if (answer == 1) {
				answer--;
				Doorbell_show_request(answer);
			}
		}
		else if (digitalRead(button_enter)) {
			//Return answer
			Doorbell_send_responce(user);
			wait_for_answer = false;
		}

		// Make buzze sounds
	}
}

void Doorbell_show_request(byte answer) {
	Oled_display_text_clear("Do you want to", 20, 10, 1);
	Oled_display_text("open the door", 25, 20, 1);
	Oled_display_text("Yes", 30, 40, 1);
	Oled_display_text("No", 80, 40, 1);
	if (answer) Oled_display_line(31, 49, 16); // Yes
	else Oled_display_line(79, 49, 13); // No
    display.display();
}

void Doorbell_send_responce(int user, byte answer) {
	mqtt_doorbell_message.resicer = user;
	mqtt_doorbell_message.room = entry;
	mqtt_doorbell_message.header = Doorbell;
	mqtt_doorbell_message.int_data[0] = { "reply", answer };

	Mqtt_pub(&mqtt_doorbell_message);
}
#endif
