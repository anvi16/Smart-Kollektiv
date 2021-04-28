import time
import json
import enum
import paho.mqtt.client as paho
import Outdoor_temp

class Header(enum.Enum):
    none = 0x00
    Booking	= 0x01
    Doorbell = 0x02
    Light = 0x03
    Heat = 0x04
    Energi_consumption = 0x05
    Entry_logging = 0x06
    Acsess_controll = 0x07
    Room_Controller = 0x08
    
    


class Room(enum.Enum):
	All = 0x00
	Dormroom_1 = 0x01
	Dormroom_2 = 0x02
	Dormroom_3 = 0x03
	Dormroom_4 = 0x04
	Dormroom_5 = 0x05
	Dormroom_6 = 0x06
	Livingroom = 0x07
	Kitchen	= 0x08
	Bathroom = 0x09
	Entry = 0x0a


id = "Rpi_Hub"
topic = "My_home/mqtt"
broker="broker.hivemq.com"


def Outdoor_temp_send(temp):
    
    message = {
       "id":"All",
       "room":0,  #Room.All,
       "header":8,  #Header.Room_Controller,
       "data_int":{ "Outdoor_temp":temp }
    }
    
    message_json = json.dumps(message)

    client.publish(topic, message_json)


def on_message(client, userdata, message):
    print("received message =", str(message.payload.decode("utf-8")))
    payload = json.loads(message.payload)

	# Handel payload
    if (payload["header"] == Header.Doorbell):
        Doorbell(payload["String_data"]["user"])



client= paho.Client() #create client object client1.on_publish = on_publish #assign function to callback client1.connect(broker,port) #establish connection client1.publish("house/bulb1","on")

######Bind function to callback
client.on_message = on_message
#####

# loop
print("connecting to broker ", broker)
client.connect(broker) #connect
print("subscribing ")
client.subscribe("My_home/mqtt") #subscribe
time.sleep(2)
client.loop_start()

while True:
    client.loop()
    Outdoor_temp_send(10)#Outdoor_temp.get())


def Doorbell(user):
    _user = user

    message = {
		"id": _user,
		"room" : Room.Entry,
		"header" : Header.Doorbell
	}

    message_json = json.dumps(message)

    client.publish(topic, message_json)
    
    
    
    

