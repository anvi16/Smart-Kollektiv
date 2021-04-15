
#ifndef CONTROLLER_CONFIG.H
#define CONTROLLER_CONFIG.H

#define pin_up      26
#define pin_dwn     12
#define pin_lft     14
#define pin_rgt     27

#include <vector>
#include <stdlib.h>
#include <ESP32_Servo.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

// Pre-declare functions
void hw_setup();
void tft_setup();

#endif