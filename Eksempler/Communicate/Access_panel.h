/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

// Globals
#include "Globals"

const uint8_t users = USERS;

// RFID
#include <SPI.h>
#include <MFRC522.h>

MFRC522 rfid(SS_PIN, RST_PIN);


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
const uint8_t door_check = DOOR_CHECK;


// OLED Display
#include "Oled_display.h"


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

bool   set_new_tolk_check = false;
int    user_id_at_tolk_check;
String set_new_tolk_type;


// Timers

uint32_t time_door_cloded;
int      door_close_delay = 5000; // 5 sek

uint32_t set_new_tolk;
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




///////////////// Start program ///////////////

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

    // Encryption
    Access_panel_sha256("");
    Access_panel_pull_tolk();
}



void Access_panel_loop() {

    char key = keypad.getKey();

    if (rfid.PICC_IsNewCardPresent()) {
        Access_panel_read_card();
    }

    if (set_new_tolk + set_new_tolk_delay < millis()) {
        set_new_tolk_check = false;

     // Reset keypad presses
        code_attempt = "";
        key_presses = 0;
    }

 // Lock door, but wait for it to close
    bool door_closed = digitalRead(door_check);

    if (!door_closed) {
        time_door_cloded = millis();
    }

    if (time_door_cloded + door_close_delay < millis()) {
        myservo.write(0);
    }
}



void Access_panel_keypad_event(char key_press) {
    
    if (set_new_tolk_check) {
        Access_panel_set_new_tolk(key_press);
    }
    else {
        switch (keypad.getState()) {
            case PRESSED:
                            Access_panel_keypad_loop(key_press);
                            break;

            case HOLD:
                            if (key_press == '0'){
                                set_new_tolk_check = true;
                                code_attempt = "";
                                key_presses = 0;
                            }
                            break;
        }
    }
}


enum Menu { Choose_type, Authenticate, Create_new_tolk, Send_tolk };
Menu menu = Choose_type;

void Access_panel_set_new_tolk (char key_press) {

    switch (menu) {
        case Choose_type:
                                if (key_press == '1') {
                                    set_new_tolk_type= "card";
                                    menu = Authenticate;
                                }
                                else if (key_press == '2') {
                                    set_new_tolk_type = "code";
                                    menu = Authenticate;
                                }
                                break;
      
        case Authenticate:
                                if (Access_panel_keypad_loop(key_press) || Access_panel_read_card()) {
                                    menu = Create_new_tolk;
                                    card_attempt = "";
                                    code_attempt = "";
                                    key_presses = 0;
                                }
                                break;
      
        case Create_new_tolk:
                                if (Access_panel_read_card() && set_new_tolk_type == "card") {
                                    menu = Send_tolk;
                                }
                                                                
                                if (Access_panel_keypad_loop(key_press) && set_new_tolk_type == "code") {
                                    menu = Send_tolk;
                                }
                                break;

        case Send_tolk:
                                if (set_new_tolk_type == "card") {
                                    Access_panel_push_tolk("user" + (String)user_id_at_tolk_check, set_new_tolk_type, card_attempt);
                                }
                                else if (set_new_tolk_type == "code") {
                                    Access_panel_push_tolk("user" + (String)user_id_at_tolk_check, set_new_tolk_type, code_attempt);
                                }
                                card_attempt = "";
                                code_attempt = "";
                                key_presses = 0;

                                menu = Choose_type;
                                break;
    }
}



bool Access_panel_keypad_loop(char key_press) {

    if (key_press == '#') {
        code_attempt = "";
        key_presses = 0;
    }
    else if (key_press == '*') {
        user_id_at_tolk_check = Access_panel_check_tolk();

        if (user_id_at_tolk_check != 0) {

            if (!set_new_tolk_check) {
                Access_panel_open_Door();

                Serial.println("LE GOW");

                code_attempt = "";
                key_presses = 0;
            }

            return 1;
        }
        else {
            Serial.println("Nope");
        }
        code_attempt = "";
        key_presses = 0;
    }
    else if (key_presses < 10) {
        Serial.println(key_press);
        code_attempt += key_press;
        key_presses++;
    }

    return 0;
}



bool Access_panel_read_card() {

    if (rfid.PICC_ReadCardSerial()) {
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

        for (int i = 0; i < 4; i++) {
            card_attempt += String(rfid.uid.uidByte[i], HEX);
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();

        user_id_at_tolk_check = Access_panel_check_tolk();

        if (user_id_at_tolk_check != 0) {

            if (!set_new_tolk_check) {
                Access_panel_open_Door();

                Serial.println("LE GOW");

                card_attempt = "";
            }

            return true;
        }
        else {
            Serial.println("Nope");
        }
        card_attempt = "";
    }

    return false;
}



int Access_panel_check_tolk() {

 // Convert card_attempt and code_attempt to sha256 hash
    card_attempt = Access_panel_sha256(card_attempt);
    code_attempt = Access_panel_sha256(code_attempt);

    for (int i = 0; i < users; i++) {
        if      (card_attempt == user_card[i])  return i + 1;
        else if (code_attempt == user_code[i])  return i + 1;
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

    access_panel_message.data_String[0] = { "request", "push" };
    access_panel_message.data_String[1] = { "user"   ,  user };
    access_panel_message.data_String[2] = { "type_tolk", "type_tolk" };
    access_panel_message.data_String[3] = { "tolk"   ,  tolk };

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