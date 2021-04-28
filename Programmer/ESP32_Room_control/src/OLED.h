//#ifndef OLED.h
//#define OLED.h

#include <Controller_config.h>

#define TFT_GREY 0x5AEB

#define cntr_x 120      // Screen pixle centre x
#define cntr_y 110      // Screen pixle centre y
#define space_y 70      // Space between menu icons
#define rect_w 200      // Menu rectangle width
#define rect_h 50       // Menu rectangle height
#define rect_in 20      // Indentation for menu rectangle

void menu_icon(int, int, std::string);                      // Function drawing a single menu icon. Must be called for all "buttons" in a menu
void val_adjust_window(int, const char*);                   // Function drawing a window dedicated to adjusting values (typically altering a percentage value)
void temp_adjust_window(int , const char*);
void display_menu(int, std::vector<std::string>);           // Displays one complete menu page
int  mod_val(bool, bool, bool, bool, int);                  // If menu navigation, val_adj = LOW, if value adjustment +/- 5%, val_adj = HIGH   
int  mod_temp(bool, bool, int);                             // Function for modifying a temperature value
void toggle_popup(bool, const char*);
void display_message(int, char*, char*, char*);             // Function for displaying a message on screen for x milliseconds
void display_setup_screen();                                // Function for displaying setup screen while setting up controller
void display_reconnecting_screen();                         // Function for displaying WiFi reconnection screen
void display_setup_messages(const char*, const char*, const char*);  // Function for displaying setup screen with custom text
void display_screensaver(int, const char*);                 // Function for displaying screensaver
//#endif