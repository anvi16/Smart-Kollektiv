#include <Controller_config.h>
#include <math.h>

/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	02/05/2021

Description: 
  Peripherals functions

Resp & dev: 
  Jorgen Andreas Mo

*************************************************/


int light_intensity_setpoint(int input_pin, int output_resolution){
    
    analogReadResolution(10);                                   // Set 10 bit resolution for measurement
    int measured_intensity = (analogRead(input_pin) * pow(2, output_resolution)) / pow(2, 10);  // Divide 10 bit value by 4 to get 8 bit value
    
    return measured_intensity;
}
