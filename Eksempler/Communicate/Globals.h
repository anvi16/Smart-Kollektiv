/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description:
		Define Enum's og struct's 
*************************************************/


#ifndef Globals_H
#define Globals_H

#include <Arduino.h>

//#define DEBUG // define for debug

/////// I/O define pins ///////
// Number of users
#define USERS 6


// Access_panel
  // RFID
	#define SS_PIN 21
	#define RST_PIN 22

  // Keypad
	#define ROW1 13
	#define ROW2 12
	#define ROW3 14
	#define ROW4 27

	#define COLS1 26
	#define COLS2 25
	#define COLS3 33

  //Servo
	#define SERVO_PIN 15

// Access_log
#define DOOR_CHECK		  34
#define DOOR_SENSOR_INNER 35
#define DOOR_SENSOR_OUTER 32

// Doorbell
#define BUTTON_UP         4 
#define BUTTON_ENTER      16 
#define BUTTON_DOWN       17




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
	Access_controll		= 0x07,
	Room_Controller		= 0x08
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
	Data_int	data_int[15];
	Data_String data_String[15];

};



#endif
