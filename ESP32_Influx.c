/////////////////////////////////// MING.ino ///////////////////////////////////////////////////////////      
// MING Stack 
// @knandas Github https://github.com/knandas


#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const char* ssid = "ELEC302";  //ELEC302_plus
const char* password = "elec1234";

char *mqttServer = "broker.emqx.io";
int mqttPort = 1883; 
#define topic1 "MIIX/xxx/1" ///////edit topic///// 
#define topic2 "MIIX/xxx/2"
#define thisDevice 1

#define led 2
int value = 0;
unsigned long lastMillis=0;
unsigned int delayTime =5000; 
bool dir=0;




void setupMQTT() 
{
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}

void reconnect() 
{
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) 
  {
    Serial.println("Reconnecting to MQTT Broker..");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) 
    {
      Serial.println("Connected.");
      // subscribe to topic
      mqttClient.subscribe(topic2);   //in topic
    }
    delay(1000);      
  }
}

void setup() 
{
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.println("Connected to Wi-Fi");
  
  setupMQTT();
}

void loop() 
{
 if (!mqttClient.connected()) reconnect();
 mqttClient.loop();

 if(millis()-lastMillis>delayTime)
 {
  if(dir==0)
  {
    value++;
    if(value==100) dir=1;
    Serial.println(value);
  }else
  {
    value--;
    if(value==0) dir=0;
    Serial.println(value);
  }
 
  sendMQTT();   
  
  digitalWrite(led,HIGH); delay(100); digitalWrite(led,LOW);
  lastMillis=millis();
 }  
 delay(2);

}


void sendMQTT()
{
  StaticJsonDocument<128> doc;
  doc["device"] = thisDevice;
  doc["value1"] = value;
  doc["value2"] = value/2;
 

  char out[128];
  int b =serializeJson(doc, out);
  //Serial.print("bytes = ");
  //Serial.println(b,DEC);
  //Serial.println(out);
  mqttClient.publish(topic1, out);
  delay(10);
}



void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char str[length+1];
  int i=0;
  for (i=0;i<length;i++) 
  {
    Serial.print((char)payload[i]);
    str[i]=(char)payload[i];
  }
  str[i] = 0; // Null termination
  String payload2=str;
  Serial.println(payload2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////  
