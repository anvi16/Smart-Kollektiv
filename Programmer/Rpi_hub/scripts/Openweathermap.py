import json  ## Import the necessary libraries
import requests

## openweathermap
API_key = '9829833eacd6faa924391ab12c32f53b'
lat = '10.3951'
lon = '10.3951'
url = 'http://api.openweathermap.org/data/2.5/weather?q=Trondheim,NO&APPID=9829833eacd6faa924391ab12c32f53b'


def get_temp():
    # Update the weatherforecast. This will request the data from openweathermap.
    try:
        response = requests.get(url)
        weatherdata = json.loads(response.text)
    except Exception as e:
        print("ERROR: " + str(e))
        return 0


    weatherdata.update()
    uteTemp = weatherdata["main"]["temp"]
    uteTemp_celsius = (uteTemp - 273)
    # print (uteTemp_celsius)
    
    return round(uteTemp_celsius)


def get_sun_panel_production():
    # Update the weatherforecast. This will request the data from openweathermap.
    try:
        response = requests.get(url)
        weatherdata = json.loads(response.text)
    except Exception as e:
        print("ERROR: " + str(e))
        return 0

    weatherdata.update()
    sunrise = weatherdata["sys"]["sunrise"]
    sunset = weatherdata["sys"]['sunset']
    clouds = weatherdata["clouds"]["all"]
    
    sun_effiency = (105 - clouds) / 100
    sun_hours = (sunset - sunrise) / 3600
    
    # print (sun_effiency)
    
    power_production = 0.212 * 150 * sun_effiency * sun_hours  ## 0.212 is the panasonic solar panel efficiency.
    ## 150 is the area that the solar panels cover. 
    return round(power_production)



if __name__ == "__main__":
    print(get_sun_panel_production())






