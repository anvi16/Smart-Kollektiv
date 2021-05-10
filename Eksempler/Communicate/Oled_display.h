/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description:
        OLED display
*************************************************/

#ifndef OLED_DISPLAY.H
#define OLED_DISPLAY.H


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// I2C
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SPI 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, SPI_RESET, OLED_CS, OLED_SPICLOCK);
// (uint8_t w, uint8_t h, SPIClass *spi, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, uint32_t bitrate = 8000000UL);

bool oled_setup = false;


bool Oled_display_check_setup() {
    return oled_setup;
}


void Oled_display_setup() {
 // Only setup onec
    if (!oled_setup) {
     // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
#ifdef DEBUG
            Serial.println(F("SSD1306 allocation failed"));
#endif
            for (;;); // Don't proceed, loop forever
        }

     // Set setup check to true
        oled_setup = true;

     // Show initial display buffer contents on the screen --
       // the library initializes this with an Adafruit splash screen.
        display.display();
      //delay(2000); // Pause for 2 seconds
    }
}


void Oled_display_text_clear(String text, int x = 0, int y = 0, uint8_t size = 1, uint8_t color = SSD1306_WHITE) {
    display.clearDisplay();
    display.setTextSize(size);
    display.setTextColor(color);
    display.setCursor(x, y);
    display.println(text);
}


void Oled_display_text(String text, int x = 0, int y = 0, uint8_t size = 1, uint8_t color = SSD1306_WHITE) {
    display.setTextSize(size);
    display.setTextColor(color);
    display.setCursor(x, y);
    display.println(text);
}


void Oled_display_dot_clear(int x = 0, int y = 0, uint8_t color = SSD1306_WHITE) {
    display.clearDisplay();
    display.drawPixel(x, y, color);
}


void Oled_display_dot(int x = 0, int y = 0, uint8_t color = SSD1306_WHITE) {
    display.drawPixel(x, y, color);
}


void Oled_display_line_clear(int x = 0, int y = 0, int length = 10, int rot = 0, uint8_t color = SSD1306_WHITE) {
    display.clearDisplay();
    display.setCursor(x, y);
    for (int i = 0; i < length; i++) {
        if (rot) display.drawPixel(x, y + i, color); // Vertical
        else display.drawPixel(x + i, y, color);    // Horizontal
    }
    display.display();
}


void Oled_display_line(int x = 0, int y = 0, int length = 10, int rot = 0, uint8_t color = SSD1306_WHITE) {
    display.setCursor(x, y);
    for (int i = 0; i < length; i++) {
        if (rot) display.drawPixel(x, y + i, color); // Vertical
        else display.drawPixel(x + i, y, color);    // Horizontal
    }
    display.display();
}



#endif