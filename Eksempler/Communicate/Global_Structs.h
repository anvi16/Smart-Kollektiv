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
	NaN		   = 0x00,
	Dormroom_1 = 0x01,
	Dormroom_2 = 0x02,
	Dormroom_3 = 0x03,
	Dormroom_4 = 0x04,
	Dormroom_5 = 0x05,
	Dormroom_6 = 0x06,
	Livingroom = 0x07,
	Kitchen	   = 0x08,
	Bathroom   = 0x09,
	Entry      = 0x0a
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
