# -*- coding: utf-8 -*-
"""
Created on Sun May  2 13:46:41 2021

@author: Gruppe 8
"""

import requests  
import json   



token  = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MTQzIn0.RWWaVmYbo-jrnA0vCM8RhQmYBCr-EFsxE9_GPEcH1eA"


def push_payload(key, value):
        
    payload={'Key': str(key) ,'Value': value,'Token': token} 
    
    
    response=requests.put('https://circusofthings.com/WriteValue',
    				data=json.dumps(payload),headers={'Content-Type':'application/json'})
    
    
    if(response.status_code==200):
        print("succsess")
    else:
        print("error %d" % (response.status_code))




def pull_sensor(key):
    
    payload={'Key': str(key),'Token': token} 
    
    response = requests.get('https://circusofthings.com/ReadValue', params=payload)
    
    #print(response.content)
    
    json_response = json.loads(response.content)  
    
    return json_response["Value"]
    





if __name__ == "__main__":    
    
    #push_payload(28766, 50)

    #pull_sensor(28766)
    

    print()




