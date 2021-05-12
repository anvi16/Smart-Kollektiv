import time
import json
import enum
from datetime import datetime
import paho.mqtt.client as paho
from scripts import User_tolk
from scripts import Power_usage
from scripts import Openweathermap as Weather
from scripts import CoT

###### Declare variables ######
id_mqtt = "Hub"
topic = "My_home/mqtt"
broker = "localhost"

### Timers ###
seconds1 = 1
loop_time_1sec = time.time()

minutes15 = 15 * 60
loop_time_15min = time.time()
index_loop = 0
loop_push_power = False


class Header(enum.IntEnum):
    none = 0x00
    Booking = 0x01
    Doorbell = 0x02
    Light = 0x03
    Heat = 0x04
    Energi_consumption = 0x05
    Entry_logging = 0x06
    Acsess_controll = 0x07
    Room_Controller = 0x08


class Room(enum.IntEnum):
    All = 0x00
    Dormroom_1 = 0x01
    Dormroom_2 = 0x02
    Dormroom_3 = 0x03
    Dormroom_4 = 0x04
    Dormroom_5 = 0x05
    Dormroom_6 = 0x06
    Livingroom = 0x07
    Kitchen = 0x08
    Bathroom = 0x09
    Entry = 0x0a


def Access_panel_store(user, type_tolk, tolk):
    # Write tolk to user storage
    if type_tolk == "card":
        User_tolk.store_card_id(user, tolk)

    if type_tolk == "code":
        User_tolk.store_code(user, tolk)


def Access_panel_push(cards, codes):
    users = 6

    user_cards = []
    user_codes = []

    for ID in range(users):
        user_cards.append(User_tolk.get_card_id("user" + str(ID + 1)))

    for ID in range(users):
        user_codes.append(User_tolk.get_code("user" + str(ID + 1)))

    message = {
        "id": "Access_panel",
        "room": Room.Entry,
        "header": Header.Acsess_controll,
        "data_String": {
            "cards": [
                user_cards[0],
                user_cards[1],
                user_cards[2],
                user_cards[3],
                user_cards[4],
                user_cards[5]
            ],
            "codes": [
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
        "id": "Rc_dorm1",
        "room": Room.Dormroom_1,
        "header": Header.Room_Controller,
        "data_int": {"Outdoor_temp": int(temp)}
    }

    message_json = json.dumps(message)

    client.publish(topic, message_json)


def Sun_panel_production_store(production):
    Power_usage.Handel_power_usages(production, "Sun panel")


def Push_power_consumption():
    global index_loop
    global loop_push_power

    if index_loop == 0: CoT.push_payload(28766, int(Power_usage.User_avreage("Sun panel")))
    if index_loop == 1: CoT.push_payload(15415, int(Power_usage.User_avreage("user1")))
    if index_loop == 2: CoT.push_payload(20464, int(Power_usage.User_avreage("user2")))
    if index_loop == 3: CoT.push_payload(14718, int(Power_usage.User_avreage("user3")))
    if index_loop == 4: CoT.push_payload(13867, int(Power_usage.User_avreage("user4")))
    if index_loop == 5: CoT.push_payload(31659, int(Power_usage.User_avreage("user5")))
    if index_loop == 6: CoT.push_payload(27141, int(Power_usage.User_avreage("user6")))
    if index_loop == 7:
        print("Pushed to CoT")
        index_loop = 0
        loop_push_power = False

    else:
        index_loop += 1


def on_message(client, userdata, message):
    # print("received message =", str(message.payload.decode("utf-8")))

    # Make sure payload is JSON 
    try:
        payload = json.loads(message.payload)
    except ValueError as e:
        print("ERROR: " + e)
        return

    # Handel payload to Hub
    if payload["id"] == id_mqtt:

        ###### Handel doorbell messages ######
        if payload["header"] == Header.Energi_consumption:

            # User consumption
            if 0 <= payload["data_int"]["user"] < 6:
                consumption = payload["data_String"]["todaysCons"]
                user = "user" + str(payload["data_int"]["user"] + 1)
                room = payload["room"]
                booked = ""

                if payload["room"] == Room.Bathroom:
                    booked = payload["booked"]

                Power_usage.Handel_power_usages(consumption, user, room, booked)

            # Room Controller consumption
            elif 6 <= payload["data_int"]["user"] < 12:
                consumption = payload["data_String"]["todaysCons"]
                user = "user" + str(payload["data_int"]["user"] - 5)
                room = "Dorm"
                booked = ""

                Power_usage.Handel_power_usages(consumption, user, room, booked)

            # Room Controller consumption not in dorm
            else:
                consumption = payload["data_String"]["todaysCons"]

                if payload["room"] == Room.Livingroom:
                    user = "Livingroom"

                elif payload["room"] == Room.Bathroom:
                    user = "Bathroom"

                elif payload["room"] == Room.Kitchen:
                    user = "Kitchen"

                elif payload["room"] == Room.Entry:
                    user = "Entry"

                else:
                    return

                room = ""
                booked = ""

                Power_usage.Handel_power_usages(consumption, user, room, booked)

        ###### Handel access control messages ######
        if payload["header"] == Header.Acsess_controll:
            cards_pull = False
            codes_pull = False

            if payload["data_String"]["request"] == "pull":

                if payload["data_String"]["type_tolk"] == "card":
                    cards_pull = True

                if payload["data_String"]["type_tolk"] == "code":
                    codes_pull = True

                if payload["data_String"]["type_tolk"] == "card and code":
                    cards_pull = True
                    codes_pull = True

                Access_panel_push(cards_pull, codes_pull)

            if payload["data_String"]["request"] == "push":
                Access_panel_store(payload["data_String"]["user"], payload["data_String"]["type_tolk"],
                                   payload["data_String"]["tolk"])


def job_every_1s():
    Outdoor_temp_send(Weather.get_temp())


def job_every_15min():
    Sun_panel_production_store(Weather.get_sun_panel_production())


###### Setup ######
print("Startup: " + str(datetime.today()))

User_tolk.setup()
Power_usage.setup()

############## MQTT #################

client = paho.Client()

client.on_message = on_message

print("connecting to broker ", broker)
client.connect(broker)

client.reconnect()
print("subscribing ")
client.subscribe(topic)
time.sleep(1)


# Loop
while True:
    try:
        elapsed_time = time.time() - loop_time_1sec

        # Run every 1sec
        if elapsed_time > seconds1:
            loop_time_1sec = time.time()

            job_every_1s()

        elapsed_time = time.time() - loop_time_15min

        # Run every 10min
        if elapsed_time > minutes15:
            loop_time_15min = time.time()

            job_every_15min()
            loop_push_power = True

        if loop_push_power:
            Push_power_consumption()

        client.loop()

    except KeyboardInterrupt:
        client.disconnect()
        break
    except Exception as e:
        print("ERROR: ", e)
        
        print("Reconecting")
        client.reconnect()
        
        print("subscribing ")
        client.subscribe(topic)
