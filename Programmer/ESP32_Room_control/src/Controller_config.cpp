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
}

void tft_setup(){
    TFT_eSPI().init();
    TFT_eSPI().setRotation(1);
    TFT_eSPI().fillScreen(TFT_BLACK);
    TFT_eSPI().setTextColor(TFT_SKYBLUE, TFT_BLACK);
    TFT_eSPI().setTextSize(1);
}

int navigate_up_dn(int menu_lvl_val, bool pos_edge_up, bool pos_edge_dwn){
    // if 

}
