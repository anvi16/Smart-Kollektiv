/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description: 
  Hand held controller
*************************************************/


#include "MQTT.h"
#include "Booking.h"



// Wifi: SSID, Password
const char* WIFI_SSID = "Vik";
const char* WIFI_PASSWORD = "Y897R123";

// MQTT: ID, server IP, port, topics
const char* MQTT_CLIENT_ID = "user1";
const char* MQTT_SERVER_IP = "broker.hivemq.com"; //"192.168.0.120";
const uint16_t  MQTT_SERVER_PORT = 1883;
// Commen topic for system
const char* MQTT_TOPIC = "My_home/mqtt";

// Exstra Topics
const char* MQTT_SUB_TOPIC = "My_home/sub";
const char* MQTT_PUB_TOPIC = "My_home/pub";




// Initializing structs
Mqtt_message mqtt_message = { 
                "Hub",            // Resiver
                NaN,            // Room
                None,           // Header
                { "temp", 10, 
                  "heat", 20 },       // Data_int
                { "hei" , "verden" }    // Data_String
              };




void setup() {
  // Start serial monior
  Serial.begin(115200);

  // Set up MQTT for communicatision
  Mqtt_setup(WIFI_SSID, WIFI_PASSWORD, MQTT_CLIENT_ID, MQTT_SERVER_IP, MQTT_SERVER_PORT, MQTT_TOPIC);
  delay(5);
  // Set up Topic's
    // Subscribe to desirable topics
  Mqtt_sub(MQTT_SUB_TOPIC);
    // Publish data to desirable topics
  Mqtt_pub(&mqtt_message, MQTT_PUB_TOPIC);
  Booking_setup(MQTT_CLIENT_ID);

  // Exsamples, change values in structs
    //user.room = dormroom_2;

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
  
}


void Mqtt_recive() {
  // Handling incoming mqtt messages
  if (String("My_home/sub").equals(Mqtt_topic)) {
    Serial.println("OK");
  }
  if (String("My_home/sub").equals(Mqtt_topic)) {
    Serial.println(Mqtt_payload);
    Serial.println(int(Mqtt_Json_payload["data_int"]["Temp"]));
    Serial.println((const char*)Mqtt_Json_payload["data_String"]["Hei"]);

    int temp = Mqtt_Json_payload["data_int"]["Temp"];
  }
  if (int(Mqtt_Json_payload["header"]) == booking) {
      int booking_room = Mqtt_Json_payload["room"];
      String user = Mqtt_Json_payload["data_Sring"]["User"];
      Serial.println("Hei");

      Booking_recive(bool(Mqtt_Json_payload["data_int"]["Check"]));
  }
    

  // Clear message buffers 
  Mqtt_clear_msg();
}
