#include "Servo.h"
#include "Controller_config.h"

float inndoorTemp; 
float tempState;
//float outdoorTemp; 
float optimalTemp;
float indoorTemp;

int servoPosition80 = 80;
int servoPosition0= 0; 

Servo servo;

void Servo_setup() {
    servo.attach(pin_servo);
    digitalWrite(pin_servo, servoPosition0); //Start the servo at position 0
}

void servoMotor(int outdoorTemp) {
    Serial.println(outdoorTemp);

    if (outdoorTemp > optimalTemp && optimalTemp > inndoorTemp)
    {
        servo.write(servoPosition80);
        delay(1000);
        Serial.println(servoPosition80);
    }
    else if(outdoorTemp < optimalTemp && optimalTemp < inndoorTemp)
    {
        servo.write(servoPosition80);
        delay(1000);
        Serial.println(servoPosition80);
    }
    else {
        servo.write(servoPosition0);
        Serial.println(servoPosition0);
    }
}

/*void Dcmotor();
{
    if digitalRead(buttonDcmotor == HIGH);{
        digitalWrite(DcMotorPin2, HIGH);
    }
    else{
        digitalWrite(DcMotorPin2, LOW);
    }
*/