
#pragma once
#include <iostream>
#include <TimeLib.h>
#include <Arduino.h>


class RoomControl{
    private:
        
        int room_number;
        
        // LIGHT
        bool light_state;
        bool light_state_CoT;
        bool light_state_CoT_prev;

        int dimming_value;
        int dimming_value_CoT;
        int dimming_value_CoT_prev;
        

        // TEMPERATURE
        int temp_hyst;

        bool heating_state;
        bool heating_state_prev;
        unsigned long heating_pos_edge_ts;
        unsigned long heating_neg_edge_ts;
        float heating_on_period;
        float heating_on_period_consumption;
        
        int temp_req;
        int temp_in;
        int temp_out;

        int temp_CoT_home;
        int temp_CoT_home_prev;
        int temp_CoT_away;
        int temp_CoT_away_prev;
        int temp_CoT_night;
        int temp_CoT_night_prev;
        int temp_CoT_long_abs;
        int temp_CoT_long_abs_prev;

        int temp_home;
        int temp_away;
        int temp_night;
        int temp_long_abs;

        int temp_home_prev;
        int temp_away_prev;
        int temp_night_prev;
        int temp_long_abs_prev;


        // FAN
        int fan_power;
        int fan_power_auto;
        int fan_power_CoT;
        int fan_power_CoT_prev;


        // AIRING
        int window_degrees;
        int window_degrees_auto;
        int window_degrees_CoT;
        int window_degrees_CoT_prev;

        // LONG ABSENCE
        bool long_abs = 0;


        // TIME
        int prev_day;


        //CONSUMPTION
        
        int     heaterWattage;
        int     fanWattage;

        int     today_cons_table[26] = {room_number, day(), 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int     yesterday_cons_table[26] = {room_number, day()-1, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        void    resetConsumption();                                 // Resets the consumption data

    public:
        
        // CONSUMPTION
        int*    returnTodaysConsumption();                          // Gives consumption for the current day
        int*    returnYesterdaysConsumption();                      // Gives consumption for the previous day
        void    setHeaterWattage(int);                              // Setting the output power of the heating sorce (oven)
        void    setFanWattage(int);                                 // Setting the output power of the fan

        
        // TEMPERATURE
        void    updateTemperatures(int, int, int);                  // Set temp indoors, outdoors and requested
        void    setTempHysteresis(int);                             // Set temperature hysteresis (+/- x degrees)
        void    updateCurrentTemperatureList(int, int, int, int);   // Update RoomControl with the current temperature setting list
        void    updateTemperaturesFromCoT(int, int, int, int);      // Update RoomControl with the current temperature setting list from CoT
        int     getTemperatureHome();                               // Returns the updated Home temperature based on old values and CoT
        int     getTemperatureAway();                               // Returns the updated Away temperature based on old values and CoT
        int     getTemperatureNight();                              // Returns the updated Night temperature based on old values and CoT
        int     getTemperatureLongAbs();                            // Returns the updated Logn absence temperature based on old values and CoT
        bool    autoHeating();                                      // Automatic control of heater
        
        // FAN
        void    updateFanPower(int);                                // Update RoomControl with the current Fan power
        void    updateFanPowerFromCoT(int);                         // Update RoomControl with the current Fan power from CoT
        int     getFanPower();                                      // Returns the updated fan power value based on old values and CoT
        int     fanPowerAuto();                                     // Automatic control of fan power
        
        // LIGHT
        void    updateLightParameters(bool, int);                   // Update RoomControl with the current light parameters
        void    updateLightParametersFromCoT(bool, int);            // Update RoomControl with the current light parameters from CoT
        bool    getLightStatus();                                   // Returns the updated light status based on old values and CoT
        int     getLightDimming();                                  // Returns the updated light dimming value based on old values and CoT

        // AIRING
        void    updateWindowOpening(int);                           // Update RoomControl with the current window opening
        void    updateWindowOpeningFromCoT(int);                    // Update RoomControl with the current window opening from CoT
        int     getWindowOpening();                                 // Returns the updated window opening based on old values and CoT
        int     windowDegreesAuto();                                // Automatic control of window opening

        // LONG ABSENCE
        void    setLongAbsence();                                   // Activates long absence program
        void    resetLongAbsence();                                 // Deactivates long absence program


        void    update();                                           // Must be called each cycle to ensure that system updated heating control, fan control, window control etc  
        
        void    setRoomId(int);                                     // Setting the id of the current room
        
        int     getWeekday();                                       // Reads out weekday in format (Mon=0, Tue=1, Wed=2, etc...)
        

};