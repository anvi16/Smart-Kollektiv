#include <Arduino.h>
#include "Controller_config.h"

// Function for setting up hardware (pushbuttons, temp sensor, light sensor)
void hw_setup(){
    // Pinmode for input pins
    pinMode(pin_up, INPUT);
    pinMode(pin_dwn, INPUT);
    pinMode(pin_lft, INPUT);
    pinMode(pin_rgt, INPUT);

}

void tft_setup(){
    TFT_eSPI().init();
    TFT_eSPI().setRotation(1);
    TFT_eSPI().fillScreen(TFT_BLACK);
    TFT_eSPI().setTextSize(1);
    TFT_eSPI().setTextColor(TFT_SKYBLUE, TFT_BLACK);
}