# -*- coding: utf-8 -*-
"""
Created on Fri Apr 23 12:13:50 2021

@author: rafun
"""

from metno_locationforecast import Place, Forecast

# Add your own user agent here.
USER_AGENT = "metno_locationforecast/1.0 albertra@stud.ntnu.no"

# Create a Place instance.
# Place(name, latitude, longitude, altitude)
Gløshaugen = Place("Gløshaugen", 63.418084, 10.406810, 40)

# Create a Forecast instance for the place.
# Forecast(place, forecast_type, user_agent, save_location="./data/")
Gløshaugen_forecast = Forecast(Gløshaugen, USER_AGENT)

# Update the forecast. This requests data from the MET API and saves data to the
# save location.
def get(): 
    Gløshaugen_forecast.update()
    interval= Gløshaugen_forecast.data.intervals[30]
    #print (interval)

    uteTemp= interval.variables["air_temperature"]
    #print (uteTemp)
    
    return uteTemp.value