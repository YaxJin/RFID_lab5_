import paho.mqtt.client as mqtt
import json
import urllib.parse
import urllib.request
import paho.mqtt.publish as publish
import time

# The callback for when the client receives a CONNACK bbresponse from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.username_pw_set("nthuhsnl_lora","89957463")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):

        my_json = msg.payload.decode('utf8').replace("'", '"') #my_json type is str
        if (my_json=='world'):
            w=my_json

        else:
            a=my_json#a type is str
            v=a.strip('[]')
            if(v[2:5]=='cha'):
                if((v[95:111]!='0000f835ddfd5639')):
                    data1=json.loads(v)#data1 type is dic #data1 is all data
                    indata=data1["data"]
                    #print(type(indata))
                    inmacaddress=data1["macAddr"]
                    
                    if(inmacaddress=='00000000070101d7'): 
                        hexto_ascil = bytes.fromhex(indata)
                        bytetostr = str(hexto_ascil)
                        #print(bytetostr)
                        if(bytetostr[2]=='T'):
                            airtemp = bytetostr[3:8]
                            print("AIRTEMP")
                            print(airtemp)
                            airhum = bytetostr[8:13]
                            print("AIRHUM")
                            print(airhum)
                            inairtemp = str(airtemp)
                            inairhum = str(airhum)
                            whattime=time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
                            host = "iot.cht.com.tw"
                            # If broker asks user/password.
                            auth = {'username': "DK4H94FWAG03WTX4W3", 'password': "DK4H94FWAG03WTX4W3"}
                            topic = "/v1/device/27841421086/rawdata"
                            payload='[{ "id": "airtemp", "time": "'+ whattime +'","value": [ "'+ inairtemp +'"]},{"id": "airhum", "time": "'+ whattime +'","value": ["'+ inairhum +'"]}]'
                            client_id = "4b2ca1c9c5cb8fe715c36f"
                            publish.single(topic,payload, qos=0, hostname=host,auth=auth)
                            print(whattime)
                            print('-------------------------end------------------------------------')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("104.199.215.165", 1883, 60)
client.loop_forever()

