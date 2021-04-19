#include "OLED.h"
#include "Controller_config.h"


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

void val_adjust_window(int variable, const char* item_name){           // Returns a new value for dynamic_variable
    
    TFT_eSPI().drawCentreString(item_name, cntr_x , cntr_y - 1*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) - 1*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);   

    char buffer[10];
    TFT_eSPI().setTextSize(5);
    TFT_eSPI().drawCentreString("-", cntr_x , cntr_y + 90, 4); 
    TFT_eSPI().drawCentreString(itoa(variable, buffer, 10), cntr_x - 5 , cntr_y+32, 6);
    TFT_eSPI().drawCentreString("+", cntr_x , cntr_y - 10, 4); 
    TFT_eSPI().drawCentreString("%", cntr_x + 50 , cntr_y +40, 4);
}

void toggle_popup(bool var_state){           // Returns a new value for dynamic_variable
  if (var_state){ 
    TFT_eSPI().drawCentreString("ON", cntr_x , cntr_y - 1*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) - 1*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);
    delay(800); // Show message for 0.8sec before turning screen black again
    TFT_eSPI().fillScreen(TFT_BLACK);
  }  

  else;{
    TFT_eSPI().drawCentreString("OFF", cntr_x , cntr_y - 1*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) - 1*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);
    delay(800); // Show message for 0.8sec before turning screen black again
    TFT_eSPI().fillScreen(TFT_BLACK);

  }
}

void display_menu(int level_val, std::vector<std::string> item_vector){   // Function for displaying a complete menu based on a vector containing cathegories
  
  int vector_len = item_vector.size();          // Find amount of elements in vector
  for (int i = 0; i < vector_len; i++){         // Draw to OLED one box with the respective menu item for each menu item in the vector
    menu_icon(i+1, level_val, item_vector[i]);  
  }
}

int menu_pos(bool pos_edge_up, bool pos_edge_dwn, int current_pos, std::vector<std::string>cathegory_vector){
  int vector_len = cathegory_vector.size();
  
  // Navigate downwards
  if (pos_edge_dwn){
    if (current_pos < vector_len){
      current_pos ++;
      TFT_eSPI().fillScreen(TFT_BLACK);
    }
  }
  // Navigate upwards
  if (pos_edge_up){
      if (current_pos > 1){
        current_pos --;
        TFT_eSPI().fillScreen(TFT_BLACK);
        }
      }
  return current_pos;
}

int mod_val(bool bottom_reached, bool percent_adj, bool pos_edge_up, bool pos_edge_dwn, int value){      // If menu navigation, val_adj = LOW, if value adjustment +/- 5%, val_adj = HIGH   
    
    // If the request concerns a percentage value
    if (percent_adj){
        if ((pos_edge_dwn) && (value < 0)) {
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