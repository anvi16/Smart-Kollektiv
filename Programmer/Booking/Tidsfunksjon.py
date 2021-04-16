# -*- coding: utf-8 -*-
"""
Created on Tue Apr 13 10:42:36 2021

@author: magnu
"""
import time

# converts minutes to seconds
def min_to_sec (minutes):
    seconds = minutes * 5
    return seconds





# timefunction which displays to remaining time you have in the represented room
def room_countdown(room_test):
    if room_test == 'kitchen':
        t_kitchen = min_to_sec(1)
        while t_kitchen:
            mins, secs = divmod(t_kitchen, 60)
            timer= '{}'.format(mins)
            print(timer, end=" \r")
            time.sleep(1)
            t_kitchen -= 1
        print('your kitchen time is up')
        return t_kitchen
         
     
            
    elif room_test == 'bathroom':
        t_bathroom = min_to_sec(1)
        while t_bathroom:
            mins, secs = divmod(t_bathroom, 60)
            timer= '{}:{}'.format(mins, secs)
            print(timer, end=" \r")
            time.sleep(1)
            t_bathroom -= 1
        print('your bathroom time is up')
        return t_bathroom


    elif room_test == 'livingroom':
        t_livingroom = min_to_sec(40)
        while t_livingroom:
            mins, secs = divmod(t_livingroom, 60)
            timer= '{}'.format(mins)
            print(timer, end=" \r")
            time.sleep(60)
            t_livingroom -= 60
        print('your livingroom time is up')
        return t_livingroom
            

            
# which room you would like to book
room_test = 'kitchen'

# function call

room_time = print(room_countdown(room_test))


