/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

#ifndef ACCESS_PANEL.H
#define ACCESS_PANEL.H

// Globals
#include "Globals.h"

const uint8_t users = USERS;

// RFID
#include <SPI.h>
#include <MFRC522.h>

MFRC522 rfid(MFRC522_SS, SPI_RESET);


// Keypad
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = { ROW1, ROW2, ROW3, ROW4 };
byte colPins[COLS] = { COLS1, COLS2, COLS3 };

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int      button_delay      = 30;   // 30ms delay
int      button_menu_delay = 2000; // 2sek delay


// Servo
#include <ESP32Servo.h>

Servo myservo;
int   servo_Pin = SERVO_PIN;


// Door
extern const uint8_t door_check; // Import door_check from Access_log.h
       const uint8_t door_exit = DOOR_EXIT;


// OLED Display
#include "Oled_display.h"
extern void Doorbell_show(); // Import Doorbell menu
extern uint32_t screen_on_time; // Import screen_on_time from Doorbell.h


// Encryption og MQTT
#include "MQTT_Class.h"
#include "mbedtls/md.h"; 

Mqtt_message access_panel_message;
MQTT* mqtt_access_panel;

String user_card[users];
String user_code[users];

String card_attempt;
String code_attempt;
int    key_presses;
int    max_code_len = 6;

bool   set_new_tolk_check = false;
bool   read_new_tolk      = false;
int    user_id_at_tolk_check;
String set_new_tolk_type;


enum Menu { Authenticate, Choose_type, Create_new_tolk, Send_tolk };
Menu menu = Authenticate;


// Timers

uint32_t time_door_cloded = 0;
int      door_close_delay = 10000; // 10 sek

uint32_t set_new_tolk       = 0;
int      set_new_tolk_delay = 30000;  // 30 sek



// Predeclare functiones
void   Access_panel_setup(MQTT& _mqtt);
void   Access_panel_loop();
void   Access_panel_keypad_event(char key_press);
void   Access_panel_set_new_tolk(char key_press);
bool   Access_panel_keypad_loop(char key_press);
bool   Access_panel_read_card();
int    Access_panel_check_tolk();
String Access_panel_sha256(String tolk);
void   Access_panel_open_door();
void   Access_panel_store_tolk(String cards[], String codes[]);
void   Access_panel_push_tolk(String user, String type_tolk, String tolk);
void   Access_panel_pull_tolk();






void Access_panel_setup(MQTT& _mqtt) {

    Serial.begin(115200);

 // Mqtt
    mqtt_access_panel = &_mqtt;

 // Oled display
    Oled_display_setup();

 // Keypad
    keypad.addEventListener(Access_panel_keypad_event);  // Add an event listener.
    keypad.setHoldTime(button_menu_delay);
    keypad.setDebounceTime(button_delay);

 // NFC scanner
    SPI.begin();
    rfid.PCD_Init();

 // Servo
    myservo.setPeriodHertz(50);
    myservo.attach(servo_Pin, 500, 2400);
    myservo.write(0);

 // Door
    pinMode(door_exit, INPUT);

 // Encryption
    Access_panel_sha256("");
    Access_panel_pull_tolk();
}



void Access_panel_loop() {

 // Read key pressed
    keypad.getKey();

 // Check card is valid if pressent
    if (rfid.PICC_IsNewCardPresent()) {
        if (!set_new_tolk_check) {
            Access_panel_read_card();
        }
        else{
            Access_panel_set_new_tolk('r');
        }
    }

 // Exit set_new_tolk_menu after no action for set time
    if (set_new_tolk + set_new_tolk_delay < millis() && set_new_tolk_check && set_new_tolk_check) {
        set_new_tolk_check = false;
        read_new_tolk      = false;
        
     // Reset keypad presses
        card_attempt = "";
        code_attempt = "";
        key_presses = 0;

     // Reset set_new_tolk_meue
        menu = Authenticate;

     // Go back to display doorbell
        Doorbell_show();
    }

 // Lock door, but wait for it to close
    if (!digitalRead(door_check)) {
        time_door_cloded = millis();
    }

    if (time_door_cloded + door_close_delay < millis()) {
        myservo.write(0);
    }
    
 // Open door with button
    if (digitalRead(door_exit)) {
        Access_panel_open_door();
        time_door_cloded = millis();
    }

 // Send new tolk 
    if (menu == Send_tolk)
        Access_panel_set_new_tolk('r');
}



void Access_panel_keypad_event(char key_press) {
    
    if (set_new_tolk_check && keypad.getState() == PRESSED) {
        Access_panel_set_new_tolk(key_press);

    }
    else {
        switch (keypad.getState()) {
            case PRESSED:
                            Access_panel_keypad_loop(key_press);
                            break;

            case HOLD:
                            if (key_press == '0'){
                                display.ssd1306_command(SSD1306_DISPLAYON);
                                screen_on_time = millis();

                                Oled_display_text_clear("Spesial", 25, 15, 2);
                                Oled_display_text("menu", 40, 33, 2);
                                display.display();

                                delay(1500);

                                Oled_display_text_clear("Enter code", 5, 5, 2);
                                Oled_display_text("or", 54, 23, 2);
                                Oled_display_text("use card", 17, 43, 2);
                                display.display();

                                set_new_tolk       = millis();
                                set_new_tolk_check = true;

                                card_attempt = "";
                                code_attempt = "";
                                key_presses  = 0;
                            }
                            break;
        }
    }
}



void Access_panel_set_new_tolk (char key_press) {
 // Keep disply alive if there is action
    screen_on_time = millis();

 // New tolk menu
    switch (menu) {
        case Authenticate:
                                if (Access_panel_keypad_loop(key_press) || Access_panel_read_card()) {
                                    Oled_display_text_clear("Options:", 3, 3, 2);
                                    Oled_display_text("New card:1", 6, 23, 2);
                                    Oled_display_text("New code:2", 6, 43, 2);
                                    display.display();

                                    read_new_tolk = true;

                                    menu = Choose_type;
                                }
                                if (isdigit(key_press)) break;

                                card_attempt = "";
                                code_attempt = "";
                                key_presses = 0;
                                break;

        case Choose_type:
                                if (key_press == '1') {
                                    Oled_display_text_clear("New card", 15, 10, 2);
                                    Oled_display_text("is chosen", 10, 35, 2);
                                    display.display();

                                    delay(1500);

                                    Oled_display_text_clear("Scan card", 10, 24, 2);
                                    display.display();

                                    card_attempt = "";
                                    code_attempt = "";
                                    key_presses = 0;

                                    set_new_tolk_type = "card";
                                    menu = Create_new_tolk;
                                }

                                else if (key_press == '2') {
                                    Oled_display_text_clear("New code", 15, 10, 2);
                                    Oled_display_text("is chosen", 10, 35, 2);
                                    display.display();

                                    delay(1500);

                                    Oled_display_text_clear("Enter code", 5, 10, 2);
                                    Oled_display_line(15, 50);
                                    display.display();

                                    card_attempt = "";
                                    code_attempt = "";
                                    key_presses = 0;

                                    set_new_tolk_type = "code";
                                    menu = Create_new_tolk;
                                }
                                break;
      
        case Create_new_tolk:
                                if (Access_panel_read_card() && set_new_tolk_type == "card") {
                                    menu = Send_tolk;
                                }
                                
                                if (set_new_tolk_type == "code") {
                                    Access_panel_keypad_loop(key_press);
                                    Serial.print("key_presses: ");
                                    Serial.println(key_presses);

                                    if (key_press == '*' && key_presses >= 4) {
                                            menu = Send_tolk;
                                    }
                                    else if (key_press == '*') {
                                     // Print "Too short" when new code is less than 4
                                        Oled_display_text_clear("Too short", 15, 10, 2);
                                        display.display();

                                        delay(1500);

                                        Oled_display_text_clear("Enter code", 5, 10, 2);
                                        Oled_display_line(15, 50);

                                        code_attempt = "";
                                        key_presses = 0;
                                    }

                                 // Draw code with asterisks
                                    else if (key_presses == 0) {
                                        Oled_display_text_clear("Enter code", 5, 10, 2);
                                        Oled_display_line(15 + 14 * key_presses, 50);
                                    }
                                    else if (key_presses < max_code_len) {
                                        Oled_display_text("*", 17 + 14 * (key_presses - 1), 40);
                                        Oled_display_line(15 + 14 * key_presses, 50);
                                    }
                                    else {
                                        Oled_display_text("*", 17 + 14 * (key_presses - 1), 40);
                                    }
                                    display.display();
                                }
                                break;

        case Send_tolk:
                                if (set_new_tolk_type == "card") {
                                    Oled_display_text_clear("Card", 40, 15, 2);
                                    Oled_display_text("stored", 30, 35, 2);
                                    display.display();

                                    delay(1500);

                                    user_card[user_id_at_tolk_check - 1] = card_attempt;

                                    Access_panel_push_tolk("user" + (String)user_id_at_tolk_check, set_new_tolk_type, card_attempt);
                                }
                                else if (set_new_tolk_type == "code") {
                                    Oled_display_text_clear("Code", 40, 15, 2);
                                    Oled_display_text("stored", 30, 35, 2);
                                    display.display();

                                    delay(1500);

                                    user_code[user_id_at_tolk_check - 1] = code_attempt;

                                    Access_panel_push_tolk("user" + (String)user_id_at_tolk_check, set_new_tolk_type, code_attempt);
                                }

                                card_attempt = "";
                                code_attempt = "";
                                key_presses = 0;

                                set_new_tolk_check = false;
                                read_new_tolk      = false;

                                menu = Authenticate;

                                Doorbell_show();
                                break;
    }
}



bool Access_panel_keypad_loop(char key_press) {

 // Exstend time in set_new_tolk_menu if button is pressed
    if (set_new_tolk_check) {
        set_new_tolk = millis();
    }

 // Reset code attempt
    if (key_press == '#') {
        code_attempt = "";
        key_presses = 0;
    }

 // Check code attempt
    else if (key_press == '*') {

        if (!set_new_tolk_check) {
            user_id_at_tolk_check = Access_panel_check_tolk();

            if (user_id_at_tolk_check != 0) {
                Access_panel_open_door();

                #ifdef DEBUG
                    Serial.println("LE GOW");
                #endif

                code_attempt = "";
                key_presses = 0;

                return true;
            }
            else {
                #ifdef DEBUG
                    Serial.println("Nope");
                #endif

                code_attempt = "";
                key_presses = 0;
            }
        }
        else {
            if (read_new_tolk) {
                Access_panel_check_tolk();
                return true;
            }
            else {
                user_id_at_tolk_check = Access_panel_check_tolk();
                if (user_id_at_tolk_check != 0) return true;
            }
        }
    }

 // Add key pressed to code attempt
    else if (key_presses < max_code_len) {
        #ifdef DEBUG
            Serial.println(key_press);
        #endif
        code_attempt += key_press;
        key_presses++;
    }

    return false;
}



bool Access_panel_read_card() {

    if (rfid.PICC_ReadCardSerial()) {
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

        for (int i = 0; i < 4; i++) {
            card_attempt += String(rfid.uid.uidByte[i], HEX);
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();

        if (!set_new_tolk_check) {
            user_id_at_tolk_check = Access_panel_check_tolk();

            if (user_id_at_tolk_check != 0) {

                Access_panel_open_door();
                #ifdef DEBUG
                    Serial.println("LE GOW");
                #endif

               card_attempt = "";
                    
                return true;
            }
            else {
                #ifdef DEBUG
                    Serial.println("Nope");
                #endif

                card_attempt = "";
            }
        }
        else {
            if (read_new_tolk) {
                Access_panel_check_tolk();
                return true;
            }
            else {
                user_id_at_tolk_check = Access_panel_check_tolk();
                if (user_id_at_tolk_check != 0) return true;
            }
        }
    }
    return false;
}



int Access_panel_check_tolk() {

 // Convert card_attempt and code_attempt to sha256 hash
    card_attempt = Access_panel_sha256(card_attempt);
    code_attempt = Access_panel_sha256(code_attempt);

    for (int i = 0; i < users; i++) {
        if      (card_attempt == user_card[i])  return (i + 1);
        else if (code_attempt == user_code[i])  return (i + 1);
        else if (           i == users - 1   )  return 0;
    }
}



String Access_panel_sha256(String tolk) {

    char* key = "SD3wGw&teRy^!GYiY24Q2eqZ";
    char* payload = const_cast<char*>(tolk.c_str());
    byte hmacResult[32];

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    const size_t payloadLength = strlen(payload);
    const size_t keyLength = strlen(key);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char*)key, keyLength);
    mbedtls_md_hmac_update(&ctx, (const unsigned char*)payload, payloadLength);
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);

    String sha256;

    for (int i = 0; i < sizeof(hmacResult); i++) {
        char str[3];

        sprintf(str, "%02x", (int)hmacResult[i]);
        sha256 += str;
    }

    return sha256;
}



void Access_panel_open_door() {
    myservo.write(180);
    time_door_cloded = millis();
 // add buzzer
}



void Access_panel_store_tolk(String cards[], String codes[]) {
    if (cards[0] != "") {
        for (int i = 0; i < users; i++) user_card[i] = cards[i];
    }
    if (codes[0] != "") {
        for (int i = 0; i < users; i++) user_code[i] = codes[i];
    }
}



void Access_panel_push_tolk(String user, String type_tolk, String tolk) {

    access_panel_message.resiver = "Hub";
    access_panel_message.room    = Entry;
    access_panel_message.header  = Access_controll;

    access_panel_message.data_String[0] = { "request"   , "push" };
    access_panel_message.data_String[1] = { "user"      ,  user };
    access_panel_message.data_String[2] = { "type_tolk" ,  type_tolk };
    access_panel_message.data_String[3] = { "tolk"      ,  tolk };

    mqtt_access_panel->pub(access_panel_message);
}



void Access_panel_pull_tolk() {
    access_panel_message.resiver = "Hub";
    access_panel_message.room    = Entry;
    access_panel_message.header  = Access_controll;

    access_panel_message.data_String[0] = { "request"  , "pull" };
    access_panel_message.data_String[1] = { "type_tolk", "card and code" };

    mqtt_access_panel->pub(access_panel_message);
}

#endif