/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

#ifndef ACCESS_PANEL_EDIT.H
#define ACCESS_PANEL_EDIT.H

#define DEBUG  // <-- Define for debug 

#include <Keypad.h>
#include "mbedtls/md.h";


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

String user_code[users] = { "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a" };


String code;
int key_nr;


String Akses_panel_sha256() {
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


bool Akses_panel_check_password() {
    code = Akses_panel_sha256();    // Convert code to sha256 hash
    for (int i = 0; i < users; i++) {
        if (code == user_code[i]) return 1;
        else if (i == users - 1)  return 0;
    }
}


void Akses_panel_keypad_event(char key_press) {
    switch (keypad.getState()) {
    case PRESSED:
        if (key_press == '#') {
            code = "\0";
            key_nr = 0;
#ifdef DEBUG
            Serial.println("Code cleared");
#endif
        }
        else if (key_press == '*') {
#ifdef DEBUG
            Serial.print("Code check: ");
#endif

            if (Akses_panel_check_password()) {
                Serial.println("LE GOW");
            }
            else {
                Serial.println("Nope");
            }
            code = "\0";
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


void Akses_panel_setup() {
    Serial.begin(115200);
    keypad.addEventListener(Akses_panel_keypad_event);       // Add an event listener.
    keypad.setHoldTime(button_menu_delay);
    keypad.setDebounceTime(button_delay);
    Akses_panel_sha256();
}

void Akses_panel_loop() {
    char key = keypad.getKey();
}

#endif