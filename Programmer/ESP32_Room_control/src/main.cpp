/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	14/04/2021

Description: 
	Room controller
*************************************************/


#include "Controller_config.h"
#include "OLED.h"
#include "Peripherals.h"

// Instanciate TFT_eSPI library for main function. If not instanciated, it's not possible to customize text size, text colour etc.
TFT_eSPI tft_main = TFT_eSPI();

void setup() {

  // Set up hardware for room controller
  hw_setup();           // Setup hardware for room controller
  
  // Set up initial parameters for SPI control of OLED
  tft_main.init();
  tft_main.setRotation(1);
  tft_main.fillScreen(TFT_BLACK);
  tft_main.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft_main.setTextSize(1);
  
  // Set up serial communication
  Serial.begin(115200); // Serial communication
}

// Variables for light control in the room
bool roomlight_state = LOW;                // Variable determining if the light in the room should be ON or OFF
int roomlight_intensity_percentage = 100;  // Variable giving possibility to adjust the light intensity in the room downwards. Value between -128 and 127
int roomlight_dutycycle = 150;             // Variable controlling the duty cycle of the PWM.
int roomlight_intensity_measured = 0;      // Measured luminosity in the room
int roomlight_setpoint_intensity = 240;    // Set fairly bright example value. can be calbrated later by user
int roomlight_intensity_requested_dimmed;  // The requested roomlight value after dimming is applied
int daylight_compensation_value = 255;     // To be replaced by the int8 value given from the daylight compensation function

// Variables for menu navigation
bool val_adjust = LOW;                     // Var selecting if the view to be shown is a menu or someting else
int adjusted_value;                        // Containing the variable currently being altered
char* adjusted_value_name;                 // Name of the variable currently being altered (for visualisation on screen)

// Declare variables
bool buttonstate_up, buttonstate_dwn, buttonstate_lft, buttonstate_rgt;   // Declare values representing button states
int targetTime;

// Declare and set initial button states 
bool prev_buttonstate_up = LOW;
bool prev_buttonstate_dwn = LOW;
bool prev_buttonstate_lft = LOW;
bool prev_buttonstate_rgt = LOW;

bool pos_edge_up = LOW;
bool pos_edge_dwn = LOW;
bool pos_edge_lft = LOW;
bool pos_edge_rgt = LOW;

// Variables for menu navigation
int menu_lvl = 0;
int lvl1_val = 0;
int lvl2_val = 0;
int lvl3_val = 0;
int lvl4_val = 0;
int lvl5_val = 0;
int lvl6_val = 0;

int current_lvl_val = 1;            // Value for "display_menu" function, so it knows which element is active

bool deep_end_reached = LOW;        // Variable giving feedback if deepest level of menu is reached
bool modify_val_percent = LOW;      // Variable giving feedback if the value to be adjusted is a percentage value (only 0-100), (interval 5)
bool bottom_reached = LOW;          // VAriable giving feedback if bottom button in menu is reached

std::vector<std::string> current_vector{};
std::vector<int>prev_menu_adress{1,1,1,1,1,1,1};

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//                                    MENU ITEMS                                   //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Main menu (menu_lvl == 1)
std::vector<std::string> main_cathegories{"Light", "Temperature", "Fan", "Airing"};

  // Light menu (menu_lvl==2, lvl1 == 11,edit)
  std::vector<std::string> light_cathegories{"Toggle", "Dim adjust", "Set intensity"};

    // Toggle (1.1.1)
    // Toggle if selected and right push
    
    // Dim adjust (1.2)
    // ---> val_adjust_window

  // Temperature menu (2.0)
  std::vector<std::string> temperature_cathegories{"Weekly plan", "Temp adjust.", "Long absence"};

    // Weekly plan menu (2.1)
    std::vector<std::string> weekplan_cathegories{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    // Temp adjustment (2.2)
    // ---> val_adjust_window

  // Fan menu (3)
  std::vector<std::string> fan_cathegories{"Override auto.", "Fan power"};

  // Airing menu (4)
  std::vector<std::string> airing_cathegories{"Override auto.", "Opening (%) man"};

// Menu end


void loop() {

  // Read navigation pushbuttons
  buttonstate_up = digitalRead(pin_up);
  buttonstate_dwn = digitalRead(pin_dwn);
  buttonstate_lft = digitalRead(pin_lft);
  buttonstate_rgt = digitalRead(pin_rgt);

  // Read measured light intensity in room and convert from 10 to 8 bit
  analogReadResolution(10);
  roomlight_intensity_measured = analogRead(pin_light_sensor);
  roomlight_intensity_measured = roomlight_intensity_measured/4; // Need to use 10 bit reading because HW does not handle lower resolution. We then devide by 4 to get a 8 bit value


  // Pushutton edge detection
  if ((buttonstate_up == HIGH) && (prev_buttonstate_up == LOW)){pos_edge_up = HIGH;}      // Pos edge UP
  else {pos_edge_up = LOW;}
  if ((buttonstate_dwn == HIGH) && (prev_buttonstate_dwn == LOW)){pos_edge_dwn = HIGH;}   // Pos edge DOWN
  else {pos_edge_dwn = LOW;}
  if ((buttonstate_lft == HIGH) && (prev_buttonstate_lft == LOW)){pos_edge_lft = HIGH;}   // Pos edge LEFT
  else {pos_edge_lft = LOW;}
  if ((buttonstate_rgt == HIGH) && (prev_buttonstate_rgt == LOW)){pos_edge_rgt = HIGH;}   // Pos edge RIGHT
  else {pos_edge_rgt = LOW;}
  

  // Change menu level
  if (pos_edge_rgt){              // Increase menu level
      if (!deep_end_reached){
          menu_lvl ++;
          tft_main.fillScreen(TFT_BLACK);
          val_adjust = LOW;       // To solve bug of duplicated menu lines being written after having altered a value
      }
  }
  else if (pos_edge_lft){         // Decrease menu level
      if (menu_lvl > 1){          // Prevent from going to lower edit level than allowed
          menu_lvl --;
          tft_main.fillScreen(TFT_BLACK);
          val_adjust = LOW;       // To solve bug of duplicated menu lines being written after having altered a value 
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
  

  // Create adress for deciding which display should be drawn
  std::vector<int>menu_adress{menu_lvl, lvl1_val, lvl2_val, lvl3_val, lvl4_val, lvl5_val, lvl6_val};
  
  deep_end_reached = LOW;         // Always reset deep end reached variable, to run new check each scan
  modify_val_percent = LOW;
  bottom_reached = LOW;

  val_adjust = LOW;

  // Adress format: [Menu level, lvl1 value, lvl2 value....]
  // Menu level 1
  if              (menu_adress[0] == 1)   {current_vector =      main_cathegories;           // 1             - Main menu
  }

  // Menu level 2
  else if         (menu_adress[0] == 2)   {                                                  // 2
      
      if          (menu_adress[1] == 1)   {current_vector =      light_cathegories;}         // 2.1           - Light menu
        if        (menu_adress[2] == 3)   {bottom_reached =      HIGH;}                      // 2.1.3, which is the last element in the menu
      else if     (menu_adress[1] == 2)   {current_vector =      temperature_cathegories;}   // 2.2           - Temperature menu
      else if     (menu_adress[1] == 3)   {current_vector =      fan_cathegories;}           // 2.3           - Fan menu
      else if     (menu_adress[1] == 4)   {current_vector =      airing_cathegories;}        // 2.4           - Airing menu
  }

  // Menu level 3
  else if         (menu_adress[0] == 3)   {                                                  // 3 
      
      if          (menu_adress[1] == 1)   {                                                  // 3.1   - LIGHT
          if      (menu_adress[2] == 1)   {roomlight_state = !roomlight_state;               // 3.1.1 - Toggle light
                                           toggle_popup(roomlight_state, "Light");
                                           menu_lvl = 2;
                                           menu_adress[0] = 2;
                                           current_lvl_val = 1;}                             // Must be added to avoid duplicating menu lines when returning} 
          else if (menu_adress[2] == 2)   {                                                  // 3.1.2 - Dim adjust view
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
    
      else if     (menu_adress[1] == 2)   {                                                  // 3.2   - TEMPERATURE
          if      (menu_adress[2] == 1)   {current_vector =       weekplan_cathegories;}     // 3.2.1 - Weekly plan menu
          else if (menu_adress[2] == 2)   {/* Temp adjust +/- view*/;}                       // 3.2.2 - Temp adjust window
          else if (menu_adress[2] == 3)   {/* Long absence view*/;}                          // 3.2.3 - Long absence view
      }

      else if     (menu_adress[1] == 3)   {                                                  // 3.3   - FAN
          if      (menu_adress[2] == 1)   {/* Override time 1hr, 2hr, 3hr view*/;}           // 3.3.1 - Override auto time view
          else if (menu_adress[2] == 2)   {/* Fan adjust +/- 5% view*/;}                     // 3.3.2 - Fan power window
      }

      else if     (menu_adress[1] == 4)   {                                                  // 3.4   - AIRING
          if      (menu_adress[2] == 1)   {/* Override time 1hr, 2hr, 3hr view*/;}           // 3.4.1 - Override auto time view
          else if (menu_adress[2] == 2)   {/* Airing adjust +/- 5% view*/;                   // 3.4.2 - Opening percent
                                              deep_end_reached = HIGH;}                     
      }
  }

  // Menu level 4
  else if             (menu_adress[0] == 4)   {                                                  // 4
      if              (menu_adress[1] == 2)   {                                                  // 4.2       - TEMPERATURE
          if          (menu_adress[2] == 1)   {                                                  // 4.2.1     - Weekly plan menu
              
              if      (menu_adress[3] == 1)   {/* display Day menu                               // 4.2.1.1   - Power plan - Monday
                                                  adjustable variable = monday*/;}
              else if (menu_adress[3] == 2)   {/* display Day menu                               // 4.2.1.2   - Power plan - Tuesday
                                                  adjustable variable = tuesday*/;}
              else if (menu_adress[3] == 3)   {/* display Day menu                               // 4.2.1.3   - Power plan - Wednesday
                                                  adjustable variable = wednesday*/;}
              else if (menu_adress[3] == 4)   {/* display Day menu                               // 4.2.1.4   - Power plan - Thursday
                                                  adjustable variable = thursday*/;}
              else if (menu_adress[3] == 5)   {/* display Day menu                               // 4.2.1.5   - Power plan - Friday
                                                  adjustable variable = friday*/;}
              else if (menu_adress[3] == 6)   {/* display Day menu                               // 4.2.1.6   - Power plan - Saturday
                                                  adjustable variable = saturday*/;}
              else if (menu_adress[3] == 7)   {/* display Day menu                               // 4.2.1.7   - Power plan - Sunday
                                                  adjustable variable = sunday*/;}
          }
      }
  } 
  
  // Display the menu decided by the menu adressing
  
  if (menu_adress != prev_menu_adress){
    tft_main.fillScreen(TFT_BLACK);
  }

  if (val_adjust){
    val_adjust_window(adjusted_value, adjusted_value_name);
  }

  else{
    display_menu(current_lvl_val, current_vector);
  }
  
  // Set button states to "old button state" for comparison next scan
  prev_buttonstate_up = buttonstate_up;
  prev_buttonstate_dwn = buttonstate_dwn;
  prev_buttonstate_lft = buttonstate_lft;
  prev_buttonstate_rgt = buttonstate_rgt;

  prev_menu_adress = menu_adress;

  // Convert light from percentage value to 8bit int value (0-255)
  
  roomlight_intensity_requested_dimmed = roomlight_setpoint_intensity * roomlight_intensity_percentage / 100;
  
  

  //////////////////// FUNCTION FOR REGULATING LIGHT BASED ON INPUT FROM SENSO

  // Automatic regulation of the light intensity (hysteresis of +/- 5)
  if ((roomlight_intensity_measured < (roomlight_intensity_requested_dimmed - 5)) && (roomlight_dutycycle < 255)){ 
    roomlight_dutycycle +=5;}

  else if ((roomlight_intensity_measured > (roomlight_intensity_requested_dimmed + 5)) && (roomlight_dutycycle > 0)){
    roomlight_dutycycle -=5;}


  
  
  
  if ((roomlight_state) && (roomlight_dutycycle > 6)){                       // Toggle ON
    ledcWrite(CH1, roomlight_dutycycle);
  }
  else{                                       // Toggle OFF
    ledcWrite(CH1, 0);
  }
  
  // Just checking how sensor reacts to LED
  
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

  delay(50);
  


}


