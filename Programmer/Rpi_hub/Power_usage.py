# -*- coding: utf-8 -*-
"""
Created on Sun May  2 01:26:41 2021

@author: Gruppe 10
"""

import pandas as pd 
from datetime import date
import os

path = "log"
file = "Power_usage.csv"


def setup():    
  # Set up the file, if it dosen't exsist 
    if not os.path.exists(path + "/" + file):
        
      # Set up the struct of the dataframe indexses   
      # Level 1
        index_names1 = [['Sum', 'Sun panel'], [''], ['']]
        index1=pd.MultiIndex.from_product(index_names1, names = ('ID','Room','Booked'))
        
      # Level 2
        index_names2 = [['user1', 'user2', 'user3', 'user4', 'user5', 'user6'], ['Livingroom', 'Kitchen', 'Dorm'], ['']]
        index2=pd.MultiIndex.from_product(index_names2, names = ('ID','Room','Booked'))
        
      # Level 3
        index_names3 = [['user1', 'user2', 'user3', 'user4', 'user5', 'user6'], ['Bathroom'], ['Toilet', 'Shower']]
        index3=pd.MultiIndex.from_product(index_names3, names = ('ID','Room','Booked'))
    
        df1 = pd.DataFrame(index=index1)
        df2 = pd.DataFrame(index=index2)
        df3 = pd.DataFrame(index=index3)
    
        df = pd.concat([df1,df2, df3], axis=1)
        
      # Create path if it does not exsist
        if not os.path.exists(path):
            try:
                os.makedirs(path)
            except OSError as err:
                print("OS error: {0}".format(err))
                raise
      # Create the file
        try:
            df.to_csv(path + "/" + file)
        except OSError as err:
            print("OS error: {0}".format(err))
            raise
     
        
     
def Write_power_usage(consumption, ID, room, booked): 
  # Create the folder and file if it does not exsist
    if not os.path.exists(path + "/" + file):
        setup()
        
  # Get current date in (dd/mm/yy) 
    today = date.today().strftime("%d/%m/%Y")
    
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
    
  # Check if todays date exsist
    if not today in df.columns:
        df[today] = 0
    
  # Create multiindex
    df = df.set_index(['ID','Room','Booked'])
    
  # Store consumption value
    df.loc[(ID, room, booked), today] = consumption
    
  # Update the consumed power sum 
    df.loc[('Sum'), today] = df[today].sum(axis = 0, skipna = True) - df.loc[('Sum'), today]
    
  # Store dataframe to file
    df.to_csv(path + "/" + file)
    
    return df




def Read_power_usage(ID, room, booked):
  # Create the folder and file if it does not exsist
    if not os.path.exists(path + "/" + file):
        setup()
    
  # Get current date in (dd/mm/yy)
    today = date.today().strftime("%d/%m/%Y")
    
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
    
  # Check if todays date 
    if not today in df.columns:
        df[today] = 0
    
  # Create multiindex
    df = df.set_index(['ID','Room','Booked'])
    
    return df.loc[(ID, room, booked), today]

    
def Handel_power_usages(consumption, ID, room=float("NaN"), booked=float("NaN")):
    average_consumption = 0
    hours = 0
    
    if consumption.__contains__(","):
        consumption = consumption.split(",")
        for kWh in consumption:
            if (int(kWh) != 0):
                average_consumption += int(kWh)
                hours += 1
        average_consumption = average_consumption / hours
    else:
        average_consumption = int(consumption)
        
    print(average_consumption)
    print(hours)
        
    current_value = Read_power_usage(ID, room, booked)
    new_value = average_consumption

    if current_value < new_value:
        Write_power_usage(new_value, ID, room, booked)
    
    
if __name__ == "__main__":    
    
    Handel_power_usages("50", 'user4', 'Livingroom')
    
    list2 = "10,10,10,10,30,0,0,0"
    
    Handel_power_usages(list2, 'user2', 'Livingroom')
    
    
    """
    df = Write_power_usage(50, 'user5', 'Livingroom')
    print(Read_power_usage('user2', 'Livingroom'))

    print(df)
"""
