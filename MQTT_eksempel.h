
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const PROGMEM char*     WIFI_SSID         = "";
const PROGMEM char*     WIFI_PASSWORD     = "";

// MQTT: ID, server IP, port, username and password
const PROGMEM char*     MQTT_CLIENT_ID    = "Bedroom1_light";
const PROGMEM char*     MQTT_SERVER_IP    = "";
const PROGMEM uint16_t  MQTT_SERVER_PORT  =  1883;
const PROGMEM char*     MQTT_USER         = "";
const PROGMEM char*     MQTT_PASSWORD     = "";

// MQTT: topics
// state
const PROGMEM char*     MQTT_LIGHT_STATE_TOPIC                = "My_home/bedroom/light/state";
const PROGMEM char*     MQTT_LIGHT_COMMAND_TOPIC              = "My_home/bedroom/light/cmd";

// brightness
const PROGMEM char*     MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC     = "My_home/bedroom/light/brightness/state";
const PROGMEM char*     MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC   = "My_home/bedroom/light/brightness/cmd";

//
const PROGMEM char*     MQTT_LIGHT_TRANSITION_COMMAND_TOPIC   = "My_home/bedroom/light/transition/cmd";

// payloads by default (on/off)
const PROGMEM char*     LIGHT_ON         = "ON";
const PROGMEM char*     LIGHT_OFF        = "OFF";
 
// variables used to store the statea and the brightness of the light
boolean   m_light_state            = true;
uint8_t   m_light_brightness       = 255;
uint32_t  m_light_transition       = 0;
int       m_light_transition_delay = 0;

// pin used for the led (PWM)
const PROGMEM uint8_t LIGHT_PIN = 13;

// buffer used to send/receive data with MQTT
const uint8_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE]; 

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to adapt the brightness and the state of the led
void setLightState() {
  if (m_light_state) {
    analogWrite(LIGHT_PIN, m_light_brightness);
    Serial.print("INFO: Brightness: ");
    Serial.println(m_light_brightness);
  } else {
    analogWrite(LIGHT_PIN, 0);
    Serial.println("INFO: Turn light off");
  }
}

void setTransition() {
  if (m_light_transition > 0) {
    for (int i=0; i < 256; i++){
      analogWrite(LIGHT_PIN, i);
      Serial.print("INFO: Brightness: ");
      Serial.println(i);
      Serial.print(" of 255");
      delay(m_light_transition_delay);
    }
  } 
}

// function called to publish the state of the led (on/off)
void publishLightState() {
  if (m_light_state) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
  }
}

// function called to publish the brightness of the led
void publishLightBrightness() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", m_light_brightness);
  client.publish(MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC, m_msg_buffer, true);
}


// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // handle message topic
  if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHT_ON))) {
      if (m_light_state != true) {
        m_light_state = true;
        setLightState();
        publishLightState();
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      if (m_light_state != false) {
        m_light_state = false;
        setLightState();
        publishLightState();
      }
    }
  } else if (String(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC).equals(p_topic)) {
    uint8_t brightness = payload.toInt();
    if (brightness < 0 || brightness > 256) {
      // do nothing...
      return;
    } else {
      m_light_brightness = brightness;
      m_light_state = true;
      setLightState();
      publishLightState();
      publishLightBrightness();
    }
  } else if (String(MQTT_LIGHT_TRANSITION_COMMAND_TOPIC).equals(p_topic)) {
    uint32_t transition = payload.toInt();
    if (transition < 0) {
      // do nothing...
      return;
    } else {
      m_light_state = true;
      m_light_transition = transition;
      m_light_transition_delay = (transition * 1000) / 256;
      publishLightState();
      setTransition();
    }
  }
} 

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("\nINFO: connected");
      
      // Once connected, publish an announcement...
      // publish the initial values
      publishLightState();
      publishLightBrightness();

      // ... and resubscribe
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_TRANSITION_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the led
  pinMode(LIGHT_PIN, OUTPUT);
  analogWriteRange(256);
  setLightState();

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
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

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}