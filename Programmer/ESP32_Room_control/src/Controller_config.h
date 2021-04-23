
// #ifndef CONTROLLER_CONFIG.H
// #define CONTROLLER_CONFIG.H


#define pin_heating_LED     12
#define pin_temp_sensor     13
#define pin_LED_room        14 
#define pin_light_sensor    25  
#define pin_up              34
#define pin_dwn             35
#define pin_lft             32
#define pin_rgt             33


// PWM channels (for ledc)
#define CH1                 1
#define CH2                 2       
#define CH3                 3



// CoT parameters
#define CoT_refresh_interval second




#include <vector>               // Vector library
#include <iostream>
#include <stdlib.h>             // Library containing std namespace functions
#include <ESP32_Servo.h>        // Library for servo motor control
#include <SPI.h>                // TFT_eSPI is based on this library
#include <TFT_eSPI.h>           // Library for controlling OLED
#include <CircusESP32Lib.h>     // Library for Circus of Things (CoT)

// Pre-declare functions
void hw_setup();


// #endif