#include "OLED.h"
#include "Controller_config.h"


void y_centre(int box_number){
    Serial.println("");
}

void OLED_menu(int itemnumber, vector<const char*> titles) {
    // Itemnumber says which item should be displayed,
    // titles is an array of char arrays that shows the titled to be displayed on each box in menu

    int number_of_icons = titles.size();

    Serial.println(number_of_icons);

}

vector <const char *> menu_items{};

void box_selected() {
    TFT_eSPI().drawRoundRect(rect_in+1, (cntr_x-(rect_h/2))+1, rect_w-2, rect_h-2, 10, TFT_SKYBLUE);
    TFT_eSPI().drawRoundRect(rect_in-1, (cntr_x-(rect_h/2))-1, rect_w+2, rect_h+2, 10, TFT_SKYBLUE);
}
