
#include "RoomControl.h"

/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	02/05/2021

Description: 
  Room controller class

Resp & dev: 
  Jorgen Andreas Mo

*************************************************/

/*
RoomControl::RoomControl(int room_n) : room_number{room_n}{

}
*/

// CONSUMPTION
// Resets the consumption data
void RoomControl::resetConsumption(){
    for (int i=0; i<26; i++){                   // Clear consumption array
        today_cons_table[i] = 0;
    };
}

// Gives consumption for the current day
int* RoomControl::returnTodaysConsumption(){    
    return today_cons_table;
}

// Gives consumption for the previous day
int* RoomControl::returnYesterdaysConsumption(){    
    return yesterday_cons_table;
}

// Setting the output power of the heating sorce (oven) (for power cons. calculation)
void RoomControl::setHeaterWattage(int watt){
    heaterWattage = watt;
}

// Setting the output power of the fan (for power cons. calculation)
void RoomControl::setFanWattage(int watt){
    fanWattage = watt;
}



// TEMPERATURE
// Set temp indoors, outdoors and requested
void RoomControl::updateTemperatures(int in, int out, int req){ // Set temp indoors, outdoors and requested
    temp_req    = req;
    temp_in     = in;
    temp_out    = out;
}

// Set temperature hysteresis (+/- x degrees)
void RoomControl::setTempHysteresis(int hyst){
    temp_hyst = hyst;
}

// Update RoomControl with the current temperature setting list
void RoomControl::updateCurrentTemperatureList(int h, int a, int n, int l_a){
    temp_home = h;
    temp_away = a;
    temp_night = n;
    temp_long_abs = l_a;
}

// Update RoomControl with the current temperature setting list from CoT
void RoomControl::updateTemperaturesFromCoT(int h, int a, int n, int l_a){
    
    // Only update if within valid range
    if((h > 4) && (h < 40)){
        temp_CoT_home = h;};

    if((a > 4) && (a < 40)){
        temp_CoT_away = a;};

    if((n > 4) && (n < 40)){
        temp_CoT_night = n;};

    if((l_a > 4) && (l_a < 40)){
        temp_CoT_long_abs = l_a;};
}

// Returns the updated Home temperature based on old values and CoT
int RoomControl::getTemperatureHome(){
    return temp_home;
}

// Returns the updated Away temperature based on old values and CoT
int RoomControl::getTemperatureAway(){
    return temp_away;
}

// Returns the updated Night temperature based on old values and CoT
int RoomControl::getTemperatureNight(){
    return temp_night;
}

// Returns the updated Logn absence temperature based on old values and CoT
int RoomControl::getTemperatureLongAbs(){
    return temp_long_abs;
}

// Automatic control of heater
bool RoomControl::autoHeating(){
    return heating_state;
}



// FAN
// Update RoomControl with the current Fan power
void RoomControl::updateFanPower(int fanPower){
    fan_power = fanPower;
}

// Update RoomControl with the current Fan power from CoT
void RoomControl::updateFanPowerFromCoT(int fanPower){
    fan_power_CoT = fanPower;
}

// Returns the updated fan power value based on old values and CoT
int RoomControl::getFanPower(){
    return fan_power;
}

// Automatic control of fan power
int RoomControl::fanPowerAuto(){
    return fan_power_auto;
}



// LIGHT
// Update RoomControl with the current light parameters
void RoomControl::updateLightParameters(bool state, int dim_val){
    light_state = state;
    dimming_value = dim_val;
}

// Update RoomControl with the current light parameters from CoT
void RoomControl::updateLightParametersFromCoT(bool state, int dim_val){
    light_state_CoT = state;
    dimming_value_CoT = dim_val;
}

// Returns the updated light status based on old values and CoT
bool RoomControl::getLightStatus(){
    return light_state;
}

// Returns the updated light dimming value based on old values and CoT
int RoomControl::getLightDimming(){
    return dimming_value;
}



// AIRING
// Update RoomControl with the current window opening
void RoomControl::updateWindowOpening(int windowOpening){
    window_degrees = windowOpening;
}

// Update RoomControl with the current window opening from CoT
void RoomControl::updateWindowOpeningFromCoT(int windowOpening){
    window_degrees_CoT = windowOpening;
}

// Returns the updated window opening based on old values and CoT
int RoomControl::getWindowOpening(){
    return window_degrees;
}

// Automatic control of window opening
int RoomControl::windowDegreesAuto(){
    return window_degrees_auto;    
}



// GENERAL
// Must be called each cycle to ensure that system updated heating control, fan control, window control etc  
void RoomControl::update(){
    // Output control
    if (temp_in > temp_out - temp_hyst){
        
        // Too hot indoors
        if      (temp_in >= (temp_req + temp_hyst + 6)){    // If hotter indoors than hysteresis + 6
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 60;                       // Window   - 60°       - Cooling effect
                    fan_power_auto      = 100;}                     // Fan      - 100 %     - Air circulation / cooling
        else if (temp_in >= (temp_req + temp_hyst + 4)){    // If hotter indoors than hysteresis + 4
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 40;                       // Window   - 40°       - Cooling effect
                    fan_power_auto      = 60;}                      // Fan      - 60 %      - Air circulation / cooling
        else if (temp_in >= (temp_req + temp_hyst + 2)){    // If hotter indoors than hysteresis + 2
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 20;                       // Window   - 20°       - Cooling effect
                    fan_power_auto      = 40;}                      // Fan      - 40 %      - Air circulation
        else if (temp_in >= (temp_req + temp_hyst)) {       // If hotter indoors than hysteresis
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 0;                        // Window   - 0°        - Prevent heat letting out
                    fan_power_auto      = 30;}                      // Fan      - 30 %      - Air circulation
        
        // Too cold indoors
        else if (temp_in <= (temp_req - temp_hyst)) {       // If colder indoors than hysteresis
                    heating_state       = HIGH;                     // Heating  - ON        - Heating through electricity
                    window_degrees_auto = 0;                        // Window   - 0°        - Prevent heat letting out
                    fan_power_auto      = 30;}                      // Fan      - 30 %      - Air circulation
    }
    
    // Warmer outside than inside --> no use in opening window to cool down room 
    else if (temp_out > temp_in + temp_hyst){           // Warmer outdoors than indoors
        // Too hot indoors
        if      (temp_in >= (temp_req + temp_hyst + 6)){    // If hotter indoors than hysteresis + 6
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 10;                       // Window   - 10°       - Air circulation
                    fan_power_auto      = 90;}                      // Fan      - 90%       - Cooling effect
        else if (temp_in >= (temp_req + temp_hyst + 4)){    // If hotter indoors than hysteresis + 4
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 10;                       // Window   - 10°       - Air circulation
                    fan_power_auto      = 60;}                      // Fan      - 60 %      - Cooling effect
        else if (temp_in >= (temp_req + temp_hyst + 2)){    // If hotter indoors than hysteresis + 2
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 10;                       // Window   - 10°       - Air circulation
                    fan_power_auto      = 50;}                      // Fan      - 50 %      - Cooling effect
        else if (temp_in >= (temp_req + temp_hyst)) {       // If hotter indoors than hysteresis
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 10;                       // Window   - 10°       - Air circulation
                    fan_power_auto      = 30;}                      // Fan      - 30 %      - Cooling effect
        
        // Too cold indoors
        else if (temp_in <= (temp_req - temp_hyst - 4)) {   // If colder indoors than hysteresis - 4
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 60;                       // Window   - 60°       - Heating from outdoor
                    fan_power_auto      = 50;}                      // Fan      - 50 %      - Air circulation
        else if (temp_in <= (temp_req - temp_hyst - 2)) {   // If colder indoors than hysteresis - 2
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 50;                       // Window   - 50°       - Heating from outdoor
                    fan_power_auto      = 50;}                      // Fan      - 50 %      - Air circulation
        else if (temp_in <= (temp_req - temp_hyst)) {       // If colder indoors than hysteresis
                    heating_state       = LOW;                      // Heating  - OFF
                    window_degrees_auto = 40;                       // Window   - 40°       - Heating from outdoor
                    fan_power_auto      = 30;}                      // Fan      - 30 %      - Air circulation
    }

    // Long absence - Windows should not open and fan should be off for fire safety purposes
    if (long_abs){
        window_degrees_auto             = 0;
        fan_power_auto                  = 0;
    }


    // Decide if update values from CoT
    
    // CoT value has been updated
    if (temp_CoT_home != temp_CoT_home_prev){
        temp_home = temp_CoT_home;                              // Update temperature
        temp_CoT_home_prev = temp_CoT_home;                     // Reset comp value
    }
    if (temp_CoT_away != temp_CoT_away_prev){
        temp_away = temp_CoT_away;                              // Update temperature
        temp_CoT_away_prev = temp_CoT_away;                     // Reset comp value
    }
    if (temp_CoT_night != temp_CoT_night_prev){
        temp_night = temp_CoT_night;                            // Update temperature
        temp_CoT_night_prev = temp_CoT_night;                   // Reset comp value
    }
    if (temp_CoT_long_abs != temp_CoT_long_abs_prev){
        temp_long_abs = temp_CoT_long_abs;                      // Update temperature
        temp_CoT_long_abs_prev = temp_CoT_long_abs;             // Reset comp value
    }

    // Fan power
    if (fan_power_CoT != fan_power_CoT_prev){
        fan_power = fan_power_CoT;                              // Update fan power
        fan_power_CoT_prev = fan_power_CoT;                     // Reset comp value
    }
    // Airing
    if (window_degrees_CoT != window_degrees_CoT_prev){
        window_degrees = window_degrees_CoT;                    // Update window opening
        window_degrees_CoT_prev = window_degrees_CoT;           // Reset comp value
    }

    // Light
    if (light_state_CoT != light_state_CoT_prev){
        light_state = light_state_CoT;                          // Update window opening
        light_state_CoT_prev = light_state_CoT;                 // Reset comp value
    }

    if (dimming_value_CoT != dimming_value_CoT_prev){
        dimming_value = dimming_value_CoT;                      // Update window opening
        dimming_value_CoT_prev = dimming_value_CoT;             // Reset comp value
    }


    if (heating_state && !heating_state_prev){                  // Pos edge
        heating_pos_edge_ts = millis();
    }

    else if (!heating_state && heating_state_prev){             // Neg edge
        heating_neg_edge_ts = millis();

        heating_on_period = (heating_neg_edge_ts - heating_pos_edge_ts) / 1000;
        heating_on_period_consumption = (heating_on_period * heaterWattage) / 3600.0; // Calculate Wms and then Wh
        
        today_cons_table[0] = room_number;
        today_cons_table[1] = day();                            // Define current day in consumption list
        today_cons_table[hour()+2] += heating_on_period_consumption;
        
    }

    else if ((prev_hour != hour()) && heating_state){           // Update consumption at the end of every hour
        
        if(day() != prev_day){
            heating_neg_edge_ts = millis();

            heating_on_period = (heating_neg_edge_ts - heating_pos_edge_ts) / 1000;
            heating_on_period_consumption = (heating_on_period * heaterWattage) / 3600.0; // Calculate Wms and then Wh
        
            today_cons_table[0] = room_number;
            today_cons_table[1] = prev_day;                            // Define current day in consumption list
            today_cons_table[prev_hour+2] += heating_on_period_consumption;
        }

        else{

            heating_neg_edge_ts = millis();

            heating_on_period = (heating_neg_edge_ts - heating_pos_edge_ts) / 1000;
            heating_on_period_consumption = (heating_on_period * heaterWattage) / 3600.0; // Calculate Wms and then Wh
        
            today_cons_table[0] = room_number;
            today_cons_table[1] = day();                            // Define current day in consumption list
            today_cons_table[prev_hour+2] += heating_on_period_consumption;
        }

        heating_pos_edge_ts = millis();
    }

    // CONSUMPTION
    // Check for new date and reset consumption if new date
    if (day() != prev_day){
        for (int i = 0; i<26; i++){
            yesterday_cons_table[i] = today_cons_table[i];      // Move current days consumption to yesterdays consumption
        }              
        resetConsumption();                                     // Reset todays consumption
    }


    // Reset comparison for next scan
    prev_day = day();
    prev_hour = hour();
    heating_state_prev = heating_state;
}

// Reads out weekday in format (Mon=0, Tue=1, Wed=2, etc...)
int RoomControl::getWeekday(){
    
    // Giving the day of week as integer. Starting at monday = 0
    int weekday_american = weekday();
    int weekday_european;

    // Any day besides Sunday
    if (weekday_american > 1){
        weekday_european = weekday_american - 2;
    }
    
    //Sunday
    else if (weekday_american == 1){
        weekday_european = 6;}
    
    // Return converted weekday
    return weekday_european;

}

// Setting the id of the current room
void RoomControl::setRoomId(int id){    // Setting the id of the current room
    room_number = id;
}                        


// LONG ABSENCE
// Activates long absence program
void RoomControl::setLongAbsence(){
    long_abs = HIGH;
}

// Deactivates long absence program
void RoomControl::resetLongAbsence(){
    long_abs = LOW;
}