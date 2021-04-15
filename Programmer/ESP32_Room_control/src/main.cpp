/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	14/04/2021

Description: 
	Room controller
*************************************************/

#include "Controller_config.h"
#include "OLED.h"

// Declare 
bool buttonstate_up, buttonstate_dwn, buttonstate_lft, buttonstate_rgt;   // Declare values representing button states
int targetTime;

void setup() {

  hw_setup();           // Setup hardware for room controller
  tft_setup();          // Setup parameters for OLED

  Serial.begin(115200); // Serial communication
}

int sel_val_1 = 1;

// Main menu
const char * main_cathegories[] = {"Light", "Temperature", "Fan", "Airing"};

// Light control
const char * light_cathegories[] = {"Toggle", "Dim"};
const char * dim_cathegories[] = {""}; //////// NEEDS SEPARATE WINDOW SHOWING + AND -

// Temperature control
const char * temperature_cathegories[] = {"Temperature", "Weekly schedule", "Fan", "Airing"};

// Fan control 
const char * fan_cathegories[] = {"Override auto", "Fan power"};
//const char * fan_cathegories[] = {""};
//const char * fan_cathegories[] = {""};
//const char * fan_cathegories[] = {""};

// Declare and set initial button states 
bool prev_buttonstate_up = LOW;
bool prev_buttonstate_dwn = LOW;
bool prev_buttonstate_lft = LOW;
bool prev_buttonstate_rgt = LOW;

void loop() {

  // Read navigation buttons
  buttonstate_up = digitalRead(pin_up);
  buttonstate_dwn = digitalRead(pin_dwn);
  buttonstate_lft = digitalRead(pin_lft);
  buttonstate_rgt = digitalRead(pin_rgt);

  if ((buttonstate_dwn == HIGH) && (prev_buttonstate_dwn == LOW)){
    
    sel_val_1 ++;
    TFT_eSPI().fillScreen(TFT_BLACK);
  }

  if ((buttonstate_up == HIGH) && (prev_buttonstate_up == LOW)){
    if (sel_val_1 > 1){
      sel_val_1 --;
      TFT_eSPI().fillScreen(TFT_BLACK);
    }
  }

  Serial.println(sel_val_1);
  
  // menu_icon(1, sel_val_1, main_cathegories[0]);
  // menu_icon(2, sel_val_1, main_cathegories[1]);
  // menu_icon(3, sel_val_1, main_cathegories[2]);
  // menu_icon(4, sel_val_1, main_cathegories[3]);

  val_adjust_window(34);

  prev_buttonstate_up = buttonstate_up;
  prev_buttonstate_dwn = buttonstate_dwn;
  prev_buttonstate_lft = buttonstate_lft;
  prev_buttonstate_rgt = buttonstate_rgt;

  delay(50);
}


