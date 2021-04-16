# -*- coding: utf-8 -*-
"""
Created on Tue Apr 13 09:54:16 2021

@author: magnu
"""
# foreløpige problemer:
#   -lese hvor lenge det er før rommet er ledig
#   -om skriptet kan kjøres flere ganger oppå hverandre når du kobler til esp og den ber om booking
#   -når ESPen sender verdi "badet" så skal både bookingen og tidsfunksjenen lese den
#   - if verdi fra esp = kjøkken, skal kjøkken funksjonen kjøre etc.

# Forslag:
#   måle nedtellingen på rommet mot local time for å angi når rommet er ledig



import time
seconds = time.time()
local_time = time.ctime(seconds)
print('local time = ', local_time)

def min_to_sec (minutes):
    seconds = minutes * 60
    return seconds

def countdown(t):
	
	while t:
		mins, secs = divmod(t, 60)
		timer = '{}:{}'.format(mins, secs)
		print(timer, end=" \r")
		time.sleep(1)
		t -= 1
	
	print('Fire in the hole!!')


# input time in seconds
t = min_to_sec(30)

# function call
countdown(int(t))

        
    
