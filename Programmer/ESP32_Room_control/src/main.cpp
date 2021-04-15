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
bool arw_up, arw_dwn, arw_lft, arw_rgt;   // Declare values representing button states
int targetTime;

void setup() {

  hw_setup();           // Setup hardware for room controller
  tft_setup();          // Setup parameters for OLED

  Serial.begin(115200); // Serial communication
  
  int targetTime = 0; 
}

const char * main_cathegories[4] = {"Temperature", "Light", "Fan", "Airing"};
const char* test_word1 = "LIGHT";
const char* test_word2 = "TEMPERATURE";
const char* test_word3 = "FAN";

static const char* const titles[] = {"Aa", "Bb", "Cc", "Dd"};

const int lengde = countof(titles);


void loop() {

  // Read navigation buttons
  arw_up = digitalRead(pin_up);
  arw_dwn = digitalRead(pin_dwn);
  arw_lft = digitalRead(pin_lft);
  arw_rgt = digitalRead(pin_rgt);

  if (targetTime <= millis()){
    targetTime = millis() + 1000;
    Serial.println(arw_up);

    OLED_menu(1, titles);
    


      // ADJUST % WINDOW
      // drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color),
      
      TFT_eSPI().drawCentreString(test_word2, 120, 120-10, 4);

      bool selected = LOW;
      int active_element = 1;
      if (selected) { 
        // Make outline of rectangle thicker
        box_selected(active_element);
        // Make outline three times thicker to show it is selected
        
      }

      TFT_eSPI().drawRoundRect(20+1, 95+1, 200-2, 50-2, 10, TFT_SKYBLUE);
      TFT_eSPI().drawRoundRect(20+0, 95+0, 200+0, 50+0, 10, TFT_SKYBLUE);
      TFT_eSPI().drawRoundRect(20-1, 95-1, 200+2, 50+2, 10, TFT_SKYBLUE);
      
      TFT_eSPI().drawCentreString(test_word3, cntr_x, cntr_y + space_y, 4);
      TFT_eSPI().drawRoundRect(20,165,rect_w, rect_h, 10, TFT_SKYBLUE);

      TFT_eSPI().drawRoundRect(20,235,200, 50, 10, TFT_SKYBLUE);
    } 

}



