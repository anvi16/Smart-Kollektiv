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

const char *ssid =  "netgear";   // name of your WiFi network
const char *password =  "gracefulflower2"; // password of the WiFi network

const char *ID = "Example_Switch";  // Name of our device, must be unique
const char *TOPIC = "which_room";  // Topic to subcribe to

const char *broker = "broker.hivemq.com"; // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client

Serial.begin(9600);

enum class BookingStates {BOOKING_phase, BATHROOM, KITCHEN, LIVINGROOM};
enum BookingStates bookingState;

//enum BookingStatesKitchen {BOOKING_phase1, KITCHEN};
//enum BookingStatesKitchen bookingStateKitchen; 

#define button_up 26
#define button_right 6
#define button_down 7
#define button_left 8
#define potpin 9

int led_kitchen = 1;
int led_Bathroom = 2;
int led_Livingroom = 3;

int Green_Led = 4;
int Red_Led = 5;



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
    String response = (char)payload[i];
    client.publish("/home/openhab/in/DEMO_STATE","Callback is working");      //Adde topic
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  Serial.println(response);

  if (response == "1") {
    digitalWrite(Green_Led, HIGH);
    Serial.print("du har booket tid på");
    Serial.println(BookingStates);
    client.publish("/home/openhab/DEMO_SWITCH", "Booking_on");              //legge til topic-en vi skal abonnere til her
  } 
  if (response == "0") {
    digitalWrite(Red_Led, HIGH);
    Serial.println(BookingStates);
    Serial.print("er opptatt");
    client.publish("/home/openhab/DEMO_SWITCH", "Booking_off");             //legge til topic-en vi skal abonnere til her 
  }

}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(button_up, INPUT);// Configure SWITCH_Pin as an input  // enable pull-up resistor (active low)
  bookingState = BOOKING_phase;
  //bookingStateKitchen = BOOKING_phase1;
  delay(100);
  setup_wifi(); // Connect to network
  client.setServer(broker, 1883);

  pinMode(led_Kitchen, OUTPUT);
  pinMode(led_Livingroom, OUTPUT);    
  pinMode(led_Bathroom, OUTPUT);

  pinMode(Green_Led, OUTPUT);
  pinMode(Red_Led, OUTPUT);
  
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
        Serial.println("Book bathroom");
        delay(1000);
        bookingState c1 = BATHROOM;
      }
      break;


    case BATHROOM:
      System.Threading.Thread.Sleep(30000);
      client.publish(TOPIC, "bathroom");
      bookingState = BOOKING_phase;
      digitalWrite(led_Bathroom, HIGH);
    break;

    
    case BOOKING_phase:
      if (alt2 == HIGH) {
        Serial.println("Book kitchen");
        delay(1000);
        bookingState c1 = KITCHEN;  
      }
      break;

    case KITCHEN:
      System.Threading.Thread.Sleep(30000);
      client.publish(TOPIC, "kitchen");
      bookingState = BOOKING_phase;
      digitalWrite(led_Kitchen, HIGH);
    break;

    case BOOKING_phase:
      if (alt3 == HIGH) {
        Serial.println("Book livingroom");
        delay(1000);
        bookingState c1 = LIVINGROOM;  
      }
      break;

    case LIVINGROOM:
      System.Threading.Thread.Sleep(30000);
      client.publish(TOPIC, "livingroom");
      bookingState = BOOKING_phase;
      digitalWrite(led_Livingroom, HIGH); 
    break;

    
  
  

  }
}
