// #ifndef OLED.H
// #define OLED.H

#include <Controller_config.h>

#define TFT_GREY 0x5AEB

#define cntr_x 120      // Screen pixle centre x
#define cntr_y 110      // Screen pixle centre y
#define space_y 70      // Space between menu icons
#define rect_w 200      // Menu rectangle width
#define rect_h 50       // Menu rectangle height
#define rect_in 20      // Indentation for menu rectangle

void menu_icon(int, int, std::string);                      // Displays single menu icon. Must be called for all buttons
void val_adjust_window(int, const char*);                   // Displays the vale adjustment window
void display_menu(int, std::vector<std::string>);           // Displays one complete menu page
int  menu_pos(bool, bool, int, std::vector<std::string>);   // Reads pos edge up and down and the current menu item list and current position and returns current pos
int  mod_val(bool, bool, bool, bool, int);                        // If menu navigation, val_adj = LOW, if value adjustment +/- 5%, val_adj = HIGH   
void toggle_popup(bool);

// #endif