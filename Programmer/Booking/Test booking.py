# -*- coding: utf-8 -*-
"""
Created on Fri Apr  9 10:04:45 2021

@author: magnu
"""
import paho.mqtt.client as mqtt
import time

seconds = time.time()
local_time = time.ctime(seconds)

mqttBroker = "test.mosquitto.org"

kitchen = {
    'slot_1':'busy',
    'slot_2':'free'    
    }

bathroom = {
    'slot':'free'
    }

livingroom = {
    'slot_1':'busy',
    'slot_2':'busy',
    'slot_3':'free'
    }

def kitchen_status (room):
    if kitchen['slot_1'] == 'free':
        kitchen['slot_1'] = 'busy'
    elif kitchen['slot_2'] == 'free':
        kitchen['slot_2'] = 'busy'
    else:
        print('the kitchen is full')
          
    return kitchen
        
kitchen = kitchen_status(kitchen)

def bathroom_status (room):
    if ('free' in bathroom.values()) == True:
        bathroom['slot'] = 'busy'
    else:
        print('The bathroom is full')
    return bathroom

bathroom = bathroom_status(bathroom)

def livingroom_status (room):
    if livingroom['slot_1'] == 'free':
        livingroom['slot_1'] = 'busy'
    elif livingroom['slot_2'] == 'free':
        livingroom['slot_2'] = 'busy'
    elif livingroom['slot_3'] == 'free':
        livingroom['slot_3'] = 'busy'
    else:
        print('The livingroom is full')
    return livingroom

livingroom = livingroom_status(livingroom)


    









    