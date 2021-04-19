
// #ifndef CONTROLLER_CONFIG.H
// #define CONTROLLER_CONFIG.H

#define pin_heating_LED     12
#define pin_temp_sensor     13
#define pin_light_sensor    25
#define pin_LED_room        26   
#define pin_up              34
#define pin_dwn             35
#define pin_lft             32
#define pin_rgt             33

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <ESP32_Servo.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

// Pre-declare functions
void hw_setup();
void tft_setup();

// #endif