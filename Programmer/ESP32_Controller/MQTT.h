/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0	04/04/2021

Description:
    Mqtt controll
*************************************************/

#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include "libraries/PubSubClient/src/PubSubClient.cpp"
#include <ArduinoJson.h>
#include "Global_Structs.h"


// Global variables
char* Mqtt_topic;
String Mqtt_payload;
StaticJsonDocument<256> Mqtt_Json_payload;


// Internall  variables
const char* _WIFI_SSID;
const char* _WIFI_PASSWORD;
const char* _MQTT_CLIENT_ID;
const char* _MQTT_SERVER_IP;
uint16_t    _MQTT_SERVER_PORT;
const char* _MQTT_TOPIC;



WiFiClient wifiClient;
PubSubClient client(wifiClient);




void Mqtt_sub(const char* MQTT_SUB_TOPIC) {   // Subscribe to desirable topics
    client.subscribe(MQTT_SUB_TOPIC);
}


void Mqtt_pub(Mqtt_message *mqtt_message, const char* MQTT_PUB_TOPIC = _MQTT_TOPIC) {   // Publish data to desirable topics
    StaticJsonDocument<256> Json_Buffer;
    size_t len;

    Json_Buffer["id"] = mqtt_message->resiver;
    Json_Buffer["room"] = mqtt_message->room;
    Json_Buffer["header"] = mqtt_message->header;
    

    // Create a Json array in Json_Buffer and add data1 from pointer mqtt_message to it
    JsonObject Data_int = Json_Buffer.createNestedObject("data_int");
    len = sizeof(mqtt_message->data_int) / sizeof(mqtt_message->data_int[0]);
    for (uint8_t i = 0; i < len; i++) 
    {
        // Check if the key is empty
        if (!mqtt_message->data_int[i].key.equals(""))
        {
            // Store data to a JsonObject in the Json_Buffer
            Data_int[mqtt_message->data_int[i].key] = mqtt_message->data_int[i].value;

            // Clear data in pointer mqtt_message after it is stored
            mqtt_message->data_int[i].key = "";
            mqtt_message->data_int[i].value = 0;
        }
        else continue;
    }

    // Create a Json array in Json_Buffer and add data2 from pointer mqtt_message to it
    JsonObject Data_String = Json_Buffer.createNestedObject("data_String");
    len = sizeof(mqtt_message->data_String) / sizeof(mqtt_message->data_String[0]);
    for (uint8_t i = 0; i < len; i++)
    {
        // Check if the key is empty
        if (!mqtt_message->data_String[i].key.equals(""))
        {
            // Store data to a JsonObject in the Json_Buffer
            Data_String[mqtt_message->data_String[i].key] = mqtt_message->data_String[i].value;

            // Clear data in pointer mqtt_message after it is stored
            mqtt_message->data_String[i].key = "";
            mqtt_message->data_String[i].value = "";
        }
        else continue;
    }

    char buffer[256];
    size_t n = serializeJson(Json_Buffer, buffer);
    Serial.print("Package size Json : ");
    Serial.println(n);

    // Send message
    client.publish(MQTT_PUB_TOPIC, buffer);

    // Clear data in pointer mqtt_message after it is stored
    mqtt_message->room = NaN;
    mqtt_message->header = None;
}


void Mqtt_clear_msg() {   // Clear messages buffers
    Mqtt_topic = NULL;
    Mqtt_payload = "";
    Mqtt_Json_payload = NULL;
}


void Mqtt_callback(char* p_topic, byte* p_payload, unsigned int p_length) {   // Store incoming messages to buffes
    // Concat the payload into a string
    String payload;
    for (uint8_t i = 0; i < p_length; i++) {
        payload.concat((char)p_payload[i]);
    }

    StaticJsonDocument<256> Json_Buffer;
    deserializeJson(Json_Buffer, p_payload, p_length);

    // Filter messages addresed to user
    if (strcmp(Json_Buffer["id"], _MQTT_CLIENT_ID) == 0 || strcmp(Json_Buffer["id"], "All") == 0) {
        // Store payload in buffers
        Mqtt_topic = p_topic;
        Mqtt_payload = payload;
        Mqtt_Json_payload = Json_Buffer;
    }
}


void Mqtt_reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(_MQTT_CLIENT_ID)) {
            Serial.println("connected");

            // Once connected, publish an announcement...
            client.publish(_MQTT_CLIENT_ID, "hello world");
            // ... and resubscribe
            client.subscribe(_MQTT_TOPIC);

        }
        else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


void Mqtt_setup(const char* WIFI_SSID, const char* WIFI_PASSWORD, const char* MQTT_CLIENT_ID, const char* MQTT_SERVER_IP, const uint16_t MQTT_SERVER_PORT, const char* MQTT_TOPIC) {
    
    _WIFI_SSID        = WIFI_SSID;
    _WIFI_PASSWORD    = WIFI_PASSWORD;
    _MQTT_CLIENT_ID   = MQTT_CLIENT_ID;
    _MQTT_SERVER_IP   = MQTT_SERVER_IP;
    _MQTT_SERVER_PORT = MQTT_SERVER_PORT;
    _MQTT_TOPIC       = MQTT_TOPIC;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_WIFI_SSID, _WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("INFO: WiFi connected");
    Serial.print("INFO: IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(_MQTT_SERVER_IP, _MQTT_SERVER_PORT);
    client.setCallback(Mqtt_callback);

    // Connect to Mqtt broker
    Mqtt_reconnect();
    client.loop();
}


void Mqtt_loop() {
    if (!client.connected()) {
        Mqtt_reconnect();
    }

    client.loop();
}


#endif