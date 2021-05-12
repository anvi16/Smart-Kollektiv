# -*- coding: utf-8 -*-
"""
Created on Tue May  4 15:14:57 2021

@author: Gruppe 8
"""

import pandas as pd 
import os

path = 'storage'
file = 'tolk.csv'


def setup():
  # Create the folder and file if it does not exsist
    if not os.path.exists(path + '/' + file):
        index_names4 = ['user1', 'user2', 'user3', 'user4', 'user5', 'user6']
        df = pd.DataFrame(index = index_names4)
        
        df.index.name = "id"
        df['card_id'] = "" 
        df['code'] = ""
        
        
        if not os.path.exists(path):
            try:
                os.makedirs(path)
            except OSError as err:
                print("OS error: {0}".format(err))
                raise
        try:
            df.to_csv(path + '/' + file)
        except OSError as err:
            print("OS error: {0}".format(err))
            raise         
            
                
def store_card_id(user, card_id):
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
  # Set id as index
    df = df.set_index(['id'])
  # Store users card_id to dataframe
    df.loc[user, 'card_id'] = card_id
  # Store dataframe to file
    df.to_csv(path + "/" + file)
    

def store_code(user, code):
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
  # Set id as index
    df = df.set_index(['id'])
  # Store users code to dataframe
    df.loc[user, 'code'] = code
  # Store dataframe to file
    df.to_csv(path + "/" + file)
    
    
def get_card_id(user):
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
  # Set id as index
    df = df.set_index(['id'])
  # Return users card_id from dataframe
    return df.loc[user, 'card_id']
    

def get_code(user):
  # Read file to dataframe
    df = pd.read_csv(path + "/" + file)
  # Set id as index
    df = df.set_index(['id'])
    # Return users code from dataframe
    return df.loc[user, 'code']
    
    
    
    
    
    
    
    