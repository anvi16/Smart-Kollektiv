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

const uint8_t door_check		= DOOR_CHECK;
const uint8_t door_sensor_inner = DOOR_SENSOR_INNER;
const uint8_t dooe_sensor_outer = DOOR_SENSOR_OUTER;

Mqtt_message access_log_message;
MQTT* mqtt_access_log;

enum Stage {step1, step2, step3};


void Access_log_setup(MQTT& _mqtt) {
	mqtt_access_log = &_mqtt;

	pinMode(door_check, INPUT);
	pinMode(door_sensor_inner, INPUT);
	pinMode(dooe_sensor_outer, INPUT);

}

uint32_t last_read;
uint32_t door_timer;
uint32_t last_count;

int last_read_delay			= 500;		// Memory between first and second signals
int door_timer_delay		= 1000;		// How long to remember passing, if door is closed
int last_count_delay		= 500;		// Delay between counting (passings)

int person = 0;

bool new_read;

bool door_sensor_inner_read;
bool door_sensor_outer_read;

bool last_door_sensor_inner;
bool last_door_sensor_outer;

bool person_in;
bool person_out;

bool inner_true;
bool outer_true;

Stage stage = step1;


void Access_log_counter_reset() {
	outer_true = false;
	inner_true = false;
	person_in  = false;
	person_out = false;
	stage = step1;
}


void Access_log_counter() {
		door_sensor_inner_read = digitalRead(DOOR_SENSOR_INNER);
		door_sensor_outer_read = digitalRead(DOOR_SENSOR_OUTER);

	switch (stage) {
	case step1:
		if (new_read) {
			if (door_sensor_inner_read) inner_true = true;
			else if (door_sensor_outer_read) outer_true = true;
	    }

		if (inner_true || outer_true) {
			last_read = millis();
			stage = step2;
		}
		break;

	case step2:
		if		(outer_true && door_sensor_inner_read) { person_in  = true; }
		else if (inner_true && door_sensor_outer_read) { person_out = true; }

		if (person_in || person_out) {
			door_timer = millis();
			new_read = false;
			stage = step3;
		}
		else if (last_read + last_read_delay < millis()) { Access_log_counter_reset(); }
		break;

	case step3:
		if (!digitalRead(DOOR_CHECK)) {
			if		(person_in) { person++; }
			else if (person_out && person != 0) { person--; }
#ifdef DEBUG
			Serial.print("Count ");
			if (person_in) Serial.println("in ");
			if (person_out) Serial.println("out ");
			Serial.println(person);
#endif // DEBUG
			last_count = millis();
			Access_log_counter_reset();
		}
		if (door_timer + door_timer_delay < millis()) { Access_log_counter_reset(); }
		break;
	}
	if (!door_sensor_inner_read && !door_sensor_outer_read && last_count + last_count_delay < millis()) {
		new_read = true;
	}
}



void Access_log_loop() {
	Access_log_counter();
}






#endif