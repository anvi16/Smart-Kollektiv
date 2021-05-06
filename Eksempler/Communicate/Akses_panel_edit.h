/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

#ifndef ACCESS_PANEL_EDIT.H
#define ACCESS_PANEL_EDIT.H

#define DEBUG  // <-- Define for debug 

#include "MQTT_Class.h"
#include <Keypad.h>
#include "mbedtls/md.h";
#include "Oled_display.h"

Mqtt_message access_panel_message;
MQTT* mqtt_access_panel;


const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = { 13, 12, 14, 27 };
byte colPins[COLS] = { 26, 25, 33 };

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define users 6

int      button_delay = 30;   // 30ms delay
int      button_menu_delay = 2000; // 2sek delay

String user_card[users];
String user_code[users]; 

String code;
int key_nr;

bool wait;


String Access_panel_sha256(String code) {
    char* key = "SD3wGw&teRy^!GYiY24Q2eqZ";
    char* payload = const_cast<char*>(code.c_str());
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
#ifdef DEBUG
    Serial.print("Hash: ");
#endif

    String sha256;
    for (int i = 0; i < sizeof(hmacResult); i++) {
        char str[3];

        sprintf(str, "%02x", (int)hmacResult[i]);
#ifdef DEBUG
        Serial.print(str);
#endif
        sha256 += str;
    }
#ifdef DEBUG
    Serial.println();
#endif
    return sha256;
}


bool Access_panel_check_password() {
    code = Access_panel_sha256(code);    // Convert code to sha256 hash
    for (int i = 0; i < users; i++) {
        if (code == user_code[i]) return 1;
        else if (i == users - 1)  return 0;
    }
}


void Access_panel_push_tolk(String user, String item, String card = "", String code = "") {
    access_panel_message.resiver        = "Hub";
    access_panel_message.room           = Entry;
    access_panel_message.header         = Access_controll;
    access_panel_message.data_String[0] = { "request", "push" };
    access_panel_message.data_String[0] = { "user"   ,  user  };
    access_panel_message.data_String[1] = { "card"   ,  card  };
    access_panel_message.data_String[2] = { "code"   ,  code  };

    mqtt_access_panel->pub(access_panel_message);
}


void Access_panel_pull_tolk() {
    access_panel_message.resiver        = "Hub";
    access_panel_message.room           = Entry;
    access_panel_message.header         = Access_controll;
    access_panel_message.data_String[0] = { "request"  , "pull" };
    access_panel_message.data_String[1] = { "type_tolk", "card and code" };

    mqtt_access_panel->pub(access_panel_message);
    wait = true;
}


void Access_panel_keypad_event(char key_press) {
    switch (keypad.getState()) {
    case PRESSED:
        if (key_press == '#') {
            code = "";
            key_nr = 0;
#ifdef DEBUG
            Serial.println("Code cleared");
#endif
        }
        else if (key_press == '*') {
#ifdef DEBUG
            Serial.print("Code check: ");
#endif

            if (Access_panel_check_password()) {
                Serial.println("LE GOW");
            }
            else {
                Serial.println("Nope");
            }
            code = "";
            key_nr = 0;
        }
        else if (key_nr < 10) {
            code += key_press;
            key_nr++;
#ifdef DEBUG
            Serial.println(key_press);
#endif
        }
        else {
#ifdef DEBUG
            Serial.println("Too many numbers");
#endif
        }
        break;

    case HOLD:
        if (key_press == '0') {

            // Go to spesial menu //
            Serial.println("Spesial menu");
            ////////////////////////
        }
        break;
    }
}


void Access_panel_store_tolk(String cards[], String codes[] ) {
    if (cards[0] != "") {
        for (int i = 0; i < users; i++) user_card[i] = cards[i];
    }
    if (codes[0] != "") {
        for (int i = 0; i < users; i++) user_code[i] = codes[i];
    }
}


void Access_panel_setup(MQTT& _mqtt) {
    mqtt_access_panel = &_mqtt;

    Serial.begin(115200);
    Oled_display_setup();

    keypad.addEventListener(Access_panel_keypad_event);       // Add an event listener.
    keypad.setHoldTime(button_menu_delay);
    keypad.setDebounceTime(button_delay);
    Access_panel_sha256("");
    Access_panel_pull_tolk();

    //while(wait){}
    Serial.println("Codes pulled");
}

void Access_panel_loop() {
    char key = keypad.getKey();
}

#endif