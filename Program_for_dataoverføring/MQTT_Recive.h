/*
	Handeling all incoming messages
*/

#ifndef MQTT_Recive_H
#define MQTT_Recive_H

#include "MQTT.h"



void Mqtt_recive() {
	// Handling incoming mqtt messages
	if (String("My_home/sub").equals(Mqtt_topic)) {
		Serial.println("OK");
	}
	if (String("My_home/sub").equals(Mqtt_topic)) {
		Serial.println(Mqtt_payload);
		Serial.println(int(Mqtt_Json_payload["data_int"]["Temp"]));
		Serial.println((const char*)Mqtt_Json_payload["data_String"]["Hei"]);

	}

	// Clear message buffers 
	Mqtt_clear_msg();
}

#endif