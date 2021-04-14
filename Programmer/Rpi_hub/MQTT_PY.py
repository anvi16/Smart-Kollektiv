import time
import json
import enum
import paho.mqtt.client as paho

class Header(enum.Enum):
	none				= 0x00
	Booking				= 0x01
	Doorbell			= 0x02
	Light				= 0x03
	Heat				= 0x04
	Energi_consumption	= 0x05
	Entry_logging		= 0x06
	Acsess_controll		= 0x07

class Room(enum.Enum):
	NaN = 0x00,
	Dormroom_1		= 0x01
	Dormroom_2		= 0x02
	Dormroom_3		= 0x03
	Dormroom_4		= 0x04
	Dormroom_5		= 0x05
	Dormroom_6		= 0x06
	Livingroom		= 0x07
	Kitchen			= 0x08
	Bathroom		= 0x09
	Entry			= 0x0a


id = "Rpi_Hub"
topic = "My_home/mqtt"
broker="broker.hivemq.com"


def on_message(client, userdata, message):
    time.sleep(1)
    print("received message =", str(message.payload.decode("utf-8")))
    payload = json.loads(message.payload)

	# Handel payload
    if (payload["header"] == Header.Doorbell):
        Doorbell(payload["String_data"]["user"])

    if (payload["header"] == Header.Booking):
        _user = payload["String_data"]["User"]
        _room = payload["room"]
        _day =  payload["int_data"]["Day"]
        _time = payload["int_data"]["Time"]
        Booking_handeler(_user, _day, _time)


client= paho.Client(id) #create client object client1.on_publish = on_publish #assign function to callback client1.connect(broker,port) #establish connection client1.publish("house/bulb1","on")

######Bind function to callback
client.on_message = on_message
#####

# loop
print("connecting to broker ", broker)
client.connect(broker) #connect
client.loop_start() #start loop to process received messages
print("subscribing ")
client.subscribe("My_home/mqtt") #subscribe
time.sleep(2)

client.loop_forever()




def Doorbell():
    _user = user

    message = {
		"id": client,
		"room" : Room.NaN,
		"header" : Header.Doorbell
	}

    message_json = json.dumps(message)

    client.publish(topic, message_json)