
/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	04/04/2021

Description:
        Test communication
*************************************************/

#include "MQTT_Class.h"
#include "Doorbell.h"
#include "Access_log.h"


// Wifi: SSID, Password
const char* WIFI_SSID = "Vik";
const char* WIFI_PASSWORD = "Y897R123";

// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID = "user1";
const char* MQTT_SERVER_IP = "broker.hivemq.com";
const uint16_t  MQTT_SERVER_PORT = 1883;
// Commen topic for system
const char* MQTT_TOPIC = "My_home/mqtt";

Mqtt_message mqtt_message;
MQTT mqtt;

StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_payload;


void Mqtt_callback(char* p_topic, byte* p_payload, unsigned int p_length) {
    // Concat the payload into a string
    String payload;
    for (uint8_t i = 0; i < p_length; i++) {
        payload.concat((char)p_payload[i]);
    }
    Serial.println(payload);
    
    deserializeJson(Json_payload, p_payload, p_length);
    // Check if message is for user
    if (strcmp(Json_payload["id"], MQTT_CLIENT_ID) == 0 || strcmp(Json_payload["id"], "All") == 0) {
        if (int(Json_payload["header"]) == Doorbell) {
            Doorbell_recive(bool(Json_payload["data_int"]["reply"]));
        }
    }
}


void setup() {
    mqtt.setup(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER_IP, MQTT_SERVER_PORT, Mqtt_callback);
    Oled_display_setup();
    Doorbell_setup();
    Access_log_setup();

    mqtt_message.resiver  = "Hub";
    mqtt_message.header   = Doorbell;
    mqtt_message.room     = Entry;
    mqtt_message.data_int[0] = { "key", 10 };
    mqtt.pub(mqtt_message, MQTT_TOPIC, false);
}


void loop() {
    mqtt.keepalive();
}

