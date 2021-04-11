/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description: 
  Hand held controller
*************************************************/


#include "MQTT.h"
#include "Doorbell.h"
#include "Oled_display.h"


// Wifi: SSID, Password
const char* WIFI_SSID = "Vik";
const char* WIFI_PASSWORD = "Y897R123";

// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID = "user1";
const char* MQTT_SERVER_IP = "broker.hivemq.com";
const uint16_t  MQTT_SERVER_PORT = 1883;
// Commen topic for system
const char* MQTT_TOPIC = "My_home/mqtt";


// Initializing structs
Mqtt_message mqtt_message = { 
                               "Hub",                       // Resiver
                                NaN,                        // Room
                                None,                       // Header
                                { },                        // Data_int
                                {"User", MQTT_CLIENT_ID}    // Data_String
                            };




void setup() {
  // Start serial monior
  Serial.begin(115200);

  // Set up MQTT for communicatision
  Mqtt_setup(WIFI_SSID, WIFI_PASSWORD, MQTT_CLIENT_ID, MQTT_SERVER_IP, MQTT_SERVER_PORT, MQTT_TOPIC);
 
  // Set up Topic's
    // Subscribe to desirable topics
  //Mqtt_sub(MQTT_SUB_TOPIC);
    // Publish data to desirable topics
  Mqtt_pub(&mqtt_message);
  Oled_display_setup();
  Doorbell_setup();

  // Exsamples, change values in structs

    //mqtt_message.data1[0].key = "Heat";
    //mqtt_message.data1[0].value = 20;
    //
    //                key    value
    //mqtt_message.data_int[0] = { "Hello",  30  };
    //mqtt_message.data_int[1] = { "Hei"  ,  50  };
}


void loop() {
  Mqtt_loop();
  Mqtt_recive();

  Doorbell_loop();
}


void Mqtt_recive() {
  // Handling incoming mqtt messages
  if (String(MQTT_TOPIC).equals(Mqtt_topic)) {

  }
  if (int(Mqtt_Json_payload["header"]) == Doorbell) {
      Doorbell_recive(bool(Mqtt_Json_payload["data_int"]["reply"]));
  }
    

  // Clear message buffers 
  Mqtt_clear_msg();
}
