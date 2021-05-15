#ifndef ENTRY.H
#define ENTRY.H

#include "MQTT_Class.h"
#include "OLED.h"

Mqtt_message entry_message;
MQTT* mqtt_entry;

void Doorbell_show_request(uint8_t answer);
void Doorbell_send_responce(uint8_t answer);


void Entry_setup(MQTT& _mqtt) {
  mqtt_entry = &_mqtt;

}


void Entry_doorbell() {
  
  int button_left = 27;
  int button_right = 14;
  int button_enter = 25;

  // Ask user to open door
  bool wait_for_answer = true;
  uint8_t answer = 1;
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
      Doorbell_send_responce(answer);
      wait_for_answer = false;
    }

    // Make buzze sounds
  }
}


void Doorbell_show_request(uint8_t answer) {
  display_text_clear("Do you want to", 40, 80, 2);
  display_text("open the door", 50, 100, 2);
  display_text("Yes", 62, 150, 2);
  display_text("No", 162, 150, 2);
  if (answer) display_line(65, 170, 30); // Yes
  else display_line(161, 170, 22); // No
}


void Doorbell_send_responce(uint8_t answer) {
  display_clear();

  entry_message.resiver = "Access_panel";
  entry_message.room = Toilet;
  entry_message.header = Doorbell;
  entry_message.data_int[0] = { "reply", answer };

  mqtt_entry->pub(entry_message);
}
#endif
