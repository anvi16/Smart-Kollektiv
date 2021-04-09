#ifndef BOOKING.H
#define BOOKING.H

#include "MQTT.h"

Booking book;
Mqtt_message mqtt_booking_message;

const char* _user;

void Booking_send();

void Booking_setup(const char* user) {
	_user = user;

	book.id = "Hub";
	book.room = kitchen;
	book.day = Monday;
	book.time = 35050923;

	Booking_send();
}

void Booking_loop() {
	//loop
}


void Booking_send() {
	mqtt_booking_message.resiver		= book.id;
	mqtt_booking_message.room			= book.room;
	mqtt_booking_message.header			= booking;
	mqtt_booking_message.data_String[0] = { "User",  _user };
	mqtt_booking_message.data_int[0]	= { "Dag" ,  book.day };
	mqtt_booking_message.data_int[1]	= { "Tid" ,  book.time};

	Mqtt_pub(&mqtt_booking_message);
}


void Booking_recive(bool check) {
	if (check) {
		Serial.println("Hurra");
	}

}

#endif 