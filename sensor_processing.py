"""EE 250L Lab 11 Final Project

sensor_processing.py: Sensor data processing.

TODO: List team members here.
Zhenghao Dai 
Brian Chang

TODO: Insert Github repository link here.
https://github.com/usc-ee250-fall2019/finalproj-lucas-brian-11/tree/final/examples/ee250_lab11
"""

import paho.mqtt.client as mqtt
import time
from flask import Flask
from flask import jsonify
from flask import request
from datetime import datetime
import requests
import json
#flag 0 for previous dark
#flag 1 for previous light
event_flag=-1

light_threshold=50

def send_post(message,status):
    data={
        'time':str(datetime.today().strftime('%Y-%m-%d %H:%M:%S')),
        'event':status,
        'room':'vhe_205'
    }
    header={
        'Content-Type':'application/json',
        'Authorization':None
    }

    #post data to  webserver using http 
    response = requests.post('http://localhost:5000/webserver', headers=header,data=json.dumps(data) )

def custom_callback(client, userdata, message):
    # the third argument is 'message' here unlike 'msg' in on_message
    global event_flag
    adc_str =str(message.payload, "utf-8")
    adc_sample = int(adc_str)
    print(adc_sample)
    if adc_sample>light_threshold and event_flag !=1:
        send_post(adc_sample,"LIGHTS_ON")
        #set flag to current status
        event_flag=1
    elif adc_sample<light_threshold and event_flag !=0:
        send_post(adc_sample,"LIGHTS_OFF")
        event_flag=0


def on_connect(client, userdata, flags, rc):
    print("Connected to server (i.e., broker) with result code "+str(rc))

    # subscribe to the ultrasonic ranger topic here
    client.subscribe("zhenghad/light")
    client.message_callback_add(
        "zhenghad/light", custom_callback)



if __name__ == '__main__':
    # this section is covered in publisher_and_subscriber_example.py
    
    
    client = mqtt.Client()  
    client.on_connect = on_connect
    client.custom_callback = custom_callback
    client.connect(host="eclipse.usc.edu", port=11000, keepalive=60)
    client.loop_start()
    
    # check every one sec
    while True:
        

        time.sleep(1)
