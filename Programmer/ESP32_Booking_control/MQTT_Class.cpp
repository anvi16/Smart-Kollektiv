
#include "MQTT_Class.h"

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* MQTT_CLIENT_ID;
extern const char* TOPIC;
extern const char* TOPIC_BACK;


WiFiClient wifiClient;
PubSubClient client(wifiClient);


MQTT::MQTT() {

}


void MQTT::sub(const char* MQTT_SUB_TOPIC) {   // Subscribe to desirable topics
    client.subscribe(MQTT_SUB_TOPIC);
}


void MQTT::pub(Mqtt_message& mqtt_message, const char* MQTT_PUB_TOPIC, bool retain) {   // Publish data to desirable topics
    StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_Buffer;
    size_t len;
    
    Json_Buffer["id"] = mqtt_message.resiver;
    Json_Buffer["header"] = mqtt_message.header;
    Json_Buffer["room"] = mqtt_message.room;
    


    // Create a Json array in Json_Buffer and add data1 from pointer mqtt_message to it
    JsonObject Data_int = Json_Buffer.createNestedObject("data_int");
    len = sizeof(mqtt_message.data_int) / sizeof(mqtt_message.data_int[0]);
    for (uint8_t i = 0; i < len; i++)
    {
        // Check if the key is empty
        if (!mqtt_message.data_int[i].key.equals(""))
        {
            // Store data to a JsonObject in the Json_Buffer
            Data_int[mqtt_message.data_int[i].key] = mqtt_message.data_int[i].value;

            // Clear data in pointer mqtt_message after it is stored
            mqtt_message.data_int[i].key = "";
            mqtt_message.data_int[i].value = 0;
        }
        else continue;
    }

    // Create a Json array in Json_Buffer and add data2 from pointer mqtt_message to it
    JsonObject Data_String = Json_Buffer.createNestedObject("data_String");
    len = sizeof(mqtt_message.data_String) / sizeof(mqtt_message.data_String[0]);
    for (uint8_t i = 0; i < len; i++)
    {
        // Check if the key is empty
        if (!mqtt_message.data_String[i].key.equals(""))
        {
            // Store data to a JsonObject in the Json_Buffer
            Data_String[mqtt_message.data_String[i].key] = mqtt_message.data_String[i].value;

            // Clear data in pointer mqtt_message after it is stored
            mqtt_message.data_String[i].key = "";
            mqtt_message.data_String[i].value = "";
        }
        else continue;
    }
    
    char buffer[MQTT_MAX_PACKET_SIZE];
    size_t n = serializeJson(Json_Buffer, buffer);
    Serial.print("Package size Json : ");
    Serial.println(n);

    // Check if buffer is small enough
    try {
        if (n < MQTT_MAX_PACKET_SIZE) {
            // Send message
            client.publish(MQTT_PUB_TOPIC, buffer, retain);
        }
        else {
            throw (n);
        }
    }
    catch(int n) {
        Serial.println("------ERROR-------");
        Serial.println("Message is to large, try to increase \"MQTT_MAX_PACKET_SIZE\" ");
        Serial.print("Tried to send: ");
        Serial.print(n);
        Serial.print("bytes");
    }
    
    // Clear data in pointer mqtt_message after it is stored
    mqtt_message.header = None;
    mqtt_message.room = NaN;
}

/*
void MQTT::callback(char* p_topic, byte* p_payload, unsigned int p_length) {   // Store incoming messages to buffes
    // Concat the payload into a string
    String payload;
    for (uint8_t i = 0; i < p_length; i++) {
        payload.concat((char)p_payload[i]);
    }

    Serial.println(payload);
    StaticJsonDocument<MQTT_MAX_PACKET_SIZE> Json_Buffer;
    deserializeJson(Json_Buffer, p_payload, p_length);

    // Filter messages addresed to user
    if (strcmp(Json_Buffer["id"], _MQTT_CLIENT_ID) == 0 || strcmp(Json_Buffer["id"], "All") == 0) {
        // Store payload in buffers

        //Mqtt_Json_payload = Json_Buffer;
    }
}
*/

void MQTT::reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println("connected");

            // Once connected, publish an announcement...
            char* text = ":hello world";
            char buffer[strlen(MQTT_CLIENT_ID)+strlen(text)];
            strcpy(buffer, MQTT_CLIENT_ID);
            strcat(buffer, text);

            //client.publish(TOPIC, buffer);
            // ... and resubscribe
            client.subscribe(TOPIC_BACK);

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


void MQTT::setup(const char* WIFI_SSID, const char* WIFI_PASSWORD, const char* MQTT_SERVER_IP, const uint16_t MQTT_SERVER_PORT, void (*callback)(char*, uint8_t*, unsigned int)) {
    Serial.begin(115200);
       
    //////////////// WIFI /////////////////
    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("INFO: WiFi connected");
    Serial.print("INFO: IP address: ");
    Serial.println(WiFi.localIP());
    
    /////////////// MQTT ////////////////
    client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    client.setCallback(callback);
    //client.setCallback(std::bind(&MQTT::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    reconnect();
    // Run once
    client.loop();
}


void MQTT::keepalive() {
    if (!client.connected()) {
        reconnect();
    }

    client.loop();
}
