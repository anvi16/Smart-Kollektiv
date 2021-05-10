import time
import json
import enum
import paho.mqtt.client as paho
import User_tolk
import Power_usage
import Outdoor_temp


###### Declare variables ######
id = "Rpi_Hub"
topic = "My_home/mqtt"
broker="broker.hivemq.com"



class Header(enum.IntEnum):
    none                = 0x00
    Booking	            = 0x01
    Doorbell            = 0x02
    Light               = 0x03
    Heat                = 0x04
    Energi_consumption  = 0x05
    Entry_logging       = 0x06
    Acsess_controll     = 0x07
    Room_Controller     = 0x08
    
    
    
class Room(enum.IntEnum):
	All        = 0x00
	Dormroom_1 = 0x01
	Dormroom_2 = 0x02
	Dormroom_3 = 0x03
	Dormroom_4 = 0x04
	Dormroom_5 = 0x05
	Dormroom_6 = 0x06
	Livingroom = 0x07
	Kitchen	   = 0x08
	Bathroom   = 0x09
	Entry      = 0x0a

    


def Access_panel_store(user, type_tolk, tolk):
    # Write tolk to user storage
    if (type_tolk == "card"):
        User_tolk.store_card_id(user, tolk)

    if (type_tolk == "code"):
            User_tolk.store_code(user, tolk)
       
    
    
    
def Access_panel_push(cards, codes):
    users = 6
    
    user_cards = []
    user_codes = []
    
    for ID in range(users):
        user_cards.append(User_tolk.get_card_id("user"+str(ID+1)))
    
    for ID in range(users):
        user_codes.append(User_tolk.get_code("user"+str(ID+1)))
    
    message = {
      "id" : "Access_panel",
      "room" : Room.Entry,
      "header" : Header.Acsess_controll,
      "data_String" : {
        "cards" : [
          user_cards[0],
          user_cards[1],
          user_cards[2],
          user_cards[3],
          user_cards[4],
          user_cards[5]
        ],
        "codes" : [
          user_codes[0],
          user_codes[1],
          user_codes[2],
          user_codes[3],
          user_codes[4],
          user_codes[5]
        ]
      }
    }
    
    message_json = json.dumps(message)

    client.publish(topic, message_json)
    



def Outdoor_temp_send(temp):
    
    message = {
       "id" : "Rc_dorm1",
       "room" : Room.Dormroom_1,
       "header" : Header.Room_Controller,
       "data_int" : { "Outdoor_temp": int(temp) }
    }
    
    message_json = json.dumps(message)

    client.publish(topic, message_json)



def on_message(client, userdata, message):
    print("received message =", str(message.payload.decode("utf-8")))
    payload = json.loads(message.payload)
   
  # Handel payload to Hub
    if (payload["id"] == "Hub"):
    	
        
    ###### Handel doorbell messages ######
        if (payload["header"] == Header.Energi_consumption):
            
          # User consumption
            if(payload["data_int"]["user"] >= 0 and payload["data_int"]["user"] < 6):
                consumption = payload["data_String"]["todaysCons"]
                user        = "user"+str(payload["data_int"]["user"]+1)
                room        = payload["room"]
                booked      = ""
                
                if payload["room"] == Room.Bathroom:
                    booked = payload["booked"]
                    
                Power_usage.Handel_power_usages(consumption, "", user, room, booked)
            
          # Room Controller consumption
            elif (payload["data_int"]["user"] >= 6 and payload["data_int"]["user"] < 12):
                consumption = payload["data_String"]["todaysCons"]
                user        = "user"+str(payload["data_int"]["user"]-5)
                room        = "Dorm"
                booked      = ""
                    
                Power_usage.Handel_power_usages(consumption, "", user, room, booked)
                
          # Room Controller consumption not in dorm
            else:
                consumption = payload["data_String"]["todaysCons"]
                
                if   payload["room"] == Room.Livingroom:
                    user    = "Livingroom"
                    
                elif payload["room"] == Room.Bathroom:
                    user    = "Bathroom"
                    
                elif payload["room"] == Room.Kitchen:
                    user    = "Kitchen"
                    
                elif payload["room"] == Room.Entry:
                    user    = "Entry"
                    
                room        = ""
                booked      = ""
                    
                Power_usage.Handel_power_usages(consumption, "", user, room, booked)
            
            
            
    ###### Handel acsess_controll messages ######
        if (payload["header"] == Header.Acsess_controll):
            cards_pull = False
            codes_pull = False
            
            if (payload["data_String"]["request"] == "pull"):
                
                if (payload["data_String"]["type_tolk"] == "card"):
                    cards_pull = True
                    
                if (payload["data_String"]["type_tolk"] == "code"):
                    codes_pull = True
                    
                if (payload["data_String"]["type_tolk"] == "card and code"):
                    cards_pull = True
                    codes_pull = True
                    
                Access_panel_push(cards_pull, codes_pull)
                
            if (payload["data_String"]["request"] == "push"):
                    Access_panel_store(payload["data_String"]["user"], payload["data_String"]["type_tolk"], payload["data_String"]["tolk"])

               



def job_every_1s():
    Outdoor_temp_send(Outdoor_temp.get())




###### Setup ######
User_tolk.setup()
Power_usage.setup()

seconds1 = 1
loop_time = time.time()


############## MQTT #################

client= paho.Client() 

client.on_message = on_message

print("connecting to broker ", broker)
client.connect(broker)

client.reconnect()
print("subscribing ")
client.subscribe("My_home/mqtt") 
time.sleep(2)

# loop
try:
    while True:
        elapsed_time = time.time() - loop_time

        if elapsed_time > seconds1:
            job_every_1s()
            loop_time = time.time()
        
        client.loop()
        

except KeyboardInterrupt:
    client.disconnect()
except Exception as e:
    print("error: ", e)
    client.disconnect()
        
        

    
    
    
    
    

