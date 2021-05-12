/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	14/04/2021

Description: 
  Room controller

Resp: 
  Jorgen Andreas Mo

*************************************************/

// Since CoT operates very slowly, the controller will only r/w to/from CoT when in screensaver mode.

#include "Controller_config.h"
#include "OLED.h"
#include "Peripherals.h"
#include "RoomControl.h"
#include "../lib/MQTT/MQTT_Class.h"
#include "HWClass.h"

//#define DEBUG

// INTERRUPT: Function for "waking" from screensaver mode
void IRAM_ATTR ISR();

// MQTT: Predeclare func 
void Mqtt_callback(char* , byte* , unsigned int);

// TIME
unsigned long millis_prev           = 0;
bool          millis_rollover       = LOW;
const long    utcOffsetInSeconds    = 2 * 3600;
char          daysOfTheWeek[7][12]  = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int           time_ss_prev          = 0;

// WiFi: SSID, Password
const char* WIFI_SSID               = "jorgen";           // Jorgen
const char* WIFI_PASSWORD           = "majones123";       // Jorgen
// const char* WIFI_SSID               = "DESKTOP-P40U26J 5521";   // Albertin
// const char* WIFI_PASSWORD           = "g8X2684+";               // Albertin

bool        wiFi_status             = LOW;
bool        wiFi_status_prev        = LOW;
bool        wiFi_status_pos_edge    = LOW;
bool        wiFi_status_neg_edge    = LOW;
unsigned long wiFi_last_connection_attempt_ts = 0;        // Timestamp (millis()) for last connection attempt to WiFi
int         wiFi_reconnection_freq  = 30 * SECOND;        // Try to reconnect every 1 minute

// WiFi: Parameters for CoT
char        ssid[]                  = "jorgen";           // Jorgen
char        pw[]                    = "majones123";       // Jorgen
// char        ssid[]                  = "DESKTOP-P40U26J 5521";   // Albertin
// char        pw[]                    = "g8X2684+";               // Albertin

// ID
int         id_room                 = 0;                  // Declare variable for which room the controller should be configured for (0 = NaN)

std::vector<std::string> id_room_list   { "Dormroom 1",   // Rooms in house to be displayed on OLED when asking which room controller should be configured for.
                                          "Dormroom 2", 
                                          "Dormroom 3", 
                                          "Dormroom 4", 
                                          "Dormroom 5", 
                                          "Dormroom 6", 
                                          "Livingroom", 
                                          "Kitchen", 
                                          "Bathroom", 
                                          "Entry"
};

// NAVIGATION
int current_lvl_val                 = 0;                  // Value for "display_menu" function, so it knows which element is active
int menu_lvl                        = 0;  
bool bottom_reached                 = LOW;                // Variable giving feedback if bottom button in menu is reached

// NAVIGATION: Declare and set initial button states 
bool  buttonstate_up, buttonstate_dwn, buttonstate_lft, buttonstate_rgt;   // Declare values representing button states

//CoT
bool        cot_operational         = LOW;                // Bool saying if the CoT connection has been established after WiFi loss

// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID          = "RC";
const char* MQTT_SERVER_IP          = "blueberrypie.is-very-sweet.org";  //"broker.hivemq.com";
const uint16_t  MQTT_SERVER_PORT    = 1883;

// MQTT: Common topic for system
const char* MQTT_TOPIC              = "My_home/mqtt";

// MQTT: Template for sendig data
/*
mqtt_message.resiver = "Hub";
mqtt_message.header = Doorbell;
mqtt_message.room = Entry;
mqtt_message.data_int[0] = { "key", 10 };
mqtt_message.data_int[1] = { "key", 10 };
mqtt.pub(mqtt_message, MQTT_TOPIC, false);
*/

// OLED: Instanciate TFT_eSPI library for main function. If not instanciated, it's not possible to customize text size, text colour etc.
TFT_eSPI tft_main = TFT_eSPI();

// MQTT: Communication
MQTT mqtt;

// MQTT: Struct as message buffer
Mqtt_message mqtt_message;
StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_payload;

// Instanciate CircusESP32Lib
CircusESP32Lib CoT(COT_SERVER, ssid, pw);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// ROOM CONTROL - Instanciate object
RoomControl roomControl;

// AIRING: Instanciate Servo
Servo servo;

// BUTTON: Initialize button objects
Button up;
Button down;
Button left;
Button right;


void setup() {
  
  // SERIAL: Set up serial communication
  Serial.begin(115200);                                   // Serial communication
  
  // OLED: Set up initial parameters for SPI control of OLED
  tft_main.init();  
  tft_main.setRotation(1);
  tft_main.fillScreen(TFT_BLACK);
  tft_main.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft_main.setTextSize(1);
  display_setup_messages( "Setting up",                   // Activate screen saying "Setting up"
                          "system", 
                          "Please wait...");
  delay(READTIME);                                        // Allow user to read message on screen

  // HARDWARE:
  hw_setup();                                             // Setup hardware for room controller
  
  // SERVO: Setup
  servo.attach(pin_servo, 500, 2400);
  
  // WiFi: Initial connection attempt
  tft_main.fillScreen(TFT_BLACK);                         // Feedback to user: Connecting WiFi
  display_setup_messages( "Connecting to",
                          "WiFi", 
                          "Please wait...");
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
    cot_operational                 = HIGH;               // Gives feedback that CoT is up and running
    
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
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }

  // TIME SYSTEM: Read internet time
  if (WiFi.status() == WL_CONNECTED){                     // Execute if conencted to WiFi

    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "time sync server", 
                            "Please wait...");
    delay(READTIME);                                      // Allow user to read screen


    timeClient.begin();
    timeClient.update();                                  // Set new system time
    setTime(timeClient.getHours(),                         
            timeClient.getMinutes(), 
            timeClient.getSeconds(), 
            timeClient.getFormattedDate().substring(8,10).toInt(),    // DAY
            timeClient.getFormattedDate().substring(5,7).toInt(),     // MONTH
            timeClient.getFormattedDate().substring(0,4).toInt() );   // YEAR


    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "time sync server", 
                            "Connected!");
    delay(READTIME);                                      // Allow user to read message on screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }
  
  else{                                                   // Execute if not connected to WiFi
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Time sync server",           // Feedback to user
                            "unavailable", 
                            "Retry later");
    delay(READTIME);                                      // Allow user to read screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }

  // MQTT: Set up MQTT communication
  if (WiFi.status() == WL_CONNECTED){                     // Execute if conencted to WiFi

    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "MQTT", 
                            "Please wait...");
    delay(READTIME);                                      // Allow user to read screen

    mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);
    
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Connecting to",              // Feedback to user
                            "MQTT", 
                            "Connected!");
    delay(READTIME);                                      // Allow user to read message on screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }
  
  else{                                                   // Execute if not connected to WiFi
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "MQTT unavailable",           // Feedback to user
                            "at the moment", 
                            "Retry later");
    delay(READTIME);                                      // Allow user to read screen
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }


  // ID: Prepare menu system for handling room setup
  if (id_room == 0){

    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
    display_setup_messages( "Configuration:",
                            "Please select", 
                            "Room");
    delay(READTIME);
    tft_main.fillScreen(TFT_BLACK);                       // Clear screen
  }
  
  // ID: Select room ID
  while (id_room == 0){
    
    // Update current button statuses
    up.setState(digitalRead(pin_up));
    down.setState(digitalRead(pin_dwn));
    left.setState(digitalRead(pin_lft));
    right.setState(digitalRead(pin_rgt));
    
    if (right.posEdge()){                                 // Increase menu level
      menu_lvl ++;
      tft_main.fillScreen(TFT_BLACK);
    }
    
    // Navigation                                                                
    display_menu(current_lvl_val, id_room_list);

    if      (menu_lvl == 1)  {current_lvl_val                 = mod_val(bottom_reached, LOW, up.posEdge(), down.posEdge(), current_lvl_val);}
    else if (menu_lvl == 2)  {id_room                          = (current_lvl_val);
                              tft_main.fillScreen(TFT_BLACK);
                              display_setup_messages("Room","selected:", id_room_list[current_lvl_val - 1].c_str());
                              delay(READTIME);
                              current_lvl_val                 = 1;
                              menu_lvl                        = 0;
    }                                                                     

  
    if (current_lvl_val == id_room_list.size())   {bottom_reached = HIGH;}
    else                                          {bottom_reached = LOW;}
  }

  // ROOM CONTROL: Set initial values
  roomControl.setRoomId(id_room);                         // Set room ID
  roomControl.setTempHysteresis(1);                       // Set temperature hysteresis (+/- 1C)
  roomControl.setFanWattage(100);                         // Fan motor    = 100W
  roomControl.setHeaterWattage(900);                      // Heater power = 900W

  // SETUP: Finished
  tft_main.fillScreen(TFT_BLACK);                         // Clear screen
  display_setup_messages( "Setup",                        // Feedback to user
                          "procedure:", 
                          "Completed!");
  delay(READTIME);                                        // Allow user to read screen
  tft_main.fillScreen(TFT_BLACK);                         // Clear screen

  current_lvl_val                 = 1;                    // Value for "display_menu" function, so it knows which element is active
  menu_lvl                        = 1;
  bottom_reached                  = LOW;                  // Variable giving feedback if bottom button in menu is reached
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

            // Temp adjust menu       (adr 3.2.1 and 4.2.2.x)
            std::vector<std::string> temperature_adjust_cathegories{"Home", "Away", "Night", "Long absence"};
            
            // Weekly plan menu       (adr 3.2.2)
            std::vector<std::string> weekplan_cathegories{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

              // Daily plan menu       (adr 4.2.2.x)
              std::vector<std::string> hour_cathegories{"00.00 - 01.00",
                                                        "01.00 - 02.00", 
                                                        "02.00 - 03.00", 
                                                        "03.00 - 04.00", 
                                                        "04.00 - 05.00", 
                                                        "05.00 - 06.00", 
                                                        "06.00 - 07.00", 
                                                        "07.00 - 08.00", 
                                                        "08.00 - 09.00", 
                                                        "09.00 - 10.00", 
                                                        "10.00 - 11.00", 
                                                        "11.00 - 12.00", 
                                                        "12.00 - 13.00", 
                                                        "13.00 - 14.00", 
                                                        "14.00 - 15.00", 
                                                        "15.00 - 16.00", 
                                                        "16.00 - 17.00", 
                                                        "17.00 - 18.00", 
                                                        "18.00 - 19.00", 
                                                        "19.00 - 20.00", 
                                                        "20.00 - 21.00", 
                                                        "21.00 - 22.00",
                                                        "22.00 - 23.00",
                                                        "23.00 - 00.00",};
            // Temp adjust menu       (adr 3.2.1 and 4.2.2.x)
            std::vector<std::string> temperature_set_cathegories{"Home", "Away", "Night"};

        // Fan menu (3)
        std::vector<std::string> fan_cathegories{"Override auto.", "Fan power"};

        // Airing menu (4)
        std::vector<std::string> airing_cathegories{"Override auto.", "Man. opening"};
//

// LIGHT: Variables for light control in the room
bool roomlight_state                = LOW;                // Variable determining if the light in the room should be ON or OFF
int roomlight_intensity_measured    = 0;                  // Measured luminosity in the room
int roomlight_intensity_percentage  = 100;                // Variable giving possibility to adjust the light intensity in the room downwards. Used in dimmer 
int roomlight_setpoint_intensity    = 240;                // Set fairly bright example value. can be calbrated later by user
int roomlight_intensity_requested_dimmed;                 // The requested roomlight value after dimming is applied
int roomlight_dutycycle             = 150;                // Variable controlling the duty cycle of the PWM.
bool cot_light_toggle               = LOW;                // Light toggle status read from CoT
int cot_light_dimming               = 100;                // Light dimming status read from CoT

// TEMPERATURE: Variables for temperature control
bool heating_state                  = LOW;
bool long_absence                   = LOW;                // Variable active
int  temp_outdoor                   = 10;                 // Temperature read from forecast
int  temperature_profile            = 0;                  // States of temperature - 0 = Home, 1 = Away, 2 = Night, 3 = Long absence
int  temperature_set;                                     // Variable containing the new temperature to be set for the curret temperature profile
int temperatures[] = {23, 17, 13, 8};                     // Preset temperatures [Home, Away, Night, Long absence]
int cot_temp_home                   = temperatures[0];    // Set temperature read from CoT - Home
int cot_temp_away                   = temperatures[1];    // Set temperature read from CoT - Away
int cot_temp_night                  = temperatures[2];    // Set temperature read from CoT - Night
int cot_temp_long_abs               = temperatures[3];    // Set temperature read from CoT - Long absence
int temperature_read_raw            = 0;                  // Read room temperature raw value
int temperature_read_c              = 0;                  // Read room temperature in degrees
int temperature_read_interval       = 10 * SECOND;        // Set interval for reading room temperature to 10 sec
unsigned long temperature_read_ts   = 0;                  // Timestamp for last time room temp was read
int temperature_weekplan[7][24]     = {                   // Two dimensional array defining the week plan
                                       // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23     // HO = 1 = Home   ||   NI = 2 = Night   ||   AW = 3 = Away
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, AW, AW, AW, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, NI, NI, NI},   // Monday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, AW, AW, AW, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, NI, NI, NI},   // Tuesday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, AW, AW, AW, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, NI, NI, NI},   // Wednesday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, AW, AW, AW, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, NI, NI, NI},   // Thursday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, AW, AW, AW, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, NI, NI, NI},   // Friday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, HO, HO, HO, HO, AW, AW, AW, AW, AW, HO, HO, HO, HO, HO, HO, NI},   // Saturday
                                      {  NI, NI, NI, NI, NI, NI, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, HO, NI}    // Sunday
};


// FAN: Variables for fan control
bool fan_override_auto              = LOW;                // Overriding the automatic program for fan control
int fan_power_percent               = 30;                 // Percentage value giving the power of the fan in percent.
int cot_fan_power                   = 100;                // Set percentage value read from CoT - fan power

// AIRING: Variables for airing control
bool airing_override_auto           = LOW;                // Override automatic control of window
int airing_opening                  = 0;                  // Window opening in degrees
int cot_airing_opening              = 0;                  // Set percentage value read from CoT - airing window opening (degrees)

// NAVIGATION: Variables for menu navigation
unsigned long buttonpress_ts;
bool  val_adjust                    = LOW;                // Var selecting if the view to be shown is a menu or someting else
bool  temp_adjust                   = LOW;                // Var selecting if the view to be shown is for temperature modification
int   adjusted_value;                                     // Containing the variable currently being altered
char* adjusted_value_name;                                // Name of the variable currently being altered (for visualisation on screen)
int lvl1_val                        = 1;
int lvl2_val                        = 1;
int lvl3_val                        = 1;
int lvl4_val                        = 1;
int lvl5_val                        = 1;
int lvl6_val                        = 1;
bool deep_end_reached               = LOW;                // Variable giving feedback if deepest level of menu is reached
bool modify_val_percent             = LOW;                // Variable giving feedback if the value to be adjusted is a percentage value (only 0-100), (interval 5)
std::vector<std::string> current_vector{};
std::vector<int>prev_menu_adress{1,1,1,1,1,1,1};


// SCREENSAVER
bool  screensaver                   = LOW;                // Giving signal if user has been inactive and screensaver must be applied
bool  screensaver_prev              = LOW;                // For compariosn next round anf pos edge detect
int   screensaver_activate_time     = 20 * SECOND;        // If inactive for more than 20 seconds, activate screensaver
int   screensaver_exe_number        = 0;                  // Screensaver execution number. Decides which time-consuming function should be executed the current scan (typically CoT action)
volatile bool screensaver_interrupt = LOW;


// CONSUMPTION
int consumption_package_MQTT[26];


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
    temperature_read_raw            = analogRead(pin_temp_sensor);

    // Convert to millivolts
    temperature_read_raw            = temperature_read_raw * OUTPUT_MILLIVOLT / 1024; //10 bit resoultion

    // Convert to C
    temperature_read_c              = (temperature_read_raw - 500) / 10 +6; //https://learn.adafruit.com/tmp36-temperature-sensor

    // Timestamp for when temp was read last
    temperature_read_ts             = millis();
  }
  
  // NAVIGATION: Read navigation pushbuttons
  up.setState(digitalRead(pin_up));
  down.setState(digitalRead(pin_dwn));
  left.setState(digitalRead(pin_lft));
  right.setState(digitalRead(pin_rgt));

  // SCREENSAVER: Set timestamp for last time a button was pressed
  if (up.posEdge() || down.posEdge() || left.posEdge() || right.posEdge()){
    buttonpress_ts = millis();                            // Timestamp for last buttonpress
  }

  // SCREENSAVER: Activation timer
  if (millis() >= (buttonpress_ts + screensaver_activate_time))   {screensaver = HIGH;}
  else                                                            {screensaver = LOW;}

  // SCREENSAVER: 
  if (screensaver){
    
    attachInterrupt(pin_lft, ISR, RISING);                // Attach interrupts to pushbuttons to be able to "wake up"
    attachInterrupt(pin_rgt, ISR, RISING);
    attachInterrupt(pin_up,  ISR, RISING);
    attachInterrupt(pin_dwn, ISR, RISING);

    if (!screensaver_prev){                               // Clear screen if screensaver has just been activated
      tft_main.fillScreen(TFT_BLACK);}                    
    
    display_screensaver(temperature_read_c, "", time_ss_prev);  // Display screensaver
    time_ss_prev = second();                              // Set comparison time for next cycle
  }
  else if (!screensaver && screensaver_prev){             // Clear screen if screensaver has just been deactivated
    tft_main.fillScreen(TFT_BLACK);
  }

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

      tft_main.fillScreen(TFT_BLACK);                     // Clear screen
      display_setup_messages( "Connecting to",            // Feedback to user
                              "MQTT", 
                              "Please wait...");
    }

    delay(READTIME);                                      // Allow user to read screen

    mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);
    
    
    if (!screensaver){
      tft_main.fillScreen(TFT_BLACK);                     // Clear screen
      display_setup_messages( "Connecting to",            // Feedback to user
                              "MQTT", 
                              "Connected!");
      delay(READTIME);                                    // Allow user to read screen
      tft_main.fillScreen(TFT_BLACK);                     
    }
    else{
      delay(READTIME);
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
    
    // MQTT: loop
    mqtt.keepalive();
    
    if (cot_operational){
      
      // Since the CoT communication is incredibly slow, we will only execute one CoT operation per cycle to avoid stalling the rest of the program longer than necessarry
      
      // Reset counter if end if line is reached
      if (screensaver_exe_number > 18){
        screensaver_exe_number = 0;                       // Reset counter
      }

      switch (screensaver_exe_number) {                   // Select which background operation to execute
        
        // TIME: update system time
        case 0:  timeClient.update();
                  setTime(timeClient.getHours(),          // Set new system time
                          timeClient.getMinutes(), 
                          timeClient.getSeconds(), 
                          timeClient.getFormattedDate().substring(8,10).toInt(),    // DAY
                          timeClient.getFormattedDate().substring(5,7).toInt(),     // MONTH
                          timeClient.getFormattedDate().substring(0,4).toInt());    // YEAR
                  break;
        
        // CoT: read all values
        case 1:   cot_light_toggle    = CoT.read(KEY_LIGHT_TOGGLE,  TOKEN); break;            // Read   Light state
        case 2:   CoT.write(KEY_LIGHT_TOGGLE,   roomlight_state,    TOKEN); break;            // Write  Light state
        
        case 3:   cot_light_dimming   = CoT.read(KEY_LIGHT_DIMMING, TOKEN); break;            // Read   Dimming
        case 4:   CoT.write(KEY_LIGHT_DIMMING,  roomlight_intensity_percentage, TOKEN); break;// Write  Dimming
        
        case 5:   cot_temp_home       = CoT.read(KEY_TEMP_HOME,     TOKEN); break;            // Read   Home temp set
        case 6:   CoT.write(KEY_TEMP_HOME,      temperatures[0],    TOKEN); break;            // Write  Home temp set

        case 7:   cot_temp_away       = CoT.read(KEY_TEMP_AWAY,     TOKEN); break;            // Read   Away temp set
        case 8:   CoT.write(KEY_TEMP_AWAY,      temperatures[1],    TOKEN); break;            // Write  Away temp set

        case 9:   cot_temp_night      = CoT.read(KEY_TEMP_NIGHT,    TOKEN); break;            // Read   Night temp set
        case 10:  CoT.write(KEY_TEMP_NIGHT,     temperatures[2],    TOKEN); break;            // Write  Night temp set

        case 11:  cot_temp_long_abs   = CoT.read(KEY_TEMP_LONG_ABS, TOKEN); break;            // Read   Long absence temp set
        case 12:  CoT.write(KEY_TEMP_LONG_ABS,  temperatures[3],    TOKEN); break;            // Write  Long absence temp set

        case 13:  cot_fan_power       = CoT.read(KEY_FAN_POWER,     TOKEN); break;            // Read   Fan power
        case 14:  CoT.write(KEY_FAN_POWER,      fan_power_percent,  TOKEN); break;            // Write  Fan power

        case 15:  cot_airing_opening  = CoT.read(KEY_AIRING_OPENING,TOKEN); break;            // Read   Window opening
        case 16:  CoT.write(KEY_AIRING_OPENING, airing_opening,     TOKEN); break;            // Write  Window opening
        
        case 17:  CoT.write(KEY_TEMP_MEASURED,  temperature_read_c, TOKEN); break;            // Write  Indoor temperature

        case 18:  Room id = static_cast<Room>(id_room);                                       // Write consumption data to RPi

                  // Read todays consumption
                  for (int i = 0; i < 26; i++){
                    consumption_package_MQTT[i] = roomControl.returnTodaysConsumption()[i]; 
                  };

                  String buffer_today;

                  // Fill buffer with consumption data in string format
                  for (int i = 1; i < 26; i++){
                    buffer_today += String(consumption_package_MQTT[i]);
                    if (i < 25){
                      buffer_today += ",";}
                  }

                  // Read yesterdays consumption
                  for (int i = 0; i < 26; i++){
                    consumption_package_MQTT[i] = roomControl.returnYesterdaysConsumption()[i]; 
                  };

                  String buffer_yesterday;

                  // Fill buffer with consumption data in string format
                  for (int i = 1; i < 26; i++){
                    buffer_yesterday += String(consumption_package_MQTT[i]);
                    if (i < 25){
                      buffer_yesterday += ",";}
                  }
                  // Create JSON to send via MQTT to RPi
                  mqtt_message.resiver = "Hub";
                  mqtt_message.header = Energi_consumption;
                  mqtt_message.room = id;
                  mqtt_message.data_int[0] = {"user", id +5}
                  mqtt_message.data_String[0] = { "todaysCons", buffer_today};
                  mqtt_message.data_String[1] = { "yesterdaysCons", buffer_yesterday};
                  
                  // Send data
                  mqtt.pub(mqtt_message);
      }
      screensaver_exe_number ++;                          // Incrememt item number each cycle
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
    if (right.posEdge()){                                 // Increase menu level
        if (!deep_end_reached){
            menu_lvl ++;
            tft_main.fillScreen(TFT_BLACK);
            val_adjust = LOW;                             // To solve bug of duplicated menu lines being written after having altered a value
        }
    }
    else if (left.posEdge()){                             // Decrease menu level
        if (menu_lvl > 1){                                // Prevent from going to lower edit level than allowed
            menu_lvl --;
            tft_main.fillScreen(TFT_BLACK);
            val_adjust = LOW;                             // To solve bug of duplicated menu lines being written after having altered a value 
        }
    }


    // Decide which level in menu is active and which adressing value to modify
    if (!val_adjust){
      if      (menu_lvl == 1)  {lvl1_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl1_val);
                                current_lvl_val = lvl1_val;
                                lvl2_val = 1;                                                                               }   // 1st level - Main menu
      else if (menu_lvl == 2)  {lvl2_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl2_val);
                                current_lvl_val = lvl2_val;
                                lvl3_val = 1;                                                                               }   // 2nd level
      else if (menu_lvl == 3)  {lvl3_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl3_val);
                                current_lvl_val = lvl3_val;
                                lvl4_val = 1;                                                                               }   // 3rd level
      else if (menu_lvl == 4)  {lvl4_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl4_val);
                                current_lvl_val = lvl4_val;
                                lvl5_val = 1;                                                                               }   // 4th level
      else if (menu_lvl == 5)  {lvl5_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl5_val);
                                current_lvl_val = lvl5_val;
                                lvl6_val = 1;                                                                               }   // 5th level
      else if (menu_lvl == 6)  {lvl6_val = mod_val(bottom_reached, modify_val_percent, up.posEdge(), down.posEdge(), lvl6_val);
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
                                           roomlight_intensity_percentage = mod_val(LOW, HIGH, up.posEdge(), down.posEdge(), roomlight_intensity_percentage);
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
          if      (menu_adress[2] == 1)   { fan_override_auto = !fan_override_auto;           // 3.4.1 - Override auto toggle
                                            toggle_popup(fan_override_auto, "Override auto-fan");
                                            menu_lvl = 2;
                                            menu_adress[0] = 2;
                                            current_lvl_val = 1;}                             // 3.3.1 - Override auto time view
          else if (menu_adress[2] == 2)   { fan_power_percent = mod_val(LOW, HIGH, up.posEdge(), down.posEdge(), fan_power_percent); // 3.3.2 - Fan power window
                                            adjusted_value = fan_power_percent;  
                                            adjusted_value_name = "Fan power";
                                            val_adjust = HIGH;
                                            deep_end_reached = HIGH;}                         // Prevent from entering deeper level in menu
      }

      else if     (menu_adress[1] == 4)   {                                                   // 3.4   - AIRING
          if      (menu_adress[2] == 1)   { airing_override_auto = !airing_override_auto;     // 3.4.1 - Override auto toggle
                                            toggle_popup(airing_override_auto, "Override auto-vent");
                                            menu_lvl = 2;
                                            menu_adress[0] = 2;
                                            current_lvl_val = 1;}          
          else if (menu_adress[2] == 2)   {                                                   // 3.4.2 - Opening percent
                                            airing_opening = mod_window(up.posEdge(), down.posEdge(), airing_opening); 
                                            adjusted_value = airing_opening;  
                                            adjusted_value_name = "Window opening";
                                            val_adjust = HIGH;
                                            deep_end_reached = HIGH;}
      }                     
  }

  // Menu level 4
  else if             (menu_adress[0] == 4)   {                                                  // 4
      if              (menu_adress[1] == 2)   {                                                  // 4.2       - TEMPERATURE
          if          (menu_adress[2] == 1)   {                                                  // 4.2.1     - Weekly plan menu 
                                                temperature_profile = menu_adress[3] - 1;
                                                temperature_set = temperatures[temperature_profile];
                                                temperature_set = mod_temp(up.posEdge(), down.posEdge(), temperature_set);
                                                temp_adjust = HIGH;
                                                deep_end_reached = HIGH;}

          else if     (menu_adress[2] == 2)   { current_vector = hour_cathegories;}                // 4.2.2    - Daily plan                    
              
      }
  } 

  // Menu level 5
  else if             (menu_adress[0] == 5)   {                                                  // 5
      if              (menu_adress[1] == 2)   {                                                  // 5.2       - TEMPERATURE
          if          (menu_adress[2] == 2)   { current_vector = temperature_set_cathegories;    // 5.2.2     - Daily plan
          }      
      }
  }

  // Menu level 6
  else if             (menu_adress[0] == 6)   {                                                  // 6
      if              (menu_adress[1] == 2)   {                                                  // 6.2       - TEMPERATURE
          if          (menu_adress[2] == 2)   {                                                  // 6.2.2     - Daily plan
                                                temperature_weekplan[(menu_adress[3]-1)][(menu_adress[4]-1)] = (menu_adress[5]); 
                                                menu_lvl = 5;
                                                menu_adress[0] = 5;
                                                current_lvl_val = 6;
          }        
      }
  }  
  
  // Menu lvl

  // MENU: Display the menu decided by the menu adressing

  if (menu_adress != prev_menu_adress){                   // Refresh screen if menu adress has been changed (to avoid duplicated menu items) 
    tft_main.fillScreen(TFT_BLACK);}

  if (val_adjust && !screensaver){                        // If the current task requires adjusting a value, the value adjustment window should be displayed
    val_adjust_window(adjusted_value, adjusted_value_name);}

  else if (temp_adjust && !screensaver){
    temperatures[temperature_profile] = temperature_set;
    temp_adjust_window(temperatures[temperature_profile], "Temperature");
  }
  
  // WEEKPLAN
  else if(  (menu_adress[0]==5)                            // 5 (6 sybolises that value has been chosen)
          &&(menu_adress[1]==2)                            // 5.2
          &&(menu_adress[2]==2)                            // 5.2.2  - Daily plan
          &&(!screensaver)){
    display_weekplan_setting(menu_adress[menu_adress[0]], temperature_weekplan[(menu_adress[3]-1)][(menu_adress[4]-1)], current_vector);
  }

  else if(!screensaver){                                  // If no "special" window needs to be displayed, display menu with correct cathegories
    display_menu(current_lvl_val, current_vector);
  }

  // MENU: Check if last item in menu is reached. If so, it is not possible to scroll further down
  if (menu_adress[menu_adress[0]] == current_vector.size()){ // Was menu_lvl in stead of menu_adress[0]
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
  if ((roomlight_state) && (roomlight_dutycycle > 10)){   // Toggle ON (set 10 as lowest value to avoid glowing bulb)
    ledcWrite(CHANNEL_LED, roomlight_dutycycle);}
  else{                                                   // Toggle OFF
    ledcWrite(CHANNEL_LED, 0);}

  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                                   ROOM CONTROL                                  //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////

  // TEMPERATURE
  // Get correct temp from 1-sensor, 2-MQTT, 3-weekly plan
  
  if (long_absence){
    roomControl.setLongAbsence();                         // Start long absence program
    roomControl.updateTemperatures(temperature_read_c, temp_outdoor, temperatures[3]);}
  else {
    roomControl.resetLongAbsence();                       // Stop long absence program
    roomControl.updateTemperatures(temperature_read_c, temp_outdoor, temperatures[temperature_weekplan[roomControl.getWeekday()][hour()] - 1]);
  }

  // ROOM CONTROL: Update controls
  // Update temp values from CoT. If they have changed from last cycle, roomControl will set the new temperature
  roomControl.updateCurrentTemperatureList(temperatures[0], temperatures[1], temperatures[2], temperatures[3]);
  roomControl.updateTemperaturesFromCoT(cot_temp_home, cot_temp_away, cot_temp_night, cot_temp_long_abs);
  
  roomControl.updateLightParameters(roomlight_state, roomlight_intensity_percentage);
  roomControl.updateLightParametersFromCoT(cot_light_toggle, cot_light_dimming);

  roomControl.updateFanPower(fan_power_percent);
  roomControl.updateFanPowerFromCoT(cot_fan_power);

  roomControl.updateWindowOpening(airing_opening);
  roomControl.updateWindowOpeningFromCoT(cot_airing_opening);
  


  roomControl.update();



  temperatures[0]   = roomControl.getTemperatureHome();
  temperatures[1]   = roomControl.getTemperatureAway();
  temperatures[2]   = roomControl.getTemperatureNight();
  temperatures[3]   = roomControl.getTemperatureLongAbs();

  fan_power_percent = roomControl.getFanPower();

  airing_opening    = roomControl.getWindowOpening();

  roomlight_state   = roomControl.getLightStatus();
  roomlight_intensity_percentage = roomControl.getLightDimming();
  
  // TEMPERATURE: Control heating
  digitalWrite(pin_heating_LED, roomControl.autoHeating());
  
  // FAN: Auto / manual
  if (fan_override_auto)    {ledcWrite(CHANNEL_FAN, fan_power_percent * 255 /100);}           // Manual control
  else                      {ledcWrite(CHANNEL_FAN, roomControl.fanPowerAuto()* 255 /100);}   // Automatic control

  // AIRING: Auto / manual
  if (airing_override_auto) {servo.write(airing_opening);}                                    // Manual control
  else                      {servo.write(roomControl.windowDegreesAuto());}                   // Automatic control

  
  /////////////////////////////////////////////////////////////////////////////////////
  //                                                                                 //
  //                             GENERAL END-OF-LOOP TASKS                           //
  //                                                                                 //
  /////////////////////////////////////////////////////////////////////////////////////


  // Set current menu adress as previous menu adress for comparison next scan
  prev_menu_adress = menu_adress;

  // TIME
  millis_prev = millis();
  
  // SCREENSAVER
  screensaver_prev = screensaver;

  // INTERRUPT
  if (screensaver_interrupt){
    screensaver_interrupt = LOW;
    buttonpress_ts        = millis();

    // When no longer in screensaver mode, buttons resume to normal operation
    detachInterrupt(pin_up);
    detachInterrupt(pin_dwn);
    detachInterrupt(pin_lft);
    detachInterrupt(pin_rgt);
  }
}

void IRAM_ATTR ISR(){                                     // ISR function for "waking" from screensaver mode.
  screensaver_interrupt   = HIGH;
}

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
            temp_outdoor = int(Json_payload["data_int"]["Outdoor_temp"]); 
        }
    }
}


#ifdef DEBUG
 
#endif
  




