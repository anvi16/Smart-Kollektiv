# -*- coding: utf-8 -*-
"""
Created on Thu Apr 15 13:55:48 2021

@author: magnu
"""

import paho.mqtt.client as mqtt
import json
from queue import Queue
q = Queue()

def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))
    client.subscribe("which_room")
    
def on_message(client, userdata, message):
    #global messages
    m = "message received ", str(message.payload.decode("utf-8"))
    q.put(m)
    print("message recieved ", m)

while not q.empty():
    message = q.get()
    print("queue: ", message)
 
  

    
    
    
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


client.connect("broker.hivemq.com", 1883, 60)


client.loop_forever()






