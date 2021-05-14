/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	04/04/2021

Description:
		Communication handeler
*************************************************/

#ifndef MQTT_H
#define MQTT_H

#define MQTT_MAX_PACKET_SIZE 256
#define ARDUINOJSON_ENABLE_STD_STREAM 0

#include "Global_Structs.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

extern const char* TOPIC;

class MQTT {
public:
	MQTT();
	void sub(const char* MQTT_SUB_TOPIC);
	void pub(Mqtt_message& mqtt_message, const char* MQTT_PUB_TOPIC = TOPIC, bool retain = false);
	void setup(const char* _WIFI_SSID, const char* _WIFI_PASSWORD, const char* MQTT_SERVER_IP, const uint16_t MQTT_SERVER_PORT, void (*callback)(char*, uint8_t*, unsigned int));
	void keepalive();

private:
	void reconnect();
	//void callback(char* p_topic, byte* p_payload, unsigned int p_length);

	// Internall  variables
	

};

#endif
