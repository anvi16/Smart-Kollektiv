
#ifndef Global_Structs_H
#define Global_Structs_H


enum Users {
	Hub	  = 0x00,
	user1 = 0x01,
	user2 = 0x02,
	user3 = 0x03,
	user4 = 0x04,
	user5 = 0x05,
	user6 = 0x06
};

enum Room {
	NaN			= 0x00,
	dormroom_1	= 0x01,
	dormroom_2	= 0x02,
	dormroom_3	= 0x03,
	dormroom_4	= 0x04,
	dormroom_5	= 0x05,
	dormroom_6	= 0x06,
	livingroom	= 0x07,
	kitchen		= 0x08,
	bathroom	= 0x09,
	entry		= 0x0a
};

enum Days {
	Unused		= 0x00,
	Monday		= 0x01,
	Tuseday		= 0x02,
	Wedneday	= 0x03,
	Tursday		= 0x04,
	Friday		= 0x05,
	Saturday	= 0x06,
	Sunday		= 0x07
};

enum Header {
	None				= 0x00,
	booking				= 0x01,
	Doorbell			= 0x02,
	Light				= 0x03,
	Heat				= 0x04,
	Energi_consumption	= 0x05,
	Entry_logging		= 0x06,
	Acsess_controll		= 0x07
};

typedef struct 
{
	String id;
	Room room;
	Days day;
	String time;
} Booking;

typedef struct
{
	uint8_t id;
	Room room;
	String energi_consumption;
	uint8_t guests;
} User;

typedef struct
{
	String key;
	int value;
} Data_int;

typedef struct
{
	String key;
	String value;
} Data_String;

struct Mqtt_message
{
	Users resiver;
	Room room;
	Header header;
	Data_int data_int[10];
	Data_String data_String[10];
};

#endif
