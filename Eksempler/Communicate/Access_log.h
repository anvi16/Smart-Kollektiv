/************************************************
Prosjekt: Smart Kollektiv
Created by: Group 8
Version: v1.0  04/04/2021

Description:
		Access log
*************************************************/

#ifndef ACCESS_LOG.H
#define ACCESS_LOG.H

#include "MQTT_Class.h"

#define DOOR_CHECK 26
#define DOOR_SENSOR_INNER 25
#define DOOR_SENSOR_OUTER 33

Mqtt_message access_log_message;
MQTT mqtt_access_log;

enum State {step1, step2, step3, step4};


void Access_log_setup() {
	pinMode(DOOR_CHECK, INPUT);
	pinMode(DOOR_CHECK, INPUT);
	pinMode(DOOR_CHECK, INPUT);


}

uint32_t delay_counter = 1000;
uint32_t delay_debounce = 20;
uint32_t door_delay_timer;

int door_count_delay;

int person = 0;

bool door_sensor_inner;
bool door_sensor_outer;
bool last_door_sensor_inner;
bool last_door_sensor_outer;

bool person_in;
bool person_out;

bool inner_read;
bool outer_read;
void Access_log_loop() {

}

void Door_counter() {

	switch (State) {
	case step1: 
		if (door_sensor_inner) inner_read = true;
		if (door_sensor_outer) outer_read = true;

		State = step2;
	case step2:	
		if (door_sensor_inner && outer_read) person_in  = HIGH;
		if (door_sensor_outer && inner_read) person_out = HIGH;
				
		State = step3;
	case step3: 
		if (!digitalRead(DOOR_CHECK)) {
			if (person_in) person++;
		}

		if (door_delay_timer + door_count_delay < millis()) State = step1;
	}
	
}








#endif