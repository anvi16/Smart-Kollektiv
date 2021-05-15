#include "OLED.h"
#include "MQTT_Class.h"
#include "Entry.h"
#include <stdlib.h>

// MENU
// Main menu                  (adr 1)
std::vector<std::string> main_cathegories{"Kitchen", "Bathroom", "Livingroom"};

// Kitchen menu               (adr 2.1)
std::vector<std::string> kitchen_cathegories{"Book", "Cancel booking"};

//Cancel kit menu            (adr 3.1.2)
std::vector<std::string> cancel_kit_menu{"Slot 1", "Slot 2"};



// Bathroom menu         (adr 2.2)
std::vector<std::string> bathroom_cathegories{"Book", "Cancel bathrrom"};

//Book bath menu    (adr 3.2.1)
std::vector<std::string> book_bath_menu{"Toilet", "Shower"};

std::vector<std::string> cancel_bath_menu{"Toilet", "Shower"};



// Livingroom menu (2.3)
std::vector<std::string> livingroom_cathegories{"Book", "Cancel livingroom"};

// Book liv menu    (adr 3.3.1)
std::vector<std::string> book_liv_menu{"30 min", "60 min", "120 min"};

// Cancel liv menu  (adr 3.3.2)
std::vector<std::string> cancel_liv_menu{"Slot 1", "Slot 2", "Slot 3"};



TFT_eSPI tft_main = TFT_eSPI();

const char* WIFI_SSID =  "Vik";   // name of your WiFi network
const char* WIFI_PASSWORD =  "Y897R123"; // password of the WiFi network

//
// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID = "user4";
const char* MQTT_SERVER_IP = "blueberrypie.is-very-sweet.org";
const uint16_t MQTT_SERVER_PORT = 1883;
// Common topic for system
const char* TOPIC = "My_home/mqtt";
const char* TOPIC_BACK = "booked";
const char* user = MQTT_CLIENT_ID;

// Initializing structs
Mqtt_message mqtt_message;
MQTT mqtt;


StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_payload;

#define button_up 26
#define button_right 14
#define button_down 25
#define button_left 27

bool last_up;
bool last_right;
bool last_down;
bool last_left;
bool last_buttonState;

bool pos_edge_up;
bool pos_edge_dwn;
bool pos_edge_lft;
bool pos_edge_rgt;

int lvl1_val = 1;
int lvl2_val = 1;
int lvl3_val = 1;
int lvl4_val = 1;
int lvl5_val = 1;
int lvl6_val = 1;

bool deep_end_reached = LOW;

std::vector<std::string> current_vector{};
std::vector<int>prev_menu_adress{1, 1, 1, 1, 1, 1, 1};

int current_lvl_val = 1;
int menu_lvl = 1;
bool bottom_reached = LOW;


void Mqtt_callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // Concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  Serial.println(payload);

  // Check if topic is json_topic
  if (strcmp(p_topic, TOPIC) == 0) {
    deserializeJson(Json_payload, p_payload, p_length);
  
    // Check if message is for user
    if (strcmp(Json_payload["id"], MQTT_CLIENT_ID) == 0 || strcmp(Json_payload["id"], "All") == 0) {
        if (int(Json_payload["header"]) == Doorbell) {
            Entry_doorbell();
        }
    }
  }

  // Handel all responce from topic_back
  if (strcmp(p_topic, TOPIC_BACK) == 0) {
    tft_main.setRotation(1);
    tft_main.fillScreen(TFT_BLACK);
    tft_main.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft_main.setTextSize(1);
  
    int sentence_len = 19;
    int char_index   = 0;
    int line_index   = 0;

    String a_word   = "";    // Buffer for to store words
    String sentence = "";  // Buffer to store sentences

    String line1 = "";
    String line2 = "";
    String line3 = "";

    payload = payload + " "; // Add to end to make last word append to last sentence

    for (auto x : payload) {
        if (char_index <= sentence_len) {  // Make sure sentens is no longer than x chars
            if (x == ' ') {  // Add word to sentence her
                sentence = sentence + a_word;
                sentence = sentence + " ";
                a_word = "";
            }
            else {  // Add char to a word her
                a_word = a_word + x;
            }
        }

        if (char_index == sentence_len) {  // Go to next sentence her
            sentence = "";
            line_index++;
            char_index = 0;
        }
        switch (line_index) {  // Store sentences to a line her
        case 0: line1 = sentence; break;
        case 1: line2 = sentence; break;
        case 2: line3 = sentence; break;
        }
        char_index++;
    }
    display_message(4000, strdup(line1.c_str()), strdup(line2.c_str()), strdup(line3.c_str()));
  }
}

void setup() {
  mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);

  tft_main.init();
  tft_main.setRotation(1);
  tft_main.fillScreen(TFT_BLACK);
  tft_main.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft_main.setTextSize(1);
  display_setup_messages( "Setting up",                   // Activate screen saying "Setting up"
                          "system",
                          "Please wait...");
  delay(1000);
  tft_main.fillScreen(TFT_BLACK);
  pinMode(button_up, INPUT);            // Configure SWITCH_Pin as an input  // enable pull-up resistor (active low)
  pinMode(button_right, INPUT);
  pinMode(button_down, INPUT);
  pinMode(button_left, INPUT);
  delay(100);                       // Connect to network

  Entry_setup(mqtt);

}

void loop() {
  mqtt.keepalive();
  
  int Button_up = digitalRead(button_up);
  int Button_right = digitalRead(button_right);
  int Button_down = digitalRead(button_down);
  int Button_left = digitalRead(button_left);

  // Positive edge detection Button State
  if ((Button_up == HIGH) && (last_up == LOW)) {
    pos_edge_up = HIGH;
  }                                            // Pos edge UP
  else {
    pos_edge_up = LOW;
  }

  if ((Button_down == HIGH) && (last_down == LOW)) {
    pos_edge_dwn = HIGH;
  }                                           // Pos edge DOWN
  else {
    pos_edge_dwn = LOW;
  }

  if ((Button_left == HIGH) && (last_left == LOW)) {
    pos_edge_lft = HIGH;
  }                                           // Pos edge LEFT
  else {
    pos_edge_lft = LOW;
  }

  if ((Button_right == HIGH) && (last_right == LOW)) {
    pos_edge_rgt = HIGH;
  }                                           // Pos edge RIGHT
  else {
    pos_edge_rgt = LOW;
  }


  if (pos_edge_rgt) {                                               // Increase menu lvl
    if (!deep_end_reached) {
      menu_lvl ++;
      tft_main.fillScreen(TFT_BLACK);
    }
  }
  else if (pos_edge_lft) {                                          // Decrease menu lvl
    if (menu_lvl > 1) {                                             // Prevent from going to lower level than allowed
      menu_lvl --;
      tft_main.fillScreen(TFT_BLACK);
    }
  }

  if      (menu_lvl == 1)  {
    lvl1_val = mod_val(bottom_reached, LOW, pos_edge_up, pos_edge_dwn, lvl1_val);
    current_lvl_val = lvl1_val;
    lvl2_val = 1;
  }   // 1st level - Main menu
  else if (menu_lvl == 2)  {
    lvl2_val = mod_val(bottom_reached, LOW, pos_edge_up, pos_edge_dwn, lvl2_val);
    current_lvl_val = lvl2_val;
    lvl3_val = 1;
  }   // 2nd level
  else if (menu_lvl == 3)  {
    lvl3_val = mod_val(bottom_reached, LOW, pos_edge_up, pos_edge_dwn, lvl3_val);
    current_lvl_val = lvl3_val;
    lvl4_val = 1;
  }   // 3rd level
  else if (menu_lvl == 4)  {
    lvl4_val = mod_val(bottom_reached, LOW, pos_edge_up, pos_edge_dwn, lvl4_val);
    current_lvl_val = lvl4_val;
  }   // 4th level



  std::vector<int>menu_adress{menu_lvl, lvl1_val, lvl2_val, lvl3_val, lvl4_val};

  deep_end_reached = LOW;                                         // Always reset deep end reached variable, to run new check each scan
  bottom_reached = LOW;

  // Adress format: [Menu level, lvl1 value, lvl2 value....]
  // Menu level 1
  if              (menu_adress[0] == 1)   {
    current_vector =      main_cathegories;            // 1              - Main menu
  }

  // Menu level 2
  else if         (menu_adress[0] == 2)   {                                                   // 2
    if            (menu_adress[1] == 1)   {current_vector = kitchen_cathegories;}             // 2.1            - Kitchen menu
    else if       (menu_adress[1] == 2)   {current_vector = bathroom_cathegories;}            // 2.2                 - Bathroom menu
      

    else if       (menu_adress[1] == 3)   {current_vector = livingroom_cathegories;}       // 2.3       - Livingroom menu
  }

  // Menu level 3
  //void display_message(int displaytime_ms, char* line1, char* line2, char* line3)
  else if         (menu_adress[0] == 3)   {                                                   // 3
    if            (menu_adress[1] == 1)   {                                                     // 3.1   - Kitchen
      if          (menu_adress[2] == 1)   {mqtt_message.resiver  = "Hub";                     // 3.1.1 - Book kitchen
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = Kitchen;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 1;}                                 // Must be added to avoid duplicating menu lines when returning}

      else if    (menu_adress[2] == 2)    {current_vector = cancel_kit_menu;}}                   // 3.1.2 - Choose which slot to cancel

    else if      (menu_adress[1] == 2) {                                                         // 3.2 - Bathroom
      
      if         (menu_adress[2] == 1) {current_vector = book_bath_menu;}                        // 3.2.1 - Select whether you want to use toilet or shower
   
      else if   (menu_adress[2] == 2) {mqtt_message.resiver  = "Hub";                            // 3.2.2 - Cancel bathroom slot
                                       mqtt_message.header   = Booking;
                                       mqtt_message.room     = CanBath;
                                       mqtt_message.data_String[0] = { "user", user };
                                       mqtt.pub(mqtt_message, TOPIC, false);
                                       menu_lvl = 1;
                                       menu_adress[0] = 1;
                                       current_lvl_val = 1;}}
         
    else if (menu_adress[1] == 3) {                                                               // 3.3 - Livingroom
      if (menu_adress[2] == 1) {
        current_vector = book_liv_menu; // 3.3.1 - Select for how long to book livingroom
      }
      else if (menu_adress[2] == 2) {current_vector = cancel_liv_menu;}}} // 3.3.2 - Select which livingroom slot to cancel
        
  // Menu lvl 4
  else if         (menu_adress[0] == 4) {                                                     // 4
    if            (menu_adress[1] == 1) {                                                     // 4.1
      if          (menu_adress[2] == 2) {                                                     // 4.1.2
        if        (menu_adress[3] == 1) {mqtt_message.resiver  = "Hub";                       // 4.1.2.1 - Cancel kitchen slot 1
                                         mqtt_message.header   = Booking;
                                         mqtt_message.room     = CanKit1;
                                         mqtt_message.data_String[0] = { "user", user };
                                         mqtt.pub(mqtt_message, TOPIC, false);
                                         menu_lvl = 1;
                                         menu_adress[0] = 1;
                                         current_lvl_val = 1;}

        else if     (menu_adress[3] == 2) {mqtt_message.resiver  = "Hub";                       // 4.1.2.2 - Cancel kitchen slot 2
                                          mqtt_message.header   = Booking;
                                          mqtt_message.room     = CanKit2;
                                          mqtt_message.data_String[0] = { "user", user };
                                          mqtt.pub(mqtt_message, TOPIC, false);
                                          menu_lvl = 1;
                                          menu_adress[0] = 1;
                                          current_lvl_val = 2;}}}

      else if      (menu_adress[1] == 2) {                                                      // 4.2 - Bathroom choice
        if         (menu_adress[2] == 1) {                                                      // 4.2.1
          if       (menu_adress[3] == 1) {mqtt_message.resiver  = "Hub";                        // 4.2.1.1 - Book toilet
                                          mqtt_message.header   = Booking;
                                          mqtt_message.room     = Toilet;
                                          mqtt_message.data_String[0] = { "user", user };
                                          mqtt.pub(mqtt_message, TOPIC, false);
                                          menu_lvl = 1;
                                          menu_adress[0] = 1;
                                          current_lvl_val = 1;}
                                          
          else if  (menu_adress[3] == 2) {mqtt_message.resiver  = "Hub";                        // 4.2.1.2 - Boomk shower
                                          mqtt_message.header   = Booking;
                                          mqtt_message.room     = Shower;
                                          mqtt_message.data_String[0] = { "user", user };
                                          mqtt.pub(mqtt_message, TOPIC, false);;
                                          menu_lvl = 1;
                                          menu_adress[0] = 1;
                                          current_lvl_val = 2;}}}
                                        
        
      else if     (menu_adress[1] == 3) {                                                       // 4.3 - Livingroom choice
        if        (menu_adress[2] == 1) {                                                       // 4.3.1.
          if      (menu_adress[3] == 1) {mqtt_message.resiver  = "Hub";                         // 4.3.1.1 - Book livingroom for 30 minutes
                                         mqtt_message.header   = Booking;
                                         mqtt_message.room     = Liv30;
                                         mqtt_message.data_String[0] = { "user", user };
                                         mqtt.pub(mqtt_message, TOPIC, false);
                                         menu_lvl = 1;
                                         menu_adress[0] = 1;
                                         current_lvl_val = 1;}
                                   
                                        

          else if   (menu_adress[3] == 2) {mqtt_message.resiver  = "Hub";                         // 4.3.1.2 - Book livingroom for 60 minutes
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = Liv60;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);;
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 2;}
                                            
          else if   (menu_adress[3] == 3) {mqtt_message.resiver  = "Hub";                          // 4.3.1.3 - Book livingroom for 90 minutes
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = Liv120;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 3;}}
                                                      
        else if     (menu_adress[2] == 2) {                                                        // 4.3.2 - Cancel livingroom slots

          if        (menu_adress[3] == 1) {mqtt_message.resiver  = "Hub";                          // 4.3.2.1 - cancel livingroom slot 1
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = CanLiv1;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 1;}

          else if   (menu_adress[3] == 2) {mqtt_message.resiver  = "Hub";                           // 4.3.2.2 - cancel livingroom slot 2
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = CanLiv2;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 2;}
                                                
          else if   (menu_adress[3] == 3) {mqtt_message.resiver  = "Hub";                            // 4.3.2.3 - cancel livingroom slot 3
                                           mqtt_message.header   = Booking;
                                           mqtt_message.room     = CanLiv3;
                                           mqtt_message.data_String[0] = { "user", user };
                                           mqtt.pub(mqtt_message, TOPIC, false);
                                           menu_lvl = 1;
                                           menu_adress[0] = 1;
                                           current_lvl_val = 3;}}}}
                                          
          
  if (menu_adress != prev_menu_adress) {                              // Refresh screen if menu adress has been changed (to avoid duplicated menu items)
    tft_main.fillScreen(TFT_BLACK);
  }


  display_menu(current_lvl_val, current_vector);

  if (menu_adress[menu_adress[0]] == current_vector.size()) {         // Was menu_lvl in stead of menu_adress[0]
    bottom_reached = HIGH;
  }
  else {
    bottom_reached = LOW;
  }

  // Sets button state to the old state, for comparison to the next scan
  last_up = Button_up;
  last_down = Button_down;
  last_right = Button_right;
  last_left = Button_left;


  prev_menu_adress = menu_adress; // Set current menu adress as previous menu adress for comparison next scan

}
