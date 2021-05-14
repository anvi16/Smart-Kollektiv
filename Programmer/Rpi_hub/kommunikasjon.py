# -*- coding: utf-8 -*-
"""
Created on Thu Apr 15 13:55:48 2021

@author: magnu
"""

import paho.mqtt.client as mqtt    # Import library which allows for connection to mqtt

"""
def init_booking(message):
    room = message
    print(room)
"""


def on_connect(client, userdata, flags, rc):        # Function which establishes connection to mqtt
    print('Connected with result code ' + str(rc))  # Prints if connection was sucesfull
    client.subscribe("which_room")                  # Declares which topic to subscribe to
    
    
global client               # Declare global variable client
client = mqtt.Client() 
   
def mqtt_loop(callback, sendback):
    client.on_connect = on_connect
    client.on_message = callback
    client.on_publish = sendback


    client.connect("broker.hivemq.com", 1883, 60)


    client.loop_forever()

def mqtt_publish(topic, payload):           # Function which publishes to a topic
    client.publish(topic, payload, True)
    


