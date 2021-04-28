
const PROGMEM char*     LIGHT_ON         = "ON";
const PROGMEM char*     LIGHT_OFF        = "OFF";
 
// variables used to store the statea and the brightness of the light
boolean   light_state           = true;
int  min_light_brightness       = 0;
int  max_light_brightness       = 255;
int  light_transition           = 0;
int  light_transition_delay     = 0;


// pin used for the led (PWM)
const int LIGHT_PIN = 13;


// function called to adapt the brightness and the state of the led
//husk å lage funksjonen setLightState
void setLightState() {
  if (light_state) {
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
