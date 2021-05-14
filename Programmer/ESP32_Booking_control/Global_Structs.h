/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description:
		Define Enum's og struct's 
*************************************************/


#ifndef Global_Structs_H
#define Global_Structs_H

#include <Arduino.h>

enum Room {
	NaN	= 0x00,
	Liv30 = 0x01,
  Liv60 = 0x02,
  Liv120 = 0x03,
  CanLiv1 = 0x04,
  CanLiv2 = 0x05,
  CanLiv3 = 0x06,
  Kitchen = 0x07,
  CanKit1 = 0x08,
  CanKit2 = 0x09,
  Toilet = 0x0a,
  Shower = 0x0b,
  CanBath = 0x0c
  
};

enum Header {
	None				= 0x00,
	Booking				= 0x01,
	Doorbell			= 0x02,
	Light				= 0x03,
	Heat				= 0x04,
	Energi_consumption	= 0x05,
	Entry_logging		= 0x06,
	Acsess_controll		= 0x07
};


struct Data_int
{
	String key;
	int value;
};


struct Data_String
{
	String key;
	String value;
};


struct Mqtt_message
{
	const char* resiver;
	Header		header;
	Room		room;
	Data_int	data_int[10];
	Data_String data_String[10];

};



#endif
