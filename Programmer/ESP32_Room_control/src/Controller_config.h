
//#ifndef CONTROLLER_CONFIG.H
//#define CONTROLLER_CONFIG.H

// Time constants
    #define SECOND              1000        // In milliseconds
    #define MINUTE              60  *SECOND
    #define HOUR                60  *MINUTE
    #define DAY                 24  *HOUR
    #define READTIME            2   *SECOND // should be 3

// Controller constants
    #define OUTPUT_MILLIVOLT    3300

// Peripherals
    
    // Pins - (Since ADC2 is busy with WiFi, pins  0, 2, 4, 12, 13, 14, 15, 25, 26, 27 are thereby are incapable of ADC.)
    #define pin_heating_LED     12    
    #define pin_up              14
    #define pin_fan             21
    #define pin_servo           22
    #define pin_lft             25
    #define pin_rgt             26
    #define pin_dwn             27
    #define pin_temp_sensor     32
    #define pin_LED_room        33
    #define pin_light_sensor    35
    
    // PWM
    #define CHANNEL_LED         9
    #define CHANNEL_FAN         10       
    #define CH3                 3   
 

// CoT - Circus of Things
    
    // Access
    #define TOKEN               "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTgzIn0.r1TT34J2y7Z9SHE0iB3HUjkWZoiRwiSBxaECZHlCgcI"
    #define COT_SERVER          "www.circusofthings.com"


    // Signal keys
    #define KEY_LIGHT_TOGGLE    "14408"
    #define KEY_LIGHT_DIMMING   "10213"

    #define KEY_TEMP_HOME       "7172"
    #define KEY_TEMP_AWAY       "32511"
    #define KEY_TEMP_NIGHT      "12735"
    #define KEY_TEMP_LONG_ABS   "16719"
    #define KEY_TEMP_MEASURED   "14340"

    #define KEY_FAN_POWER       "29862"

    #define KEY_AIRING_OPENING  "16586"
//

// Temperature
    #define HO                   1       // Home
    #define AW                   2       // Night
    #define NI                   3       // Away
//

#include <Arduino.h>
#include <vector>               // Vector library
#include <iostream>
#include <stdlib.h>             // Library containing std namespace functions
#include <ESP32_Servo.h>        // Library for servo motor control
#include <WiFi.h>               // WiFi library  
#include <SPI.h>                // TFT_eSPI is based on this library
#include <TFT_eSPI.h>           // Library for controlling OLED
#include <CircusESP32Lib.h>     // Library for Circus of Things (CoT)
#include <NTPClient.h>          // Library for Network Time Protocol client
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <time.h>


// Pre-declare functions
void hw_setup();


//#endif