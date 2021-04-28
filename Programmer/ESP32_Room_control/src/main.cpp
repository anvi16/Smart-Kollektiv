/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	14/04/2021

Description: 
  Room controller
*************************************************/

// Since CoT operates very slowly, the controller will only r/w to/from CoT when in screensaver mode.

#include "Controller_config.h"
#include "OLED.h"
#include "Peripherals.h"
#include "ClassTest.h"
#include "../lib/MQTT/MQTT_Class.h"
#include "Servo.h"

//#define DEBUG

// INTERRUPT: 
volatile bool return_to_menu = LOW;


void IRAM_ATTR ISR();
// Function for "waking" from screensaver mode


// Time
unsigned long millis_prev           = 0;
bool          millis_rollover       = LOW;

// Wifi: SSID, Password
  //const char* WIFI_SSID           = "Vik";
  //const char* WIFI_PASSWORD       = "Y897R123";
const char* WIFI_SSID               = "jorgen";
const char* WIFI_PASSWORD           = "majones123";
bool        wiFi_status             = LOW;
bool        wiFi_status_prev        = LOW;
bool        wiFi_status_pos_edge    = LOW;
bool        wiFi_status_neg_edge    = LOW;
bool        cot_operational         = LOW;                // Bool saying if the CoT connection has been established after WiFi loss
unsigned long wiFi_last_connection_attempt_ts = 0;        // Timestamp (millis()) for last connection attempt to WiFi
int         wiFi_reconnection_freq  = 30 * SECOND;        // Try to reconnect every 1 minute

// WiFi: Parameters for CoT
char        ssid[]                  = "jorgen";
char        pw[]                    = "majones123";
const char* WIFI_SSID = "DESKTOP-P40U26J 5521";
const char* WIFI_PASSWORD = "g8X2684+";

// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID          = "RC_1";
const char* MQTT_SERVER_IP          = "broker.hivemq.com";
const uint16_t  MQTT_SERVER_PORT    = 1883;

// MQTT: Common topic for system
const char* MQTT_TOPIC              = "My_home/mqtt";

// OLED: Instanciate TFT_eSPI library for main function. If not instanciated, it's not possible to customize text size, text colour etc.
TFT_eSPI tft_main = TFT_eSPI();

// MQTT: Communication
MQTT mqtt;

// MQTT: Struct as message buffer
Mqtt_message mqtt_message;
StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_payload;

/*
void Mqtt_callback(char* p_topic, byte* p_payload, unsigned int p_length) {
    // Concat the payload into a string
    String payload;
    for (uint8_t i = 0; i < p_length; i++) {
        payload.concat((char)p_payload[i]);
    }
    Serial.println(payload);

    deserializeJson(Json_payload, p_payload, p_length);
    // Check if message is for user
    if (strcmp(Json_payload["id"], MQTT_CLIENT_ID) == 0 || strcmp(Json_payload["id"], "All") == 0) {
        if (int(Json_payload["header"]) == Room_Controller) {
            servoMotor(int(Json_payload["data_int"]["Outdoor_temp"])); 
        }
    }
}
*/

/*
mqtt_message.resiver = "Hub";
mqtt_message.header = Doorbell;
mqtt_message.room = Entry;
mqtt_message.data_int[0] = { "key", 10 };
mqtt_message.data_int[1] = { "key", 10 };
mqtt.pub(mqtt_message, MQTT_TOPIC, false);
*/

// Instanciate CircusESP32Lib
CircusESP32Lib CoT(COT_SERVER, ssid, pw);

void setup() {
  // Set up serial communication
  Serial.begin(115200); // Serial communication
  Servo_setup();


  // Mqtt setup
  mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);

  // Set up hardware for room controller
  hw_setup();           // Setup hardware for room controller
  
  // Serial: Set up serial communication
  Serial.begin(115200); // Serial communication
  
  
  
  // OLED: Set up initial parameters for SPI control of OLED
  tft_main.init();
  tft_main.setRotation(1);
  tft_main.fillScreen(TFT_BLACK);
  tft_main.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft_main.setTextSize(1);
  display_setup_screen();                                 // Activate screen saying "Setting up"
  delay(READTIME);                                        // Allow user to read message on screen
  
  // WiFi: Initial connection attempt
  tft_main.fillScreen(TFT_BLACK);                         // Feedback to user: Connecting WiFi
  display_setup_messages("Connecting to", "WiFi", "Please wait...");
  Serial.println("Connecting to Wireless network");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(5000);                                            // Allow WiFi to connenct
  

  // CoT: Set up CoT (Circus of Things). Will not skip to next step if WiFi is not connected, since CoT.begin() will delay eternally if no WiFi present
  if (WiFi.status() == WL_CONNECTED){                     // Execute if conencted to WiFi
    
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "WiFi", 
                            "Connected!");
    delay(READTIME);                                      // Allow user to read screen
    
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "CoT", 
                            "Please wait...");
    delay(READTIME);                                      // Allow user to read screen
    
    CoT.begin();                                          // Set up CoT connection
    
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "CoT", 
                            "Connected!");
    delay(READTIME);                                      // Allow user to read message on screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen

    wiFi_status_prev = HIGH;                              // To avoid controller from setting up WiFi + CoT again in "void loop()"
  }
  
  else{                                                   // Execute if not connected to WiFi
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "WiFi", 
                            "Failed!");
    delay(READTIME);                                      // Allow user to read screen
    
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "CoT unavailable",            // Feedback to user
                            "at the moment", 
                            "Retry later");
    delay(READTIME);                                      // Allow user to read screen
  }
  
  // HARDWARE: Set up hardware for room controller
  hw_setup();
  
  // MQTT setup
  // mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);
  
  // Instanciate temperature class
  Temperature t{23, 17, 13, 8};

  tft_main.fillScreen(TFT_BLACK);                         // Clear screen
  display_setup_messages( "Setup",                        // Feedback to user
                          "completed:", 
                          "Successfully!");
  delay(READTIME);                                            // Allow user to read screen
  tft_main.fillScreen(TFT_BLACK);                         // Clear screen

}


// MENU
    // Main menu                  (adr 1)
    std::vector<std::string> main_cathegories{"Light", "Temperature", "Fan", "Airing"};

        // Light menu               (adr 2.1)
        std::vector<std::string> light_cathegories{"Toggle", "Dim adjust", "Set intensity"};

            // Toggle                 (adr 3.1.1)
            // Toggle if selected and right push
            
            // Dim adjust             (adr 3.1.2)
            // ---> val_adjust_window

        // Temperature menu         (adr 2.2)
        std::vector<std::string> temperature_cathegories{"Temp. adjust", "Weekly plan", "Long absence"};

            // Temp adjust menu       (adr 3.2.1)
            std::vector<std::string> temperature_adjust_cathegories{"Home", "Away", "Night", "Long absence"};
            
            // Weekly plan menu       (adr 3.2.2)
            std::vector<std::string> weekplan_cathegories{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};


        // Fan menu (3)
        std::vector<std::string> fan_cathegories{"Override auto.", "Fan power"};

        // Airing menu (4)
        std::vector<std::string> airing_cathegories{"Override auto.", "Opening () man"};
//

// LIGHT: Variables for light control in the room
bool roomlight_state                = LOW;                // Variable determining if the light in the room should be ON or OFF
int roomlight_intensity_measured    = 0;                  // Measured luminosity in the room
int roomlight_intensity_percentage  = 100;                // Variable giving possibility to adjust the light intensity in the room downwards. Used in dimmer 
int roomlight_setpoint_intensity    = 240;                // Set fairly bright example value. can be calbrated later by user
int roomlight_intensity_requested_dimmed;                 // The requested roomlight value after dimming is applied
int roomlight_dutycycle             = 150;                // Variable controlling the duty cycle of the PWM.
bool cot_light_toggle               = LOW;                // Light toggle status read from CoT
bool cot_light_toggle_prev          = LOW;                // Light toggle status read from CoT - previous scan
int cot_light_dimming               = 100;                // Light dimming status read from CoT
int cot_light_dimming_prev          = 100;                // Light dimming status read from CoT - previous scan

// TEMPERATURE: Variables for temperature control
bool long_absence                   = LOW;                // Variable active
int  temperature_profile            = 0;                  // States of temperature - 0 = Home, 1 = Away, 2 = Night, 3 = Long absence
int  temperature_set;                                     // Variable containing the new temperature to be set for the curret temperature profile
std::vector<int> temperatures{23, 17, 13, 8};             // Preset temperatures [Home, Away, Night, Long absence]
int cot_temp_home                   = temperatures[0];    // Set temperature read from CoT - Home
int cot_temp_home_prev              = temperatures[0];    // Set temperature read from CoT - Home - Value of previous scan
int cot_temp_away                   = temperatures[1];    // Set temperature read from CoT - Away
int cot_temp_away_prev              = temperatures[1];    // Set temperature read from CoT - Away - Value of previous scan
int cot_temp_night                  = temperatures[2];    // Set temperature read from CoT - Night
int cot_temp_night_prev             = temperatures[2];    // Set temperature read from CoT - Night - Value of previous scan
int cot_temp_long_abs               = temperatures[3];    // Set temperature read from CoT - Long absence
int cot_temp_long_abs_prev          = temperatures[3];    // Set temperature read from CoT - Long absence - Value of previous scan
int temperature_read_raw            = 0;                  // Read room temperature raw value
int temperature_read_c              = 0;                  // Read room temperature in degrees
int temperature_read_interval       = 10 * SECOND;        // Set interval for reading room temperature to 10 sec
unsigned long temperature_read_ts   = 0;                  // Timestamp for last time room temp was read

// FAN: Variables for fan control
bool fan_override_auto              = LOW;                // Overriding the automatic program for fan control
int fan_power_percent               = 30;                 // Percentage value giving the power of the fan in percent.
int cot_fan_power                   = 100;                // Set percentage value read from CoT - fan power
int cot_fan_power_prev              = 100;                // Set percentage value read from CoT - fan power - Value of previous scan

// AIRING: Variables for airing control
int cot_airing_opening              = 0;                  // Set percentage value read from CoT - airing window opening (degrees)
int cot_airing_opening_prev         = 0;                  // Set percentage value read from CoT - airing window opening (degrees) - Value of previous scan

// NAVIGATION: Variables for menu navigation
bool  buttonstate_up, buttonstate_dwn, buttonstate_lft, buttonstate_rgt;   // Declare values representing button states
volatile unsigned long buttonpress_ts;
bool  val_adjust                    = LOW;                // Var selecting if the view to be shown is a menu or someting else
bool  temp_adjust                   = LOW;                // Var selecting if the view to be shown is for temperature modification
int   adjusted_value;                                     // Containing the variable currently being altered
char* adjusted_value_name;                                // Name of the variable currently being altered (for visualisation on screen)
volatile bool  screensaver          = LOW;                // Giving signal if user has been inactive and screensaver must be applied
bool  screensaver_prev              = LOW;                // For compariosn next round anf pos edge detect
int   screensaver_activate_time     = 30 * SECOND;        // If inactive for more than 15 seconds, activate screensaver

// NAVIGATION: Declare and set initial button states 
bool  prev_buttonstate_up           = LOW;
bool  prev_buttonstate_dwn          = LOW;
bool  prev_buttonstate_lft          = LOW;
bool  prev_buttonstate_rgt          = LOW;

bool pos_edge_up                    = LOW;
bool pos_edge_dwn                   = LOW;
bool pos_edge_lft                   = LOW;
bool pos_edge_rgt                   = LOW;

// NAVIGATION: Variables for menu navigation
int menu_lvl                        = 0;
int lvl1_val                        = 0;
int lvl2_val                        = 0;
int lvl3_val                        = 0;
int lvl4_val                        = 0;
int lvl5_val                        = 0;
int lvl6_val                        = 0;

int current_lvl_val                 = 1;                  // Value for "display_menu" function, so it knows which element is active

bool deep_end_reached               = LOW;                // Variable giving feedback if deepest level of menu is reached
bool modify_val_percent             = LOW;                // Variable giving feedback if the value to be adjusted is a percentage value (only 0-100), (interval 5)
bool bottom_reached                 = LOW;                // Variable giving feedback if bottom button in menu is reached

std::vector<std::string> current_vector{};
std::vector<int>prev_menu_adress{1,1,1,1,1,1,1};


void loop() {
  
  // TIME: Check for millis rollover, and reset timestamp values to 0
  if (millis() < millis_prev){                            // If millis() value read is lower than the previously read value, a rollover must have occurred 
    millis_rollover                 = HIGH;
    millis_prev                     = 0;                  // Reset "previous millis" value to cater for millis rollover. Will result in a small time descrepancy, but not problematic in this application
    buttonpress_ts                  = 0;                  // --,,--
    temperature_read_ts             = 0;                  // --,,--
    wiFi_last_connection_attempt_ts = 0;                  // --,,--
  }
  else {
    millis_rollover = LOW;
  }

  // LIGHT: Read measured light intensity in room and convert from 10 to 8 bit
  analogReadResolution(10);
  roomlight_intensity_measured      = analogRead(pin_light_sensor);
  roomlight_intensity_measured      = roomlight_intensity_measured/4; // Need to use 10 bit reading because HW does not handle lower resolution. We then devide by 4 to get a 8 bit value

  // TEMPERATURE: Read room temperature
    if (millis() >= temperature_read_ts + temperature_read_interval){ //To avoid reading temperature each cycle
    // Get temp sensor reading
    analogReadResolution(10);
    temperature_read_raw              = analogRead(pin_temp_sensor);

    // Convert to millivolts
    temperature_read_raw = temperature_read_raw * OUTPUT_MILLIVOLT / 1024; //10 bit resoultion

    // Convert to C
    temperature_read_c = (temperature_read_raw - 500) / 10 + 10;           //https://learn.adafruit.com/tmp36-temperature-sensor

    temperature_read_ts = millis();
  }
  // NAVIGATION: Read navigation pushbuttons
  buttonstate_up                    = digitalRead(pin_up);
  buttonstate_dwn                   = digitalRead(pin_dwn);
  buttonstate_lft                   = digitalRead(pin_lft);
  buttonstate_rgt                   = digitalRead(pin_rgt);

  // NAVIGATION: Pushutton edge detection
  if ((buttonstate_up == HIGH) && (prev_buttonstate_up == LOW))   { pos_edge_up = HIGH;                 // Pos edge UP
                                                                    buttonpress_ts = millis();}      
  else {pos_edge_up = LOW;}

  if ((buttonstate_dwn == HIGH) && (prev_buttonstate_dwn == LOW)) { pos_edge_dwn = HIGH;                // Pos edge DOWN
                                                                    buttonpress_ts = millis();}   
  else {pos_edge_dwn = LOW;}

  if ((buttonstate_lft == HIGH) && (prev_buttonstate_lft == LOW)) { pos_edge_lft = HIGH;                // Pos edge LEFT
                                                                    buttonpress_ts = millis();}   
  else {pos_edge_lft = LOW;}

  if ((buttonstate_rgt == HIGH) && (prev_buttonstate_rgt == LOW)) { pos_edge_rgt = HIGH;                // Pos edge RIGHT
                                                                    buttonpress_ts = millis();}   
  else {pos_edge_rgt = LOW;}

  
  // SCREENSAVER: Activation timer
  if (millis() >= (buttonpress_ts + screensaver_activate_time))   {screensaver = HIGH;}
  else                                                            {screensaver = LOW;}

  // SCREENSAVER: 
  if (screensaver){
    
    attachInterrupt(pin_lft, ISR, RISING);
    attachInterrupt(pin_rgt, ISR, RISING);
    attachInterrupt(pin_up, ISR, RISING);
    attachInterrupt(pin_dwn, ISR, RISING);

    if (!screensaver_prev){                               // Reset screen if pos edge 
      tft_main.fillScreen(TFT_BLACK);}                    
    
    display_screensaver(temperature_read_c, "Current temp.");
  }
  else if (!screensaver && screensaver_prev){             // Reset screen before activating a different screen
    tft_main.fillScreen(TFT_BLACK);
  }

  // MQTT loop
  /////////////////////////////mqtt.keepalive();


  // WiFi: booleans displaying connection status
  if (WiFi.status() == WL_CONNECTED)        { wiFi_status = HIGH;}
  else                                      { wiFi_status = LOW;}

  // WiFi: Just connected
  if (wiFi_status && !wiFi_status_prev)     { wiFi_status_pos_edge = HIGH;}
  else                                      { wiFi_status_pos_edge = LOW;}
  
  // WiFi: Just disconnected 
  if (!wiFi_status && wiFi_status_prev)     { wiFi_status_neg_edge = HIGH;}
  else                                      { wiFi_status_neg_edge = LOW;} 

  wiFi_status_prev = wiFi_status;                         // Set wiFi status for next scan

  // WiFi: Connection lost message 
  if (wiFi_status_neg_edge){
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "WiFi connection",            // Feedback to user
                            "lost", 
                            "Retry later...");
    delay(READTIME);                                      // Allow user to read screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }
  
  // CoT: Set up new CoT connection only if not previously connected
  if (wiFi_status_pos_edge){
    if (!screensaver){
      tft_main.fillScreen(TFT_BLACK);                     // Clear screen
      display_setup_messages( "Connecting to",            // Feedback to user
                              "WiFi", 
                              "Connected!");
      delay(READTIME);                                    // Allow user to read screen
    
      tft_main.fillScreen(TFT_BLACK);                     // Clear screen
      display_setup_messages( "Connecting to",            // Feedback to user
                              "CoT", 
                              "Please wait...");
      delay(READTIME - 1*SECOND);                         // Allow user to read screen
    }

    CoT.begin();
    
    cot_operational                 = HIGH;               // Bool saying if the CoT connection has been established after WiFi loss
    
    if (!screensaver){
      tft_main.fillScreen(TFT_BLACK);                     // Clear screen
      display_setup_messages( "Connecting to",            // Feedback to user
                              "CoT", 
                              "Connected!");
      delay(READTIME);                                    // Allow user to read screen
      tft_main.fillScreen(TFT_BLACK);                     
    } 
  }                          
  
  // WiFi: Attempt to reconnect with given intervals
  else if ((!wiFi_status) && (millis() >= (wiFi_last_connection_attempt_ts + wiFi_reconnection_freq))){

    if (!screensaver){
      tft_main.fillScreen(TFT_BLACK);
      display_setup_messages( "Connecting to", 
                              "WiFi", 
                              "Please wait...");
    }
   
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    delay(5000);                                          // Delay added to allow for WiFi to connect

    cot_operational                 = LOW;                // Bool saying if the CoT connection has been established after WiFi loss
    wiFi_last_connection_attempt_ts = millis();           // Set new timestamp for last connection atempt
    tft_main.fillScreen(TFT_BLACK);                       // Reset screen for next item to pop up
  }

  // SCREENSAVER: Background operations when screensaver is activated
  else if (wiFi_status && screensaver){                   // Only operate CoT when screensaver is activated
    
    if (cot_operational){
      
      // CoT: Each of the CoT operations are veru time consuming
      if (!return_to_menu)  { cot_light_toggle    = CoT.read(KEY_LIGHT_TOGGLE,  TOKEN);}
      if (!return_to_menu)  { cot_light_dimming   = CoT.read(KEY_LIGHT_DIMMING, TOKEN);}
      if (!return_to_menu)  { cot_temp_home       = CoT.read(KEY_TEMP_HOME,     TOKEN);}
      if (!return_to_menu)  { cot_temp_away       = CoT.read(KEY_TEMP_AWAY,     TOKEN);}
      if (!return_to_menu)  { cot_temp_night      = CoT.read(KEY_TEMP_NIGHT,    TOKEN);}
      if (!return_to_menu)  { cot_temp_long_abs   = CoT.read(KEY_TEMP_LONG_ABS, TOKEN);}
      if (!return_to_menu)  { cot_fan_power       = CoT.read(KEY_FAN_POWER,     TOKEN);}
      if (!return_to_menu)  { cot_airing_opening  = CoT.read(KEY_AIRING_OPENING,TOKEN);}
    }
  } 

    
  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                                  MENU NAVIGATION                                //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////

  // MENU: Navigation
  if (!screensaver){                                      // When screensaver is active, the push of a button should only "wake" the controller
    // Change menu level
    if (pos_edge_rgt){              // Increase menu level
        if (!deep_end_reached){
            menu_lvl ++;
            tft_main.fillScreen(TFT_BLACK);
            val_adjust = LOW;                             // To solve bug of duplicated menu lines being written after having altered a value
        }
    }
    else if (pos_edge_lft){                               // Decrease menu level
        if (menu_lvl > 1){                                // Prevent from going to lower edit level than allowed
            menu_lvl --;
            tft_main.fillScreen(TFT_BLACK);
            val_adjust = LOW;                             // To solve bug of duplicated menu lines being written after having altered a value 
        }
    }


    // Decide which level in menu is active and which adressing value to modify
    if (!val_adjust){
      if      (menu_lvl == 1)  {lvl1_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl1_val);
                                current_lvl_val = lvl1_val;
                                lvl2_val = 1;                                                                               }   // 1st level - Main menu
      else if (menu_lvl == 2)  {lvl2_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl2_val);
                                current_lvl_val = lvl2_val;
                                lvl3_val = 1;                                                                               }   // 2nd level
      else if (menu_lvl == 3)  {lvl3_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl3_val);
                                current_lvl_val = lvl3_val;
                                lvl4_val = 1;                                                                               }   // 3rd level
      else if (menu_lvl == 4)  {lvl4_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl4_val);
                                current_lvl_val = lvl4_val;
                                lvl5_val = 1;                                                                               }   // 4th level
      else if (menu_lvl == 5)  {lvl5_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl5_val);
                                current_lvl_val = lvl5_val;
                                lvl6_val = 1;                                                                               }   // 5th level
      else if (menu_lvl == 6)  {lvl6_val = mod_val(bottom_reached, modify_val_percent, pos_edge_up, pos_edge_dwn, lvl6_val);
                                current_lvl_val = lvl6_val;                                                                 }   // 6th level
    }
  }
  
  // Create adress for deciding which display should be drawn
  std::vector<int>menu_adress{menu_lvl, lvl1_val, lvl2_val, lvl3_val, lvl4_val, lvl5_val, lvl6_val};
  
  deep_end_reached = LOW;                                 // Always reset deep end reached variable, to run new check each scan
  modify_val_percent = LOW;
  bottom_reached = LOW;

  val_adjust = LOW;
  temp_adjust = LOW;
  

  // Adress format: [Menu level, lvl1 value, lvl2 value....]
  // Menu level 1
  if              (menu_adress[0] == 1)   {current_vector =      main_cathegories;            // 1              - Main menu
  }

  // Menu level 2
  else if         (menu_adress[0] == 2)   {                                                   // 2 
      if          (menu_adress[1] == 1)   {current_vector =      light_cathegories;}          // 2.1            - Light menu
      else if     (menu_adress[1] == 2)   {current_vector =      temperature_cathegories;}    // 2.2            - Temperature menu
      else if     (menu_adress[1] == 3)   {current_vector =      fan_cathegories;}            // 2.3            - Fan menu
      else if     (menu_adress[1] == 4)   {current_vector =      airing_cathegories;}         // 2.4            - Airing menu
  }

  // Menu level 3
  else if         (menu_adress[0] == 3)   {                                                   // 3 
      
      if          (menu_adress[1] == 1)   {                                                   // 3.1   - LIGHT
          if      (menu_adress[2] == 1)   {roomlight_state = !roomlight_state;                // 3.1.1 - Toggle light
                                           toggle_popup(roomlight_state, "Light");
                                           menu_lvl = 2;
                                           menu_adress[0] = 2;
                                           current_lvl_val = 1;}                              // Must be added to avoid duplicating menu lines when returning} 
          else if (menu_adress[2] == 2)   {                                                   // 3.1.2 - Dim adjust view
                                           roomlight_intensity_percentage = mod_val(LOW, HIGH, pos_edge_up, pos_edge_dwn, roomlight_intensity_percentage);
                                           adjusted_value = roomlight_intensity_percentage;  
                                           adjusted_value_name = "Light intensity";
                                           val_adjust = HIGH;
                                           deep_end_reached = HIGH;}
          else if (menu_adress[2] == 3)   {                                                  // 3.1.3 - Set current light intensity
                                           roomlight_setpoint_intensity = light_intensity_setpoint(pin_light_sensor, 8); // Set setpoint for wanted light value
                                           display_message(1500, "Wait...", "Reading light", "intensity");                // Window showing that calibration is being performed
                                           menu_lvl = 2;                                     // Return to previous menu level
                                           menu_adress[0] = 2;
                                           current_lvl_val = 3;                               // Must be added to avoid duplicating menu lines when returning
                                           
                                           }                                
      }
    
      else if     (menu_adress[1] == 2)   {                                                   // 3.2   - TEMPERATURE
          if      (menu_adress[2] == 1)   {current_vector = temperature_adjust_cathegories;}  // 3.2.1 - Weekly plan menu
          else if (menu_adress[2] == 2)   {current_vector = weekplan_cathegories;}            // 3.2.2 - Temp adjust window
          else if (menu_adress[2] == 3)   {long_absence = !long_absence;                      // 3.2.3 - Long absence toggle
                                           toggle_popup(long_absence, "Long absence");
                                           menu_lvl = 2;
                                           menu_adress[0] = 2;
                                           current_lvl_val = 3;}                           
      }

      else if     (menu_adress[1] == 3)   {                                                   // 3.3   - FAN
          if      (menu_adress[2] == 1)   {/* Override time 1hr, 2hr, 3hr view*/;}            // 3.3.1 - Override auto time view
          else if (menu_adress[2] == 2)   {/* Fan adjust +/- 5% view*/;}                      // 3.3.2 - Fan power window
      }

      else if     (menu_adress[1] == 4)   {                                                   // 3.4   - AIRING
          if      (menu_adress[2] == 1)   {/* Override time 1hr, 2hr, 3hr view*/;}            // 3.4.1 - Override auto time view
          else if (menu_adress[2] == 2)   {/* Airing adjust +/- 5% view*/;                    // 3.4.2 - Opening percent
                                              deep_end_reached = HIGH;}                     
      }
  }

  // Menu level 4
  else if             (menu_adress[0] == 4)   {                                                  // 4
      if              (menu_adress[1] == 2)   {                                                  // 4.2       - TEMPERATURE
          if          (menu_adress[2] == 1)   {                                                  // 4.2.1     - Weekly plan menu 
                                                temperature_profile = menu_adress[3] - 1;
                                                temperature_set = temperatures[temperature_profile];
                                                temperature_set = mod_temp(pos_edge_up, pos_edge_dwn, temperature_set);
                                                temp_adjust = HIGH;}
      }
  } 
  
  // MENU: Display the menu decided by the menu adressing
  
  if (menu_adress != prev_menu_adress){                   // Refresh screen if menu adress has been changed (to avoid duplicated menu items) 
    tft_main.fillScreen(TFT_BLACK);}

  if (val_adjust && !screensaver){                        // If the current task requires adjusting a value, the value adjustment window should be displayed
    val_adjust_window(adjusted_value, adjusted_value_name);}

  else if (temp_adjust && !screensaver){
    temperatures[temperature_profile] = temperature_set;
    temp_adjust_window(temperatures[temperature_profile], "Temperature");
  }

  else if(!screensaver){                                  // If no "special" window needs to be displayed, display menu with correct cathegories
    display_menu(current_lvl_val, current_vector);}
  

  // MENU: Check if last item in menu is reached. If so, it is not possible to scroll further down
  if (menu_adress[menu_lvl] == current_vector.size()){
    bottom_reached = HIGH;}
  else {
    bottom_reached = LOW;}

  
  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                                   LIGHT CONTROL                                 //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////

  // LIGHT: Convert light from percentage value to 8bit int value (0-255)
  roomlight_intensity_requested_dimmed = roomlight_setpoint_intensity * roomlight_intensity_percentage / 100;

  // LIGHT: Automatic regulation of the light intensity (hysteresis of +/- 5)
  if ((roomlight_intensity_measured < (roomlight_intensity_requested_dimmed - 5)) && (roomlight_dutycycle < 255)){ 
    roomlight_dutycycle +=5;}
  else if ((roomlight_intensity_measured > (roomlight_intensity_requested_dimmed + 5)) && (roomlight_dutycycle > 0)){
    roomlight_dutycycle -=5;}
  
  // LIGHT: Set light intensity based on input variables
  if ((roomlight_state) && (roomlight_dutycycle > 10)){       // Toggle ON (set 10 as lowest value to avoid glowing bulb)
    ledcWrite(CH1, roomlight_dutycycle);}
    //digitalWrite(pin_LED_room, HIGH);}
  else{                                                       // Toggle OFF
    ledcWrite(CH1, 0);}

  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                                TEMPERATURE CONTROL                              //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////

  // TEMPERATURE: Background operations 
  if (wiFi_status && screensaver)    {                    // Only operate CoT when screensaver is activated

  }


  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                              CIRCUS OF THINGS - WRITE                           //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////
  
  // Light values
  ////////////////////CoT.write(key_light_toggle,   1000, token);
  ////////////////////CoT.write(key_light_dimming,  1000, token);

  // Temperature values
  ////////////////////CoT.write(key_temp_home,      1000, token);
  ////////////////////CoT.write(key_temp_away,      1000, token);
  ////////////////////CoT.write(key_temp_night,     1000, token);
  ////////////////////CoT.write(key_temp_long_abs,  1000, token);

  // Fan values
  ////////////////////CoT.write(key_fan_power,      1000, token);

  // Airing values
  ////////////////////CoT.write(key_airing_opening, 1000, token);


  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                             GENERAL END-OF-LOOP TASKS                           //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////

  // Set button states to "old button state" for comparison next scan
  prev_buttonstate_up = buttonstate_up;
  prev_buttonstate_dwn = buttonstate_dwn;
  prev_buttonstate_lft = buttonstate_lft;
  prev_buttonstate_rgt = buttonstate_rgt;

  // Set current menu adress as previous menu adress for comparison next scan
  prev_menu_adress = menu_adress;

  // Set current CoT values as previous CoT values for comparison next scan
  cot_light_toggle_prev   = cot_light_toggle;         // Light        - toggle
  cot_light_dimming_prev  = cot_light_dimming;        // Light        - dim
  cot_temp_home_prev      = cot_temp_home;            // Temperature  - home
  cot_temp_away_prev      = cot_temp_away;            // Temperature  - away
  cot_temp_night_prev     = cot_temp_night;           // Temperature  - night
  cot_temp_long_abs_prev  = cot_temp_long_abs;        // Temperature  - long absence
  cot_fan_power_prev      = cot_fan_power;            // Fan          - power
  cot_airing_opening_prev = cot_airing_opening;       // Aiting       - opening (degrees)

  
  // Time
  millis_prev = millis();

  // Screensaver
  screensaver_prev = screensaver;

  Serial.print("Roomlight dutycycle:      ");
  Serial.println(roomlight_dutycycle);
  Serial.print("Roomlight state:      ");
  Serial.println(roomlight_state);

  // INTERRUPT
  return_to_menu = LOW;

 
}

void IRAM_ATTR ISR(){                                 // Function for "waking" from screensaver mode
  
  TFT_eSPI().fillScreen(TFT_BLACK);
  display_setup_messages( "Waking up",            // Feedback to user
                          "unit", 
                          "Please wait...");

  return_to_menu  = HIGH;
  screensaver     = LOW;
  buttonpress_ts  = millis();

  // When no longer in screensaver mode, buttons resume to normal operation
  detachInterrupt(pin_up);
  detachInterrupt(pin_dwn);
  detachInterrupt(pin_lft);
  detachInterrupt(pin_rgt);
}
RTC_DATA_ATTR int x = 0;  // Variable saved even when in deep sleep





  
  
  if ((roomlight_state) && (roomlight_dutycycle > 6)){                       // Toggle ON
    ledcWrite(CH1, roomlight_dutycycle);
  }
  else{                                       // Toggle OFF
    ledcWrite(CH1, 0);
  }
  
  // Just checking how sensor reacts to LED
  #ifdef DEBUB
  Serial.print("Roomlight intensity mesured: ");
  Serial.println(roomlight_intensity_measured);
  Serial.print("Roomlight intensity requested dimmed: ");
  Serial.println(roomlight_intensity_requested_dimmed);
  Serial.print("Roomlight intensity setpoint: ");
  Serial.println(roomlight_setpoint_intensity);
  Serial.print("Roomlight duty cycle: ");
  Serial.println(roomlight_dutycycle);
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
#endif
  delay(50);
  




