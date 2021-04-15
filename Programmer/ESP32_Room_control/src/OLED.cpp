#include "OLED.h"
#include "Controller_config.h"


void menu_icon(int item_number, int selected_number, const char* item_name){
    
    int my_loc = (item_number - selected_number);  // Location of current item
    TFT_eSPI().setTextSize(1);

    TFT_eSPI().drawCentreString(item_name, cntr_x , cntr_y + my_loc*space_y, 4);
    TFT_eSPI().drawRoundRect(rect_in, cntr_x - (rect_h/2) + my_loc*space_y, rect_w, rect_h, 10, TFT_SKYBLUE);

    if (selected_number == item_number){
        // Highlight frame if selected
        TFT_eSPI().drawRoundRect(rect_in+2, cntr_x-(rect_h/2)+2 + my_loc*space_y, rect_w - 4 , rect_h -4, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in+1, cntr_x-(rect_h/2)+1 + my_loc*space_y, rect_w - 2 , rect_h -2, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in-1, cntr_x-(rect_h/2)-1 + my_loc*space_y, rect_w + 2 , rect_h +2, 10, TFT_WHITE);
        TFT_eSPI().drawRoundRect(rect_in-2, cntr_x-(rect_h/2)-2 + my_loc*space_y, rect_w + 4 , rect_h +4, 10, TFT_WHITE);
    }
}

void val_adjust_window(int variable){           // Returns a new value for dynamic_variable
    
    char buffer[10];
    TFT_eSPI().setTextSize(5);
    TFT_eSPI().drawCentreString("-", cntr_x , cntr_y + 90, 4); 
    TFT_eSPI().drawCentreString(itoa(variable, buffer, 10), cntr_x , cntr_y - 30, 8);
    TFT_eSPI().drawCentreString("+", cntr_x , cntr_y - 90, 4); 
    TFT_eSPI().drawCentreString("%", cntr_x + 90 , cntr_y, 4);
}