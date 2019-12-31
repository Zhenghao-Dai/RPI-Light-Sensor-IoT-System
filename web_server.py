"""EE 250L Lab 11 Final Project

web_server.py: Event logging server.

TODO: List team members here.
Zhenghao Dai 
Brian Chang

TODO: Insert Github repository link here.
https://github.com/usc-ee250-fall2019/finalproj-lucas-brian-11/tree/final/examples/ee250_lab11
"""
from flask import Flask
from flask import jsonify
from flask import request
import json

app = Flask(__name__)
storage=[]

@app.route("/")
def home():
    return "Hello, World!"

#History page
@app.route("/history",methods=['GET'])
def History():
    a=''
    for i in storage:
        a+=str(i)
        a+="<br/>"
    #return the string of the current data
    return a

@app.route('/webserver', methods=['POST'])
def post_callback():
    #get json
    payload = request.get_json()
    storage.append(payload['time']+": "+payload['event']+" at "+payload['room'])
    print(payload)
    return "sucess" 

if __name__ == "__main__":
    app.run()