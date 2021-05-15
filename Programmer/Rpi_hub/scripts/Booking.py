# -*- coding: utf-8 -*-
"""
Created on Wed Apr 28 15:22:20 2021

@author: magnu
"""


import time
import enum
import pandas as pd



def setup(client):
    global k
    k = client



# Fucntion which converts current time since 1.jan 1970 to milliseconds
def current_milli_time():
    return round(time.time() * 1000)

class Header(enum.IntEnum):
    none = 0x00
    Booking	= 0x01
    Doorbell = 0x02
    Light = 0x03
    Heat = 0x04
    Energi_consumption = 0x05
    Entry_logging = 0x06
    Acsess_controll = 0x07
    Room_Controller = 0x08
    
class Room(enum.IntEnum):
    NaN = 0x00
    Liv30 = 0x01
    Liv60 = 0x02
    Liv120 = 0x03
    CanLiv1 = 0x04
    CanLiv2 = 0x05
    CanLiv3 = 0x06
    Kitchen = 0x07
    CanKit1 = 0x08
    CanKit2 = 0x09
    Toilet = 0x0a
    Shower = 0x0b
    CanBath = 0x0c


# Dictionary for kitchen, bathroom og livingroom slots
kitchen = {
    'slot_1':'free',
    'slot_2':'free'    
    }

bathroom = {
    'slot':'free'
    }

livingroom = {
    'slot_1':'free',
    'slot_2':'free',
    'slot_3':'free'
    }

# Defining variables which will be used inside the booking functions
kit_time1, kit_time2, bath_time, liv_time1, liv_time2, liv_time3 = 0, 0, 0, 0, 0, 0
previous_value = 0

df = pd.DataFrame()


def kitchen_status(room, time, user):     # Function for kitchen booking
    
    global kit_time1, kit_time2  # defining two global variables, so they can be used inside the function
    
    if room == 'Kitchen':        # Checks if the recived message is Kitchen
            
        if current_milli_time() >= kit_time1 + time:
            kitchen['slot_1'] = 'free'
        if current_milli_time() >= kit_time2 + time:
            kitchen['slot_2'] = 'free'             
            # Checks if the current time is greater or equal to the slot time for the kitchen
            # If it is greater, then the time has run out and the slot is free once more
        
        if kitchen['slot_1'] == 'free':
            kitchen['slot_1'] = 'busy'
            kit_time1 = current_milli_time()
            mqtt_publish("booked", "You have booked kitchen slot 1 for 30 minutes")
            # Checks if slot 1 is free, then occupies the slot and starts the clock if the statement is True
            # Publishes to mqtt broker
            
        elif kitchen['slot_2'] == 'free':
            kitchen['slot_2'] = 'busy'
            kit_time2 = current_milli_time()
            mqtt_publish("booked", "You have booked kitchen slot 2 for 30 minutes")
            # Checks if slot 2 is free, then occupies the slot and starts the clock if the statement is True
            # Publishes to mqtt broker
            
        else:
            kit_1 = kit_time1 + time
            rem_1 = kit_1 - current_milli_time()
            kit_2 = kit_time2 + time
            rem_2 = kit_2 - current_milli_time()           
            # Declares two variables, kit_1 and kit_2, for which value current millis have when the slot time is over
            # Declares two variables, rem_1 and rem_2, which contains the remaing slot time                        
            
            if rem_1 <= rem_2:
                mqtt_publish("booked", "The kitchen is full, it will be free in " + str(round(rem_1/60000, 0)) + " minutes")
            elif rem_2 <= rem_1:
                mqtt_publish("booked","The kitchen is full, it will be free in " + str(round(rem_2/60000, 0)) + " minutes")
            # Checks which reamning time is the lowest
            # Publishes to mqtt how long to wait before a slot is free
            
    elif room == 'Cancel kit slot1':     # Checks if messaged receved asked to cancel the booking for slot 1
        kitchen['slot_1'] = 'free'       # Sets the slot to free 
        mqtt_publish('booked', 'You have canceled your slot')
    elif room == 'Cancel kit slot2':     # Checks if messaged recieved asked to cancel the booking for slot 2
        kitchen['slot_2'] = 'free'       # Sets the slot to free
        mqtt_publish('booked', 'You have canceled your slot')
            
        return kitchen      # Rreturns value for kitchen
        
def bathroom_status(room, time, user):                   # Function for bathroom booking
    global bath_time                                     # Defining global variable                  
    if room == 'Toilet':                                 # Checks if message asks to book the toilet
        if current_milli_time() >= bath_time + time:    
            bathroom['slot'] = 'free'
            # Checks if the current time is greater or equal to the slot time given for bathroom
            # If it is greater, then the time has run out and the slot is free once more
        
        if ('free' in bathroom.values()) == True:   # Cheks if dictionary bathroom 
            bathroom['slot'] = 'busy'               # Changes the slot from free to busy
            bath_time = current_milli_time()   # Time in millis when the slot was booked
            mqtt_publish('booked', 'You have booked the bathroom for 5 minutes')
            # Publishes message to mqtt
            
        
        else:
            ba_1 = bath_time + time                     # Sets the time when the bathroom time has run out
            rem_bath_1 = ba_1 - current_milli_time()    # Remaing time in millis before slot time has run out
            ba_2 = bath_time + time                     # Sets the time when the bathroom time has run out
            rem_bath_2 = ba_2 - current_milli_time()
        
            if rem_bath_1 <= rem_bath_2:
                mqtt_publish('booked', 'The bathroom is full, it will be free in ' + str(round(rem_bath_1/60000, 0)) + ' minutes')
            elif rem_bath_1 >= rem_bath_2:
                mqtt_publish('booked', 'The bathroom is full, it will be free in ' + str(round(rem_bath_2/60000, 0)) + ' minutes')   
                # Publishes message to mqtt on how long time remains before the slot is free
            
            
    elif room == 'Shower':                          # Checks if message asks to book the shower
        if current_milli_time() >= bath_time + time:
            bathroom['slot'] = 'free'
            # Checks if the current time is greater or equal to the slot time given for bathroom
            # If it is greater, then the time has run out and the slot is free once more
            
        if ('free' in bathroom.values()) == True:   # Cheks if dictionary bathroom if current_milli_time() >= kit_time1 + 1800000
            bathroom['slot'] = 'busy'               # Changes the slot from free to busy
            bath_time = current_milli_time()        # Time in millis when the slot became occupied
            mqtt_publish('booked', 'You have booked the bathroom for 15 minutes')
            
        
        else:
            ba_1 = bath_time + time                     # Sets the time when the bathroom time has run out
            rem_bath_1 = ba_1 - current_milli_time()
            ba_2 = bath_time + time                     # Sets the time when the bathroom time has run out
            rem_bath_2 = ba_2 - current_milli_time()    # Remaing time in millis before slot time has run out
            
            if rem_bath_1 <= rem_bath_2:
                mqtt_publish('booked', 'The bathroom is full, it will be free in ' + str(round(rem_bath_1/60000, 0)) + ' minutes')
            elif rem_bath_1 >= rem_bath_2:
                mqtt_publish('booked', 'The bathroom is full, it will be free in ' + str(round(rem_bath_2/60000, 0)) + ' minutes')
            
        
        
    
    elif room == 'Cancel bathroom':                             # Checks if user wants to cancel bathroom slot
        bathroom['slot'] = 'free'                               # Sets bathroom status to free
        mqtt_publish('booked', 'You have canceled your slot')   #Publish message to mqtt
                                
            
        return bathroom
    
def livingroom_status(room, time, user):        # Function for livingroom booking
    global liv_time1, liv_time2, liv_time3      # Defining global variables
    if room == 'Livingroom':                    # Checks if subscribed value is Livingroom
        
        if current_milli_time() >= liv_time1 + time:
            livingroom['slot_1'] = 'free'            
        if current_milli_time() >= liv_time2 + time:
            livingroom['slot_2'] = 'free'
        if current_milli_time() >= liv_time3 + time:
            livingroom['slot_3'] = 'free'
            # Checks if the current time is greater or equal to the slot time given for the livingroom
            # If it is greater, then the time has run out and the slot is free once more
            
        if livingroom['slot_1'] == 'free':
            livingroom['slot_1'] = 'busy'       # Checks if slot 1 is free and sets it to busy            
            liv_time1 = current_milli_time()    # Starts timer for livingroom
            mqtt_publish('booked', 'You have booked livingroom slot 1 for ' + str(round(time/60000, 0)) + ' minutes')
            # Publishes to mqtt

        
        elif livingroom['slot_2'] == 'free':            
            livingroom['slot_2'] = 'busy'       # Checks if slot 2 is free and sets it to busy
            liv_time2 = current_milli_time()    # Starts timer for livingroom
            mqtt_publish('booked', 'You have booked livingroom slot 2 for ' + str(round(time/60000, 0)) + ' minutes')
            # Publishes to mqtt

        
        elif livingroom['slot_3'] == 'free':
            livingroom['slot_3'] = 'busy'       # Checks if slot 3 is free and sets it to busy
            liv_time3 = current_milli_time()    # Starts timer for livingroom
            mqtt_publish('booked', 'You have booked livingroom slot 3 for ' + str(round(time/60000, 0)) + ' minutes')
            # Publishes to mqtt
            
        else:
            
            liv_1 = liv_time1 + time
            rem_1 = liv_1 - current_milli_time()
            liv_2 = liv_time2 + time
            rem_2 = liv_2 - current_milli_time()
            liv_3 = liv_time3 + time
            rem_3 = liv_3 - current_milli_time()         
            # Defines variables, liv_1.., for at which time the livingroom timer has run out
            # Defines variables , rem_1.., for how much time is remaing until the respective slot is free
            
            if (rem_1 <= rem_2 and rem_1 <= rem_3):
                mqtt_publish('booked', "The livingroom is full, it will be free in " + str(round(rem_1/60000, 0)) + " minutes")
            elif (rem_2 <= rem_1 and rem_2 <= rem_3):
                mqtt_publish('booked', "The livingroom is full, it will be free in " + str(round(rem_2/60000, 0)) + " minutes")
            elif (rem_3 <= rem_1 and rem_3 <= rem_1):
                mqtt_publish('booked', "The livingroom is full, it will be free in " + str(round(rem_3/60000, 0)) + " minutes")
            # Checks which slot has the least time remaining
            # Publishes to mqtt how long one have to wait before a slot is free        
        
    elif room == 'Cancel liv slot1':
        livingroom['slot_1'] = 'free'
        mqtt_publish('booked', 'You have canceled your slot')
    elif room == 'Cancel liv slot2':
        livingroom['slot_2'] = 'free'
        mqtt_publish('booked', 'You have canceled your slot')
    elif room == 'Cancel liv slot3':
        livingroom['slot_3'] = 'free'
        mqtt_publish('booked', 'You have canceled your slot')
        # Checks if the message recieved ask for cancellation of any slots
        # Sets the specific slot back to free
        
        return livingroom   # Returns livingroom state
   
    
def book_room(user, room):  # Receive booking requests

    if (room == Room.Kitchen):
        time = 30 * 60000  # Sets time for kitchen
        kitchen = kitchen_status('Kitchen', time, user)     # Checks if message is kitchen, then runs kitchen function

    elif (room == Room.Toilet):
        time = 5 * 60000  # Sets time for use of toilet
        bathroom = bathroom_status('Toilet', time, user)    # Calls bathroom function

    elif (room == Room.Shower):
        time = 15 * 60000   # Sets time for use of shower
        bathroom = bathroom_status('Shower', time, user)    # Calls bathroom function

    elif (room == Room.Liv30):
        time = 30 * 60000   # Sets livingroom time to 30 minutes
        livingroom = livingroom_status('Livingroom', time, user)    # Calls livingroom function

    elif (room == Room.Liv60):
        time = 60 * 60000   # Sets livingroom time to 60 minutes
        livingroom = livingroom_status('Livingroom', time, user)    # Calls livingroom function

    elif (room == Room.Liv120):
        time = 120 * 60000  # Sets livingroom time to 120 minutes
        livingroom = livingroom_status('Livingroom', time, user)    # Calls livingroom function

    elif (room == Room.CanKit1):
        kitchen = kitchen_status('Cancel kit slot1', 0, "")

    elif (room == Room.CanKit2):
        kitchen = kitchen_status('Cancel kit slot2', 0, "")

    elif (room == Room.CanBath):
        bathroom = bathroom_status('Cancel bathroom', 0, "")

    elif (room == Room.CanLiv1):
        livingroom = livingroom_status('Cancel liv slot1', 0, "")

    elif (room == Room.CanLiv2):
        livingroom = livingroom_status('Cancel liv slot2', 0, "")

    elif (room == Room.CanLiv3):
        livingroom = livingroom_status('Cancel liv slot3', 0, "")


def mqtt_publish(topic, payload):  # Function which publishes to a topic
    k.publish(topic, payload, False)


