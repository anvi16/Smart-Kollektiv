/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

//#define DEBUG  // <-- Define for debug 

#include<CircusESP32Lib.h>

char ssid[]        = "Telenor6619dyp";
char password[]    = "josehome08";
char server[]      = "www.circusofthings.com";
char token[]       = "LED";
char User1_Key_1[] = "6442";
char User1_Key_2[] = "30385";
char User1_Key_3[] = "8691";
char User1_Key_4[] = "21957";

CircusESP32Lib circusESP32(server,ssid,password);

    
int button_pin0 = 2;
int button_pin1 = 3;
int button_pin2 = 4;
int button_pin3 = 5;
int button_pin4 = 6;
int button_pin5 = 7;
int button_pin6 = 8;
int button_pin7 = 9;
int button_pin8 = 10;
int button_pin9 = 11;

int button_pinEnter = 12;
int button_pinRemove = 13;

int      button_delay      = 30;   // 30ms delay
int      button_menu_delay = 2000; // 2sek delay
uint32_t button_press_last = 0;
int      last_pressed;
bool     last_read_press;


int user1_val_1, user1_val_2, user1_val_3, user1_val_4;

int key_nr;
int code[4];
int access_code1[4] = { 1, 3, 3, 4 };
int access_code2[4] = { 1, 2, 3, 4 };
int access_code3[4] = { 1, 0, 0, 0 };
int access_code4[4] = { 4, 3, 2, 1 };
int access_code5[4] = { 1, 0, 0, 0 };
int access_code6[4] = { 1, 0, 0, 0 };

//int access_code1[4] = {user1_val_1, user1_val_2, user1_val_3, user1_val_4} ;


void setup() {
    Serial.begin(9600);
    pinMode(button_pin0, INPUT_PULLUP);
    pinMode(button_pin1, INPUT_PULLUP);
    pinMode(button_pin2, INPUT_PULLUP);
    pinMode(button_pin3, INPUT_PULLUP);
    pinMode(button_pin4, INPUT_PULLUP);
    pinMode(button_pin5, INPUT_PULLUP);
    pinMode(button_pin6, INPUT_PULLUP);
    pinMode(button_pin7, INPUT_PULLUP);
    pinMode(button_pin8, INPUT_PULLUP);
    pinMode(button_pin9, INPUT_PULLUP);
    pinMode(button_pinEnter,  INPUT_PULLUP);
    pinMode(button_pinRemove, INPUT_PULLUP);

    circusESP32.begin();
}

void loop() {
    user1_val_1 = circusESP32.read(User1_Key_1, token);
    user1_val_2 = circusESP32.read(User1_Key_2, token);
    user1_val_3 = circusESP32.read(User1_Key_3, token);
    user1_val_4 = circusESP32.read(User1_Key_4, token);

    bool b0 = !digitalRead(button_pin0);
    bool b1 = !digitalRead(button_pin1);
    bool b2 = !digitalRead(button_pin2);
    bool b3 = !digitalRead(button_pin3);
    bool b4 = !digitalRead(button_pin4);
    bool b5 = !digitalRead(button_pin5);
    bool b6 = !digitalRead(button_pin6);
    bool b7 = !digitalRead(button_pin7);
    bool b8 = !digitalRead(button_pin8);
    bool b9 = !digitalRead(button_pin9);

    bool bE = !digitalRead(button_pinEnter);
    bool bC = digitalRead(button_pinRemove);


    bool Code_box[] = { b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, bE, bC };

    for (int i = 0; i < 12; i++) {
        if (Code_box[i] && !last_read_press && button_press_last + button_delay < millis()) {  // Check's for debounse and if button is not pressed long
            button_press_last = millis();
            last_pressed = i;
            last_read_press = true;
            if (i < 10 && key_nr < 4) { // Check's if butten is 0-9 and pressed four times
                #ifdef DEBUG
                    Serial.println(i);
                #endif
                code[key_nr] = i;
                key_nr++;
                break;
            }
            else if (bC) {
                code[0] = 0;
                code[1] = 0;
                code[2] = 0;
                code[3] = 0;
                key_nr = 0;
                #ifdef DEBUG
                    Serial.println("Clear");
                #endif
            }
            else if (bE) {
                bool accept_the_code = false;
                if      (Enter(access_code1)) accept_the_code = true;
                else if (Enter(access_code2)) accept_the_code = true;
                else if (Enter(access_code3)) accept_the_code = true;
                else if (Enter(access_code4)) accept_the_code = true;
                else if (Enter(access_code5)) accept_the_code = true;
                else if (Enter(access_code6)) accept_the_code = true;

                if (accept_the_code) {
                    Serial.println("LE GOW");
                }
                else {
                    Serial.println("Nope");
                }
                key_nr = 0;
            }
        }
        else {
            if (last_pressed == 0 && button_press_last + button_menu_delay < millis()) {
                button_press_last = millis();
                // Reset all
                code[0] = 0;
                code[1] = 0;
                code[2] = 0;
                code[3] = 0;
                key_nr = 0;

                // Go to spesial menu //
                Serial.println("Spesial menu");
                ////////////////////////
            }
            else if (!Code_box[last_pressed]) {     // Check if button pressed is released
                 last_read_press = false;
            }
        }
    }
}


bool Enter(int user_code[]) {
    int check = 0;
    for (int i = 0; i < 4; i++) {
        if (code[i] == user_code[i]) {
            check++;
            if (check == 4) {
                code[0] = 0;
                code[1] = 0;
                code[2] = 0;
                code[3] = 0;
                return 1;
            }
        }
        else {
            return 0;
        }
    }
}

/*
Hei, har godt over koden og lagt til en debounser (for å fjerne delay()'en) og en sjekk om knappene er holdt inne, 
måtte flytte litt på deler av koden for å få det til. 
Har ikke endret koden din noe mer en og fjrnet noen få overflødige variabler og fasongen (API'en gjør mye endringer selv), 
håper du ser at koden er den samme. Du må gjerne ender tilbake det du skulle føle er nødvndig (utsene messig/ eventuelt annet)

Lagt klart til å kunne lagre nye koder og kort med adgang til en spesial meny.

Det må gjerne legges til litt komentarer på koden, underveis og ikke nøl med å prøve og ta kontakt på discord
om du lurer på noe, eller lurer på hvordan det kan være lurt å legge inn oppdateringen av nye koder/kort mot rasperry pi.
(Kan anbefale og ikke gå gjennom CoT)

*/