/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description:
        Doorbell
*************************************************/


#ifndef DOORBELL.H
#define DOORBELL.H

#include "MQTT_Class.h"
#include "Oled_display.h"

extern void Access_panel_open_door(); // Import Open door

const uint8_t user_array_length = USERS;
const uint8_t button_up         = BUTTON_UP;
const uint8_t button_enter      = BUTTON_ENTER;
const uint8_t button_down       = BUTTON_DOWN;

uint32_t last_debounce_time = 0; 
int      debounce_delay     = 20;

uint32_t wait_for_responce_timer;
int      wait_for_responce_timer_delay = 10000; // 10 Sec

bool last_button_up_state    = LOW;
bool last_button_enter_state = LOW;
bool last_button_down_state  = LOW;
bool button_change           = LOW;

bool button_up_filtered;
bool button_enter_filtered;
bool button_down_filtered;

bool wait_for_responce;


uint8_t index_user_array = 0;
String user_names[user_array_length] = { "Ole", "Lise", "Kari", "Henrik", "Karro", "Simen" };


Mqtt_message doorbell_message;
MQTT* mqtt_doorbell;


// pre declare functions
void Doorbell_setup(MQTT& _mqtt);
void Doorbell_loop();
void Doorbell_show();
void Doorbell_call_user(String user);
void Doorbell_scorll();
void Doorbell_send(String user);
void Doorbell_recive(bool reply);
void Doorbell_timeout();
void Doorbell_refuse();
void Doorbell_accept();




void Doorbell_setup(MQTT& _mqtt) {

    mqtt_doorbell = &_mqtt;

	pinMode(button_up, INPUT);
	pinMode(button_enter, INPUT);
	pinMode(button_down, INPUT);

    Oled_display_setup();
    Doorbell_show();
}



void Doorbell_loop() {
    
    int reading1 = digitalRead(button_up);
    int reading2 = digitalRead(button_enter);
    int reading3 = digitalRead(button_down);

    
//////////////////////////////////////////////
 // Add debounce
    
    if (reading1 != last_button_up_state) {
     // Reset the debouncing timer
        last_debounce_time = millis();
    }

    if (reading2 != last_button_enter_state) {
     // Reset the debouncing timer
        last_debounce_time = millis();
    }

    if (reading3 != last_button_down_state) {
     // Reset the debouncing timer
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

     // Call user
        if (button_enter_filtered) {
            Doorbell_call_user(user_names[index_user_array]);

        }

     // Scroll Up
        else if (button_up_filtered) {
            if (index_user_array > 0) {
                index_user_array--;
                Doorbell_scorll();
            }
        }

     // Scroll Down
        else if (button_down_filtered) {
            if (index_user_array < user_array_length - 1) {
                index_user_array++;
                Doorbell_scorll();
            }
        }

        button_change = false; 
    }

 // Display timeout if user does not respond in time 
    if (wait_for_responce_timer + wait_for_responce_timer_delay < millis() && wait_for_responce) {
        Doorbell_timeout();

     // Reset doorbell
        index_user_array = 0;
        wait_for_responce = false; 
        Doorbell_show();
    }
}



void Doorbell_show() {
    Oled_display_text_clear(">", 2, 2, 2);

    for (int i = 0; i < 6; i++) {
        display.setCursor(16, i * 17 + 2);
        display.println(user_names[i]);
    }
    display.display();

}



void Doorbell_call_user(String user) {
    size_t len = (user.length());

    Oled_display_text_clear("Calling:", 17, 13, 2);
    Oled_display_text(user, SCREEN_WIDTH / 2 - len * 6, 35, 2);
    
    display.display();

    Doorbell_send(user);

    wait_for_responce = true;
    wait_for_responce_timer = millis();

}



void Doorbell_scorll() {
    Oled_display_text_clear(">", 2, 2, 2);

    display.setCursor(10, 3);
    for (int i = 0; i < 6 - index_user_array; i++) {
        display.setCursor(16, i * 17 + 2);
        display.println(user_names[i + index_user_array]);
    }
    display.display();
}



void Doorbell_send(String user) {
    doorbell_message.resiver = user.c_str();
    doorbell_message.room = Entry;
    doorbell_message.header = Doorbell;

    mqtt_doorbell->pub(doorbell_message, MQTT_TOPIC, false);
}



void Doorbell_recive(bool reply) {
    if (reply) {
        #ifdef DEBUG
            Serial.println("Door open");
        #endif

        Access_panel_open_door();
        Doorbell_accept();
    }
    else {
        #ifdef DEBUG
            Serial.println("Door refused");
        #endif

        Doorbell_refuse();
    }

 // Reset
    index_user_array = 0;
    wait_for_responce = false; 
    Doorbell_show();
}



void Doorbell_timeout() {
    size_t len = user_names[index_user_array].length();
    Oled_display_text_clear(user_names[index_user_array] + " is", SCREEN_WIDTH / 2 - len * 6 - 16, 13, 2);
    Oled_display_text("not home", 17, 35, 2);
    display.display();
    delay(3000);
}



void Doorbell_refuse() {
    Oled_display_text_clear("Go away", 23, 22, 2);
    display.display();
    delay(3000);
}



void Doorbell_accept() {
    Oled_display_text_clear("Welcome", 23, 22, 2);
    display.display();
    delay(3000);
}


#endif
