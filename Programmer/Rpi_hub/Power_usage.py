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


def Write_power_usage(consumption, ID, room=float("NaN"), place=float("NaN")):

    today = date.today().strftime("%d/%m/%Y")
    
    
  # Set up the file, if it dosen't exsist 
    if not os.path.exists(path + "/" + file):
        
      # Set up the struct of the dataframe indexses   
      # Level 1
        index_names1 = [['Sun panel', 'Sum'], [''], ['']]
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
     
    
    df = pd.read_csv(path + "/" + file)
    
  # Check if todays date 
    if not today in df.columns:
        df[today] = 0
    
  # Create multiindex
    df = df.set_index(['ID','Room','Booked'])
    
  # Store consumption value
    df.loc[(ID, room, place), today] = consumption
    
  # Update the consumed power sum 
    df.loc[('Sum'), today] = df[today].sum(axis = 0, skipna = True) - df.loc[('Sum'), today]
    
  # Store dataframe to file
    df.to_csv(path + "/" + file)
    
    return df




def Read_power_usage(ID, room=float("NaN"), place=float("NaN")):
  # Create the folder and file if it does not exsist
    if not os.path.exists(path + "/" + file):
        Write_power_usage(0, ID)
    
  # Get current date in (dd/mm/yy)
    today = date.today().strftime("%d/%m/%Y")
    
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
    
  # Check if todays date 
    if not today in df.columns:
        df[today] = 0
    
  # Create multiindex
    df = df.set_index(['ID','Room','Booked'])
    
    return df.loc[(ID, room, place), today]

    
    
def Funksjons_navn(consumption, ID, room=float("NaN"), place=float("NaN")):
    current_value = Read_power_usage(ID, room, place)
    new_value = consumption

    if current_value < new_value:
        Write_power_usage(new_value, ID, room, place)

    
    
    
if __name__ == "__main__":    
    
    df = Write_power_usage(50, 'user5', 'Livingroom')
    print(Read_power_usage('user2', 'Livingroom'))

    print(df)

