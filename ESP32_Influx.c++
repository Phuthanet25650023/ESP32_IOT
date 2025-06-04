/////////////////////////////////// MING.ino ///////////////////////////////////////////////////////////      
// MING Stack 
// @knandas Github https://github.com/knandas

#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

// สร้าง client สำหรับใช้งาน WiFi และ MQTT
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// กำหนด WiFi ที่จะเชื่อมต่อ
const char* ssid = "xxx";       // <-- เปลี่ยนเป็นชื่อ WiFi
const char* password = "xxx";   // <-- เปลี่ยนเป็นรหัสผ่าน WiFi

// กำหนด MQTT Broker
char *mqttServer = "broker.emqx.io";
int mqttPort = 1883; 

// กำหนด MQTT Topic (ให้แก้ xxx เป็นรหัส 3 ตัวท้ายของตนเอง)
#define topic1 "MIIX/xxx/1"
#define topic2 "MIIX/xxx/2"   // ยังไม่ได้ใช้งาน

#define thisDevice 1           // รหัสของอุปกรณ์
#define led 2                  // ขา GPIO ที่ต่อกับ LED

// ตัวแปรหลัก
int value = 0;
unsigned long lastMillis = 0;
unsigned int delayTime = 5000;  // เวลาหน่วงรอบละ 5 วินาที
bool dir = 0;

////////////////////////////////////////////////////////////////////////////////////
// ฟังก์ชันตั้งค่า MQTT
void setupMQTT() 
{
  mqttClient.setServer(mqttServer, mqttPort);  // ตั้งค่า server และ port
  mqttClient.setCallback(callback);            // กำหนด callback เมื่อมี message เข้า
}

// ฟังก์ชันเชื่อมต่อ MQTT Broker ใหม่เมื่อหลุด
void reconnect() 
{
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) 
  {
    Serial.println("Reconnecting to MQTT Broker..");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX); // สร้าง client id แบบสุ่ม

    if (mqttClient.connect(clientId.c_str())) 
    {
      Serial.println("Connected.");
      mqttClient.subscribe(topic2);  // subscribe หัวข้อ (ยังไม่ได้ใช้งาน)
    }
    delay(1000);      
  }
}

////////////////////////////////////////////////////////////////////////////////////
// ฟังก์ชันเริ่มต้น (setup)
void setup() 
{
  pinMode(led, OUTPUT);        // ตั้งขา LED เป็น output
  digitalWrite(led, LOW);      // ปิด LED เริ่มต้น
  Serial.begin(115200);        // เริ่ม serial monitor
  
  WiFi.begin(ssid, password);  // เชื่อมต่อ WiFi
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.println("Connected to Wi-Fi");
  
  setupMQTT();                 // ตั้งค่า MQTT
}

////////////////////////////////////////////////////////////////////////////////////
// ฟังก์ชันหลัก (loop)
void loop() 
{
  if (!mqttClient.connected()) reconnect(); // ถ้ายังไม่เชื่อม MQTT ให้ reconnect
  mqttClient.loop();                        // รอ message จาก MQTT

  // เช็คเวลาผ่านไปเกิน delayTime หรือยัง (5 วินาที)
  if (millis() - lastMillis > delayTime)
  {
    // เปลี่ยนค่าขึ้นลงระหว่าง 0-100
    if (dir == 0)
    {
      value++;
      if (value == 100) dir = 1;
    }
    else
    {
      value--;
      if (value == 0) dir = 0;
    }

    Serial.println(value);  // แสดงค่า value ใน serial monitor
    
    sendMQTT();             // ส่งค่าไปยัง MQTT

    // กระพริบ LED
    digitalWrite(led, HIGH); 
    delay(100); 
    digitalWrite(led, LOW);

    lastMillis = millis();  // รีเซ็ตเวลา
  }

  delay(2);  // ลดภาระ CPU
}

////////////////////////////////////////////////////////////////////////////////////
// ฟังก์ชันส่งข้อมูล MQTT
void sendMQTT()
{
  StaticJsonDocument<128> doc;

  doc["device"] = thisDevice;
  doc["value1"] = value;
  doc["value2"] = random(0, 200);   // กำหนด value2 ให้สุ่มค่าระหว่าง 0-199

  char out[128];
  serializeJson(doc, out);
  mqttClient.publish(topic1, out);  // ส่งไปยัง MQTT topic
  delay(10);
}

////////////////////////////////////////////////////////////////////////////////////
// ฟังก์ชัน callback เมื่อได้รับ message MQTT
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char str[length + 1];
  int i = 0;

  for (i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
    str[i] = (char)payload[i];
  }

  str[i] = 0; // สิ้นสุด string
  String payload2 = str;
  Serial.println(payload2);
}
