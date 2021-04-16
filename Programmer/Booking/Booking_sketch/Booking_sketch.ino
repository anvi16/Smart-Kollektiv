/******************************************************************************
  MQTT_Switch_Example.ino
  Example for controlling a light using an MQTT switch
  by: Alex Wende, SparkFun Electronics

  This sketch connects the ESP32 to a MQTT broker and subcribes to the topic
  room/light. When the button is pressed, the client will toggle between
  publishing "on" and "off".
******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid =  "MagnusNett";   // name of your WiFi network
const char *password =  "Arrkom2021"; // password of the WiFi network

const char *ID = "Example_Switch";  // Name of our device, must be unique
const char *TOPIC = "which_room";  // Topic to subcribe to

const char *broker = "broker.hivemq.com"; // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client

enum BookingStates {BOOKING_phase, BATHROOM};
enum BookingStates bookingState;

#define button_up 26
#define button_right 6
#define button_down 7
#define button_left 8
#define potpin 9



// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String response;
  for (int i = 0; i < length; i) {
    response = (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  Serial.println(response);

}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(button_up, INPUT);// Configure SWITCH_Pin as an input  // enable pull-up resistor (active low)
  bookingState = BOOKING_phase;
  delay(100);
  setup_wifi(); // Connect to network
  client.setServer(broker, 1883);
}

void loop() {
  if (!client.connected())  // Reconnect if connection is lost
  {
    reconnect();
  }
  client.loop();
  bookingESP32();

}
void bookingESP32() {
  //MULIGHET Å LAGE ENUM?? ETC. bookingstates {booking, kitchen, bathroom, livingroom}

  int alt1 = digitalRead(button_up);
  int alt2 = digitalRead(button_right);
  int alt3 = digitalRead(button_down);
  int alt4 = digitalRead(button_left);
  int potPos = map(analogRead(potpin), 0, 1023, 0, 180);

  switch (bookingState) {
    case BOOKING_phase:
      if (alt1 == HIGH) {
        Serial.println("book bathroom");
        delay(1000);
        bookingState = BATHROOM;
      }
      break;


    case BATHROOM:
      client.publish(TOPIC, "bathroom");
      bookingState = BOOKING_phase;
    break;
  }
}
