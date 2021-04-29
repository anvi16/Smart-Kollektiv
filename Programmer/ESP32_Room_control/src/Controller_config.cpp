#include <Arduino.h>
#include "Controller_config.h"

// Function for setting up hardware (pushbuttons, temp sensor, light sensor)
void hw_setup(){
    // Pinmode for input pins
    pinMode(pin_up, INPUT_PULLUP);
    pinMode(pin_dwn, INPUT_PULLUP);
    pinMode(pin_lft, INPUT_PULLUP);
    pinMode(pin_rgt, INPUT_PULLUP);
    
    pinMode(pin_temp_sensor, INPUT);
    pinMode(pin_light_sensor, INPUT);

    pinMode(pin_heating_LED, OUTPUT);
    pinMode(pin_LED_room, OUTPUT);

    analogReadResolution(8);
    ledcSetup(CHANNEL_LED, 5000, 8);
    ledcAttachPin(pin_LED_room, CHANNEL_LED);
}

