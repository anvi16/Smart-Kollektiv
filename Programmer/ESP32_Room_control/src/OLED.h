#ifndef OLED.H
#define OLED.H

#include <Controller_config.h>

#define TFT_GREY 0x5AEB

#define cntr_x 120      // Screen pixle centre x
#define cntr_y 110      // Screen pixle centre y
#define space_y 70      // Space between menu icons
#define rect_w 200      // Menu rectangle width
#define rect_h 50       // Menu rectangle height
#define rect_in 20      // Indentation for menu rectangle

void OLED_setup();
void OLED_scroll();
void box_selected(bool);
void menuframe(int, bool, const char*);

#endif