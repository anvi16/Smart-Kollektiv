/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

// Kort leser
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 21
#define RST_PIN 22
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
byte rowPins[ROWS] = { 32, 12, 9, 27 };
byte colPins[COLS] = { 26, 25, 33 };
int      button_delay = 30;   // 30ms delay
int      button_menu_delay = 2000; // 2sek delay


String new_card;
String code;
int key_nr;

bool spesial_menu_check = false;
int new_user_id;
String new_user_item;


Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
#define users 6


// Servo
#include <ESP32Servo.h>
Servo myservo;
int servoPin = 13;
int servo_door_opne_delay = 5000;
long unsigned int now_time;



//Kryptering og MQTT
#include "mbedtls/md.h"; // 
String user_card[users];
String user_code[users] = { "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a",
                            "3e8a85792028bdeeb97650328ac3394a32eab166740c08808f603f0541517e2a"
                          };

String Akses_panel_sha256(String tolk) {
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
int Akses_panel_check_password() {
  new_card = Akses_panel_sha256(new_card);
  code = Akses_panel_sha256(code);          // Convert code to sha256 hash
  for (int i = 0; i < users; i++) {
    if      (new_card == user_card[i]) return i;
    else if (code  == user_code[i]) return i;
    else if (i == users - 1)  return 0;
  }
}





void setup()
{
  // Keypad
  Serial.begin(115200);
  keypad.addEventListener(Akses_panel_keypad_event);       // Add an event listener.
  keypad.setHoldTime(button_menu_delay);
  keypad.setDebounceTime(button_delay);
  // NFC scanner
  SPI.begin();
  rfid.PCD_Init();

  // Servo
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
}


void loop()
{





  char key = keypad.getKey();
  if (rfid.PICC_IsNewCardPresent())
  {
    read_card();
  }
}







void read_card()
{
  if (rfid.PICC_ReadCardSerial()) {
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    for (int i = 0; i < 4; i++) {
      new_card += String(rfid.uid.uidByte[i], HEX);

    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}



// Tall koden blir gennerert her.
void Akses_panel_keypad_event(char key_press) {
  if (spesial_menu_check == true)
  {
    Spesial_Menu(key_press);
  }
  else {
    switch (keypad.getState()) {
      case PRESSED:
        make_a_code(key_press);
        break;

      case HOLD:
        if (key_press == '0')
        {
          spesial_menu_check = true;
          code = "";
        }
        break;
    }
  }
}
void Open_Door()
{
  now_time = millis();
  myservo.write(180);
  if ((millis() - now_time) >= servo_door_opne_delay /* && magnet_on_door_is_closed */ )
  {
    myservo.write(0);
  }
}


void Spesial_Menu (char key_press)
{
  enum Menu {Item, Authenticate, New_code_card, Send_tolk};
  Menu menu = Item;

  switch (menu)
  {
    case Item:
      if (key_press == '1')
      {
        new_user_item= "card";
        menu = Authenticate;
      }
      else if (key_press == '2')
      {
        new_user_item = "code";
        menu = Authenticate;
      }
      break;
      
    case Authenticate:
      if (make_a_code(key_press))
      {
        menu = New_code_card;
        code = "";
        key_nr = 0;
      }
      break;
      
    case New_code_card:
      if (make_a_code(key_press))
      {
        menu = Send_tolk;
      }
        
      break;
    case Send_tolk:
      if (new_user_item == "card"){
    // Send tolk Access_panel_push_tolk(new_user_id, new_user_item, new_card);
      }
      else if (new_user_item == "code"){
    // Send tolk Access_panel_push_tolk(new_user_id, new_user_item, "", code);
      }
      break;
  }
}

bool make_a_code(char key_press)
{
  if (key_press == '#') {
    code = "";
    key_nr = 0;
  }
  else if (key_press == '*') {
    new_user_id = Akses_panel_check_password();
    if (new_user_id != 0) {
      if (!spesial_menu_check)
      {
        Open_Door();
        Serial.println("LE GOW");
        code = "";
        key_nr = 0;
      }
      return true;
    }
    else
    {
      Serial.println("Nope");
    }
    code = "";
    key_nr = 0;
  }
  else if (key_nr < 10) {
    code += key_press;
    key_nr++;

  }
  return false;
}
