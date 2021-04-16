# -*- coding: utf-8 -*-
"""
Created on Sat Apr 10 13:42:51 2021

@author: magnu
"""
import Tidsfunksjon as t

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


room = 'kitchen'
def room_status (room):
    if room == 'kitchen':
        if kitchen['slot_1'] == 'free':
            kitchen['slot_1'] = 'busy'
            room_time = t.room_countdown(room)
            if room_time == 0:
                kitchen['slot_1'] = 'free'
        elif kitchen['slot_2'] == 'free':
            kitchen['slot_2'] = 'busy'
            room_time = t.room_countdown(room)
            if room_time == 0:
                kitchen['slot_2'] = 'free'
        else:
            print('the kitchen is full')
        return kitchen
        
    elif room == 'bathroom':
        if ('free' in bathroom.values()) == True:
            bathroom['slot'] = 'busy'
            print(bathroom)
            room_time = t.room_countdown(room)
            if room_time == 0:
                bathroom['slot'] = 'free'
        else:
            print('The bathroom is full, it will be ready in minutes')

        return bathroom
    
    elif room == 'livingroom':
        if livingroom['slot_1'] == 'free':
            livingroom['slot_1'] = 'busy'
            room_time = t.room_countdown(room)
            if room_time == 0:
                livingroom['slot_1'] = 'free'
        elif livingroom['slot_2'] == 'free':
            livingroom['slot_2'] = 'busy'
            room_time = t.room_countdown(room)
            if room_time == 0:
                livingroom['slot_2'] = 'free'
        elif livingroom['slot_3'] == 'free':
            livingroom['slot_3'] = 'busy'
            room_time = t.room_countdown(room)
            if room_time == 0:
                livingroom['slot_3'] = 'free'
        else:
            print('The livingroom is full')
    
        return livingroom

if room =='kitchen':
    kitchen = room_status(room)
    print(kitchen)
elif room == 'bathroom':
    bathroom = room_status(room)
elif room == 'livingroom':
    livingroom = room_status(livingroom)
    