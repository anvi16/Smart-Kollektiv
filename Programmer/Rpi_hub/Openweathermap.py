import json                                             ## Import the necessary libraries
import requests 


## openweathermap
API_key= '9829833eacd6faa924391ab12c32f53b'
lat = '10.3951'
lon= '10.3951'
url= 'http://api.openweathermap.org/data/2.5/weather?q=Trondheim,NO&APPID=9829833eacd6faa924391ab12c32f53b'
response = requests.get(url)
weatherdata = json.loads(response.text)


def get():                                              #Update the weatherforecast. This will request the data from openweathermap.
    weatherdata.update()
    interval= weatherdata.data.intervals[30]
    return interval


uteTemp = weatherdata["main"]["temp"]
uteTemp_celsius = (uteTemp - 273)
print (uteTemp_celsius)


sunrise = weatherdata["sys"]["sunrise"]
sunset  = weatherdata["sys"]['sunset']
clouds = weatherdata["clouds"]["all"]


sun_effiency = (105 - clouds)/100
sun_hours = (sunset - sunrise)/3600

print (sun_effiency)
    
power_production = 0.212*150*sun_effiency*sun_hours   ## 0.212 is the panasonic solar panel efficiency.
                                                      ## 150 is the area that the solar panels cover. 
print(str(power_production) + " Watt per day")

