# -*- coding: utf-8 -*-
"""
Created on Sun May  2 01:26:41 2021

@author: Gruppe 8
"""

import pandas as pd 
from datetime import date, datetime
import os

path = "log"
file_name = "Power_usage"

file_csv = path + "/" + file_name + "_" + str(datetime.today().year) + ".csv"
file_h5 = path + "/" + file_name + "_" + str(datetime.today().year) + ".h5"



def setup():
    
 # Set up the file, if it dosen't exsist 
    if not os.path.exists(file_csv) or not os.path.exists(file_h5):
        
        today = date.today().strftime("%d/%m/%Y")
        
      # Set up the struct of the dataframe indexses   
      # Level 1
        index_names1 = [['Sum', 'Sun panel', 'Livingroom', 'Bathroom', 'Kitchen', 'Entry'], [''], ['']]
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
        
      # Create multiindex
        df.columns=pd.MultiIndex.from_product([df.columns, ['']])
        
      # Check if todays date exsist
        if not today in df.columns:
            for column in range(24):
                df.loc[:, (today, str(column))] = 0
        
        
      # Create path if it does not exsist
        if not os.path.exists(path):
            try:
                os.makedirs(path)
            except OSError as err:
                print("OS error: {0}".format(err))
                raise

      # Create the file
        try:
            df.to_csv(file_csv)
        except OSError as err:
            print("OS error: {0}".format(err))
            raise

      # Create the file
        try:
            df.to_hdf(file_h5, "df")
        except OSError as err:
            print("OS error: {0}".format(err))
            raise
     
        
     
    
def Write_power_usage(consumption, hour, ID, room="", booked=""):
    
  # Create the folder and files if it does not exsist
    if not os.path.exists(file_csv) or not os.path.exists(file_h5):
        setup()
    
  # Get current date in (dd/mm/yy) 
    today = date.today().strftime("%d/%m/%Y")
    
  # Read from file to dataframe
    df = pd.read_hdf(file_h5, "df")
    
  # Check if todays date exsist
    if not today in df.columns:
        for column in range(24):
            df.loc[:, (today, str(column))] = 0
            
  # Store consumption to dataframe
    df[(today, hour)][(ID, room, booked)] = consumption
    
  # Update sum  
    droped_df = df.drop(['Sum', 'Sun panel'])
    sum_hour  = droped_df.loc[:, (today, '9')].sum()
    
    df[date.today().strftime("%d/%m/%Y"), '9']['Sum'] = sum_hour
    
  # Store dataframe to file
    df.to_csv(file_csv)
    df.to_hdf(file_h5, "df")
    
    return df
    
    
    
    
def Read_power_usage(hour, ID, room="", booked=""):
    
  # Create the folder and files if it does not exsist
    if os.path.exists(file_csv) or not os.path.exists(file_h5):
        setup()

  # Get current date in (dd/mm/yy)
    today = date.today().strftime("%d/%m/%Y")

  # Read file to dataframe
    df = pd.read_hdf(file_h5, 'df')

  # Check if todays date 
    if not today in df.columns:
        for column in range(24):
            df.loc[:, (today, str(column))] = 0
        
        df.to_csv(file_csv)
        df.to_hdf(file_h5, "df")
    
  # Return value
    return df.loc[(ID, room, booked), (today, hour)]




def User_avreage(user):
  # Create the folder and files if it does not exsist
    if not os.path.exists(file_csv) or not os.path.exists(file_h5):
        setup()

  # Get current date in (dd/mm/yy)
    today = date.today().strftime("%d/%m/%Y")

  # Read file to dataframe
    df = pd.read_hdf(file_h5, 'df')

  # Check if todays date 
    if not today in df.columns:
        for column in range(24):
            df.loc[ :, (today, str(column)) ] = 0
        
        df.to_csv(file_csv)
        df.to_hdf(file_h5, "df")
    
  # Return value
    return df.loc[user, (today, str(datetime.now().hour))].sum()





def Handel_power_usages(consumption, ID, room="", booked=""):
    
  # Make shure arguments is type String
    if type(consumption) != str():
        consumption = str(consumption)
        
    hour = str(datetime.now().hour)    
    df = None
    
    
  # If consumption is a comma seperated list, split and loop through
    if consumption.__contains__(","):
        consumption = consumption.split(",")

        for i in range(len(consumption)):

          # First item in consumption date in month
            if(i == 0):
              # Make shure consumption resived is from today
                if date.today().day != int(consumption[0]):
                    break
               
            elif (int(consumption[i]) != 0):
                
                current_value = Read_power_usage(str(i-1), ID, room, booked)
                new_value = int(consumption[i])
                
                if current_value <= new_value:
                    df = Write_power_usage(new_value, str(i-1), ID, room, booked)

            
  # If consumption is singel check if it`s greater and if store it
    else:
        new_value = int(consumption)
    
        current_value = Read_power_usage(hour, ID, room, booked)

        if current_value <= new_value:
            df = Write_power_usage(new_value, hour, ID, room, booked)
        
    return df






    
    
if __name__ == "__main__":    

    # df = Handel_power_usages("50", "1", 'Sun panel')
    # df = Handel_power_usages("50", "2", 'user2', 'Livingroom')
    # df = Handel_power_usages("50", "1", 'user3', 'Bathroom', 'Shower')
    
    print()

