#ifndef DOORBELL.H
#define DOORBELL.H

#include "MQTT.h"
#include "Oled_display.h"

#define user_array_len 6
#define button_up      4 
#define button_enter   16 
#define button_down    17
#define door           15


uint32_t last_debounce_time = 0; 
int      debounce_delay     = 20;

bool last_button_up_state    = LOW;
bool last_button_enter_state = LOW;
bool last_button_down_state  = LOW;
bool button_change           = LOW;

bool button_up_filtered;
bool button_enter_filtered;
bool button_down_filtered;

bool wait_for_responce;


uint8_t index_user_array = 0;
String users[user_array_len] = { "Ole", "Lise", "Kari", "Henrik", "Karro", "Simen" };


Mqtt_message doorbell_message;


// pre declare functions
void Doorbell_show();
void Doorbell_call_user(String user);
void Doorbell_scorll();
void Doorbell_send(String user);
void Doorbell_recive(bool reply);




void Doorbell_setup() {
	pinMode(button_up, INPUT);
	pinMode(button_enter, INPUT);
	pinMode(button_down, INPUT);
    pinMode(door, OUTPUT);

    Doorbell_show();
}


void Doorbell_loop() {
    int reading1 = digitalRead(button_up);
    int reading2 = digitalRead(button_enter);
    int reading3 = digitalRead(button_down);
    
    //////////////////////////////////////////////
    // Add debounce
    
    if (reading1 != last_button_up_state) {
        // reset the debouncing timer
        last_debounce_time = millis();
    }

    if (reading2 != last_button_enter_state) {
        // reset the debouncing timer
        last_debounce_time = millis();
    }

    if (reading3 != last_button_down_state) {
        // reset the debouncing timer
        last_debounce_time = millis();
    }

    if ((millis() - last_debounce_time) > debounce_delay) {
        // Check if button state has changed
        if (reading1 != button_up_filtered) {
            button_up_filtered = reading1;
            button_change = true;
        }
        if (reading2 != button_enter_filtered) {
            button_enter_filtered = reading2;
            button_change = true;
        }
        if (reading3 != button_down_filtered) {
            button_down_filtered = reading3;
            button_change = true;
        }
    }

    last_button_up_state = reading1;
    last_button_enter_state = reading2;
    last_button_down_state = reading3;

    ////////////////////////////////////////////////

    // Handel user inputs and wait on call response
    if (button_change && !wait_for_responce) {
        if (button_enter_filtered) {
            Doorbell_call_user(users[index_user_array]);

        }
        else if (button_up_filtered) {
            if (index_user_array > 0) {
                index_user_array--;
                Doorbell_scorll();
            }
        }
        else if (button_down_filtered) {
            if (index_user_array < user_array_len - 1) {
                index_user_array++;
                Doorbell_scorll();
            }
        }
        button_change = false; // reset
    }

    if (wait_for_responce) {

    }
}

void Doorbell_show() {
    Oled_display_text_clear(">", 2, 2, 2);

    for (int i = 0; i < 6; i++) {
        display.setCursor(16, i * 17 + 2);
        display.println(users[i]);
    }
    display.display();

}

void Doorbell_scorll() {
    Oled_display_text_clear(">", 2, 2);

    display.setCursor(10, 3);
    for (int i = 0; i < 6 - index_user_array; i++) {
        display.setCursor(16, i * 17 + 2);
        display.println(users[i + index_user_array]);
    }
    display.display();
}


void Doorbell_call_user(String user) {
    size_t len = (user.length());

    Oled_display_text_clear("Calling:", 17, 13);
    Oled_display_text(user, SCREEN_WIDTH / 2 - len * 6, 35);
    
    display.display();

    Doorbell_send(user);

    wait_for_responce = true;

}


void Doorbell_send(String user) {
    doorbell_message.resiver = user.c_str();
    doorbell_message.room = Entry;
    doorbell_message.header = Doorbell;

    Mqtt_pub(&doorbell_message);
}


void Doorbell_recive(bool reply) {
    if (reply) {
        Oled_display_text_clear("Welcome", 23, 22);
        digitalWrite(door, HIGH);
        //delay(2000);
    }
    else {
        size_t len = users[index_user_array].length();
        Oled_display_text_clear(users[index_user_array] + " is", SCREEN_WIDTH / 2 - len * 6 - 16, 13);
        Oled_display_text("not home", 17, 35);
    }

    index_user_array = 0;
    wait_for_responce = false; // reset
    Doorbell_show();
}

#endif
