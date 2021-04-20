#include "OLED.h"
#include "Controller_config.h"

// Function drawing a single menu icon. Must be called for all "buttons" in a menu
void menu_icon(int item_number, int selected_number, std::string item_name){
    
    int my_loc = (item_number - selected_number);  // Location of current item
    TFT_eSPI().setTextSize(1);

    TFT_eSPI().drawCentreString(item_name.c_str(), cntr_x , cntr_y + my_loc*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) + my_loc*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);

    if (selected_number == item_number){
        // Highlight frame if selected
        TFT_eSPI().drawRoundRect(rect_in+2, cntr_x-(rect_h/2)+2 + my_loc*space_y, rect_w - 4 , rect_h -4, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in+1, cntr_x-(rect_h/2)+1 + my_loc*space_y, rect_w - 2 , rect_h -2, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in-1, cntr_x-(rect_h/2)-1 + my_loc*space_y, rect_w + 2 , rect_h +2, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in-2, cntr_x-(rect_h/2)-2 + my_loc*space_y, rect_w + 4 , rect_h +4, 10, TFT_WHITE);
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
    menu_icon(i+1, level_val, item_vector[i]);  
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


void display_message(int displaytime_ms, char* line1, char* line2, char* line3){  // Function for displaying a message on screen for x milliseconds
   TFT_eSPI tft_display_message = TFT_eSPI(); // Need to instanciate library to be able to modify text colours etc

  
   tft_display_message.drawCentreString(line1, cntr_x , cntr_y -35, 4);
   tft_display_message.drawCentreString(line2, cntr_x , cntr_y - 0, 4);
   tft_display_message.drawCentreString(line3, cntr_x , cntr_y +35, 4);
   tft_display_message.drawRoundRect(rect_in, cntr_x - (180/2), rect_w, 180, 10, TFT_SKYBLUE);
   delay(displaytime_ms); // Show message for 0.8sec before turning screen black again
   tft_display_message.fillScreen(TFT_BLACK);}