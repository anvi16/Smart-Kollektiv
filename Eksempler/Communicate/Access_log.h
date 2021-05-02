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

#define DEBUG

Mqtt_message access_log_message;
MQTT mqtt_access_log;

enum State {step1, step2, step3};


void Access_log_setup() {
	pinMode(DOOR_CHECK, INPUT);
	pinMode(DOOR_SENSOR_INNER, INPUT);
	pinMode(DOOR_SENSOR_OUTER, INPUT);

}

uint32_t door_timer;
uint32_t last_read;
uint32_t last_read_sensor;

int last_read_delay			= 500;
int door_timer_delay		= 1000;
word last_read_sensor_delay = 30;

int person = 0;

bool door_sensor_inner;
bool door_sensor_outer;

bool last_door_sensor_inner;
bool last_door_sensor_outer;

bool person_in;
bool person_out;

bool inner_read;
bool outer_read;

State state = step1;


void Door_counter_reset() {
	outer_read = false;
	inner_read = false;
	person_in  = false;
	person_out = false;
	state = step1;
}


void Door_counter() {
	if (last_read_sensor + last_read_sensor_delay < millis()) {
		door_sensor_inner = digitalRead(DOOR_SENSOR_INNER);
		door_sensor_outer = digitalRead(DOOR_SENSOR_OUTER);
		last_read_sensor = millis();
	}

	switch (state) {
	case step1:
		if (door_sensor_inner) inner_read = true;
		if (door_sensor_outer) outer_read = true;

		if (inner_read || outer_read) {
			last_read = millis();
			state = step2;
		}
		break;

	case step2:
		if (outer_read && door_sensor_inner) { person_in  = true; }
		if (inner_read && door_sensor_outer) { person_out = true; }

		if (person_in || person_out) {
			door_timer = millis();
			state = step3;
		}
		else if (last_read + last_read_delay < millis()) { Door_counter_reset(); }
		break;

	case step3:
		if (!digitalRead(DOOR_CHECK)) {
			if (person_in) { person++; }
			if (person_out && person != 0) { person--; }
			Door_counter_reset();
		}
		#ifdef DEBUG
			Serial.print("Count ");
			if (person_in) Serial.println("in ");
			if (person_out) Serial.println("out ");
			Serial.println(person);
		#endif // DEBUG

		if (door_timer + door_timer_delay < millis()) { Door_counter_reset(); }
		break;
	}

}



void Access_log_loop() {
	Door_counter();
}






#endif