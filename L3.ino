#include <LWiFi.h>
#include <WiFiUdp.h>
#include <TLSClient.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <PubSubClient.h>
#include "LDHT.h"
#define DHTPIN 2          // what pin we're connected to
#define DHTTYPE DHT22     // using DHT11 sensor

LDHT dht(DHTPIN, DHTTYPE);

#define led_power_PORT 14
#define fan_power_PORT 15
float tempC = 0.0, Humi = 0.0;
char readcharbuffer[20];
int readbuffersize;
char temp_input;  

// Update these with values suitable for your network.

char ssid[] = "CUTY_LAB_WIFI";     // your network SSID (name)
char pass[] = "CUTY_2021";  // your network password
int status  = WL_IDLE_STATUS;    // the Wifi radio's status
int heartbeat_timer = 5000;      //heatbeat timer, unit:msec
int raw_timer = 30000;         //heatbeat timer, unit:msec
char mqttServer[]     = "iot.cht.com.tw";
const char* clientId       = "iohjo";
const char* userpass     = "DK4H94FWAG03WTX4W3";   //your api key
char subscribeLEDTopic[] = "/v1/device/27841421086/sensor/relay/csv";

long previousHBTime = 0;          // previous HB previous time
long previousRawTime = 0;         // previous Raw previous time
int led_value = 0;                // led value(1:high,0:off)

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  char *p = (char*)payload;
  char *str;
  int element_cnt = 0;
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println((char)payload[43]);
    if ((char)payload[43] == '0'){
          Serial.println("Turn off LED");
          digitalWrite(led_power_PORT, LOW);    // turn the LED off by making the voltage LOW
          digitalWrite(fan_power_PORT, LOW);    // turn the Fan off by making the voltage LOW
        }else if ((char)payload[43] == '1'){
          Serial.println("Turn on LED");
          digitalWrite(led_power_PORT, HIGH);   // turn the LED on (HIGH is the voltage level)
          digitalWrite(fan_power_PORT, HIGH);   // turn the Fan on (HIGH is the voltage level)
        }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId,userpass,userpass)) {
      Serial.println("connected");
       // ... and resubscribe
      client.subscribe(subscribeLEDTopic);
      Serial.print("Subscribe LED topic is:");
      Serial.println(subscribeLEDTopic);
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

	dht.begin();

	Serial.print(DHTTYPE);
	Serial.println(" test!");
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
   
    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
   // Allow the hardware to sort itself out
  delay(1500);
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

}

void loop()
{
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (dht.read())
	{
		tempC = dht.readTemperature();
		Humi = dht.readHumidity();


		Serial.println("------------------------------");
		Serial.print("Temperature Celcius = ");
		Serial.print(dht.readTemperature());
		Serial.println("C");

		Serial.print("Temperature Fahrenheit = ");
		Serial.print(dht.readTemperature(false));
		Serial.println("F");

		Serial.print("Humidity = ");
		Serial.print(dht.readHumidity());
		Serial.println("%");

		Serial.print("HeatIndex = ");
		Serial.print(dht.readHeatIndex(tempC, Humi));
		Serial.println("C");

		Serial.print("DewPoint = ");
		Serial.print(dht.readDewPoint(tempC, Humi));
		Serial.println("C");

		Serial.println("Ready to Send");
		Serial.print("AT+DTX=11,\"T");
		Serial.print(dht.readHeatIndex(tempC, Humi));
		Serial.print(dht.readHumidity());
		Serial.println("\"");
		

		Serial1.print("AT+DTX=11,\"T");
		Serial1.print(dht.readHeatIndex(tempC, Humi));
		Serial1.print(dht.readHumidity());
		Serial1.println("\"");


		//Serial1.println("AT+DTX=11,\"12345ABCdef\"");


	}
	delay(1000);

	/*
  Serial.println("Ready to Send");
  Serial1.println("AT+DTX=11,\"12345ABCdef\"");
  delay(1000);*/
  readbuffersize = Serial1.available();
  while(readbuffersize){
    temp_input = Serial1.read();
    Serial.print(temp_input);
    readbuffersize--;
 }
 delay(7000);
}
