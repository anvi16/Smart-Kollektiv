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

void display_screensaver(int temperature, const char* line1){  // Function for displaying screensaver
   TFT_eSPI tft_display_screensaver = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

   char buffer[10];
   tft_display_screensaver.drawCentreString(line1, cntr_x , cntr_y - 45, 4);
   tft_display_screensaver.drawCentreString(itoa(temperature, buffer, 10), cntr_x - 5 , cntr_y+32, 6);
   tft_display_screensaver.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);
   tft_display_screensaver.drawCentreString("°C", cntr_x + 50 , cntr_y +40, 4);}


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
// Function drawing a single menu icon. Must be called for all "buttons" in a menu
void menu_icon(int item_number, int selected_number, std::string item_name, int sel_color){
    TFT_eSPI tft_menu_icon = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

    int my_loc = (item_number - selected_number);  // Location of current item
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

*/