#include "OLED.h"
#include "Controller_config.h"

// Function drawing a single menu icon. Must be called for all "buttons" in a menu
void menu_icon(int item_number, int current_number, std::string item_name, int sel_color, int selected_number){
    TFT_eSPI tft_menu_icon = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

    int my_loc = (item_number - current_number);  // Location of current item
    tft_menu_icon.setTextSize(1);

    tft_menu_icon.drawCentreString(item_name.c_str(), cntr_x , cntr_y + my_loc*space_y, 4);
    tft_menu_icon.drawRoundRect(rect_in, cntr_x - (rect_h/2) + my_loc*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);

    if (selected_number == item_number){
        // Highlight frame if selected
        tft_menu_icon.drawRoundRect(rect_in+2, cntr_x-(rect_h/2)+2 + my_loc*space_y, rect_w - 4 , rect_h -4, 10, sel_color);
        tft_menu_icon.drawRoundRect(rect_in+1, cntr_x-(rect_h/2)+1 + my_loc*space_y, rect_w - 2 , rect_h -2, 10, sel_color);
        tft_menu_icon.drawRoundRect(rect_in-1, cntr_x-(rect_h/2)-1 + my_loc*space_y, rect_w + 2 , rect_h +2, 10, sel_color);
        tft_menu_icon.drawRoundRect(rect_in-2, cntr_x-(rect_h/2)-2 + my_loc*space_y, rect_w + 4 , rect_h +4, 10, sel_color);
    }
}

// Function drawing a window dedicated to adjusting values (typically altering a percentage value)
void val_adjust_window(int variable, const char* header){
    
    TFT_eSPI().drawCentreString(header, cntr_x , cntr_y - 1*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) - 1*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);   

    char buffer[10];
    TFT_eSPI().setTextSize(5);
    TFT_eSPI().drawCentreString("-", cntr_x , cntr_y + 90, 4); 
    TFT_eSPI().drawCentreString(itoa(variable, buffer, 10), cntr_x - 5 , cntr_y+32, 6);
    TFT_eSPI().drawCentreString("+", cntr_x , cntr_y - 10, 4); 
    TFT_eSPI().drawCentreString("%", cntr_x + 50 , cntr_y +40, 4);
}

// Function drawing a window dedicated to adjusting values (typically altering a percentage value)
void temp_adjust_window(int variable, const char* header){
    
    TFT_eSPI().drawCentreString(header, cntr_x , cntr_y - 1*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) - 1*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);   

    char buffer[10];
    TFT_eSPI().setTextSize(5);
    TFT_eSPI().drawCentreString("-", cntr_x , cntr_y + 90, 4); 
    TFT_eSPI().drawCentreString(itoa(variable, buffer, 10), cntr_x - 5 , cntr_y+32, 6);
    TFT_eSPI().drawCentreString("+", cntr_x , cntr_y - 10, 4); 
    TFT_eSPI().drawCentreString("°C", cntr_x + 50 , cntr_y +40, 4);
}

// Function displaying a popup that gives the state of a bool variable
void toggle_popup(bool var_state, const char* header){                          
  
  TFT_eSPI tft_toggle_popup = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

  if (var_state){ 
    tft_toggle_popup.drawCentreString(header, cntr_x , cntr_y - 1*space_y, 4);
    tft_toggle_popup.fillRoundRect(rect_in, cntr_x - (rect_h/2) - 0*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);
    tft_toggle_popup.setTextColor(TFT_BLACK, TFT_SKYBLUE);
    tft_toggle_popup.drawCentreString("ON", cntr_x , cntr_y, 4);
    tft_toggle_popup.setTextColor(TFT_SKYBLUE, TFT_BLACK); // reset text color to normal
    delay(800); // Show message for 0.8sec before turning screen black again
    tft_toggle_popup.fillScreen(TFT_BLACK);
  }  

  else {
    tft_toggle_popup.drawCentreString(header, cntr_x , cntr_y - 1*space_y, 4);
    tft_toggle_popup.drawRoundRect(rect_in, cntr_x - (rect_h/2) - 0*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);
    tft_toggle_popup.drawCentreString("OFF", cntr_x, cntr_y, 4);
    delay(800); // Show message for 0.8sec before turning screen black again
    tft_toggle_popup.fillScreen(TFT_BLACK);

  }
}

// Function for displaying a complete menu based on a vector containing cathegories
void display_menu(int level_val, std::vector<std::string> item_vector){   
  
  int vector_len = item_vector.size();          // Find amount of elements in vector
  for (int i = 0; i < vector_len; i++){         // Draw to OLED one box with the respective menu item for each menu item in the vector
    menu_icon(i+1, level_val, item_vector[i], TFT_SKYBLUE, level_val);  
  }
}

// Function reading "up" and "down" buttons to alter either the menu position or increase / decrease a value
// If menu navigation, val_adj = LOW, if value adjustment +/- 5%, val_adj = HIGH
int mod_val(bool bottom_reached, bool percent_adj, bool pos_edge_up, bool pos_edge_dwn, int value){   
    
    // If the request concerns a percentage value
    if (percent_adj){
        if ((pos_edge_dwn) && (value > 0)) {
            value -= 5;
            TFT_eSPI().fillScreen(TFT_BLACK);    
        }
        else if ((pos_edge_up) && (value < 100)){
            value +=5;
            TFT_eSPI().fillScreen(TFT_BLACK);
        }
    }
    
    // If the request concerns menu navigation
    else {
        if ((pos_edge_dwn) && (!bottom_reached)){
            value ++;
            TFT_eSPI().fillScreen(TFT_BLACK);    
        }
        else if ((pos_edge_up) && (value > 1)){
            value --;
            TFT_eSPI().fillScreen(TFT_BLACK);
        }
    }
    return value;
}

// Function reading "up" and "down" buttons to alter a temperature value
int mod_temp(bool pos_edge_up, bool pos_edge_dwn, int value){   
    
  if ((pos_edge_dwn) && (value > 5)) {
    value --;
    TFT_eSPI().fillScreen(TFT_BLACK);    
  }
  else if ((pos_edge_up) && (value < 35)){
    value ++;
    TFT_eSPI().fillScreen(TFT_BLACK);
  }
  return value;
}

void display_message(int displaytime_ms, char* line1, char* line2, char* line3){  // Function for displaying a message on screen for x milliseconds
   TFT_eSPI tft_display_message = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

  
   tft_display_message.drawCentreString(line1, cntr_x , cntr_y -35, 4);
   tft_display_message.drawCentreString(line2, cntr_x , cntr_y + 0, 4);
   tft_display_message.drawCentreString(line3, cntr_x , cntr_y +35, 4);
   tft_display_message.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);
   delay(displaytime_ms); // Show message for 0.8sec before turning screen black again
   tft_display_message.fillScreen(TFT_BLACK);}

void display_setup_screen(){  // Function for displaying setup screen while setting up controller
   TFT_eSPI tft_display_setup_screen = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

   tft_display_setup_screen.drawCentreString("Setting up",      cntr_x , cntr_y - 45, 4);
   tft_display_setup_screen.drawCentreString("system",          cntr_x , cntr_y - 10, 4);
   tft_display_setup_screen.drawCentreString("Please wait...",  cntr_x , cntr_y + 45, 4);
   tft_display_setup_screen.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);}

void display_reconnecting_screen(){  // Function for displaying setup screen while setting up controller
   TFT_eSPI tft_display_reconnecting_screen = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

   tft_display_reconnecting_screen.drawCentreString("Re-establishing", cntr_x , cntr_y - 45, 4);
   tft_display_reconnecting_screen.drawCentreString("WiFi connection", cntr_x , cntr_y - 10, 4);
   tft_display_reconnecting_screen.drawCentreString("Please wait...",  cntr_x , cntr_y + 45, 4);
   tft_display_reconnecting_screen.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);}

void display_setup_messages(const char* line1, const char* line2, const char* line3){  // Function for displaying setup screen with custom text
   TFT_eSPI tft_display_reconnecting_screen = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

   tft_display_reconnecting_screen.drawCentreString(line1, cntr_x , cntr_y - 45, 4);
   tft_display_reconnecting_screen.drawCentreString(line2, cntr_x , cntr_y - 10, 4);
   tft_display_reconnecting_screen.drawCentreString(line3,  cntr_x , cntr_y + 45, 4);
   tft_display_reconnecting_screen.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);}   

void display_screensaver(int temperature, const char* line1, int omm, int oss){  // Function for displaying screensaver
  TFT_eSPI tft_display_screensaver = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc
  int item = 0;

  if (second() != oss){   // Clear screen every second
    tft_display_screensaver.fillScreen(TFT_BLACK);                                     // Clear screen every minute                                                                
  }

  // Interval of 10 seconds between toggle from 
  if      ((second() >= 0)  && (second() < 10)){item = 0;}
  else if ((second() >= 10) && (second() < 20)){item = 1;}
  else if ((second() >= 20) && (second() < 30)){item = 0;}
  else if ((second() >= 30) && (second() < 40)){item = 1;}
  else if ((second() >= 40) && (second() < 50)){item = 0;}
  else if ((second() >= 50) && (second() < 60)){item = 1;}

  std::vector<std::string>weekdays{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  int week_day = weekday() -1;  // -1 because 1-indexed

  int xpos = 55;
  int ypos = 90;

  int hh    = hour();
  int mm    = minute();
  int DD    = day();
  int MM    = month();
  int YYYY  = year();

  // FRAME
  tft_display_screensaver.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);

  // WEEKDAY
  tft_display_screensaver.drawCentreString(weekdays[week_day].c_str(), cntr_x, 60, 4);


  // DATE

  if (DD < 10){ xpos += tft_display_screensaver.drawChar  ('0', xpos, ypos, 4);}          // Add hours leading zero for 24 hr clock // Copied from TFT_eSPI example code
                xpos += tft_display_screensaver.drawNumber(DD,  xpos, ypos, 4);           // Draw day

                xpos += tft_display_screensaver.drawChar  ('-', xpos, ypos, 4);
  if (MM < 10){ xpos += tft_display_screensaver.drawChar  ('0', xpos, ypos, 4);}          // Add hours leading zero for 24 hr clock // Copied from TFT_eSPI example code
                xpos += tft_display_screensaver.drawNumber(MM,  xpos, ypos, 4);           // Draw month

                xpos += tft_display_screensaver.drawChar  ('-', xpos, ypos, 4);
                xpos += tft_display_screensaver.drawNumber(YYYY,xpos, ypos, 4);           // Draw year


  if (item == 1){
    // Clock
    xpos = 55;  // Redefine xpos because has incremented during showing date
    tft_display_screensaver.setTextSize(2);
    if (hh < 10){ xpos += tft_display_screensaver.drawChar  ('0', xpos, cntr_y+25, 4);}   // Add hours leading zero for 24 hr clock // Copied from TFT_eSPI example code
                  xpos += tft_display_screensaver.drawNumber(hh,  xpos, cntr_y+25, 4);    // Draw hours
                  xpos += tft_display_screensaver.drawChar  (':', xpos, cntr_y+25, 4);
    if (mm < 10){ xpos += tft_display_screensaver.drawChar  ('0', xpos, cntr_y+25, 4);}   // Add minutes leading zero
                  xpos += tft_display_screensaver.drawNumber(mm,  xpos, cntr_y+25, 4);    // Draw minutes
  }

  else{                                                                                 // Display Temperature
    // Temperature
    char buffer[10];
    //tft_display_screensaver.drawCentreString(line1, cntr_x , cntr_y - 25, 4);
    tft_display_screensaver.drawCentreString(itoa(temperature, buffer, 10), cntr_x - 30 , cntr_y +25, 6);
    tft_display_screensaver.drawCentreString("o", cntr_x + 10 , cntr_y +20, 4);
    tft_display_screensaver.setTextSize(2);
    tft_display_screensaver.drawCentreString("C", cntr_x + 40 , cntr_y +25, 4);
  }
} 
   


   

// Function for displaying a complete menu based on a vector containing cathegories
void display_weekplan_setting(int level_val, int sel_icon, std::vector<std::string> item_vector){   // sel_lvl = selected level (pops out in green or blue)
  
  TFT_eSPI tft_display_weekplan_mode = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc
  
  int vector_len = item_vector.size();          // Find amount of elements in vector
  for (int i = 0; i < vector_len; i++){         // Draw to OLED one box with the respective menu item for each menu item in the vector
    menu_icon(i+1, level_val, item_vector[i],TFT_BLUE, sel_icon);  
  };
  tft_display_weekplan_mode.fillTriangle(5,123, 5,117, 15,120, TFT_SKYBLUE);

}

  
  /*
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.print(weekday());
  Serial.print(" - ");
  
  Serial.print(year());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(day());
  Serial.print(" : ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
  */


 