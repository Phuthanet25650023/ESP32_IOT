//////////////////////////////// Receive MQTT ///////////////////////////////////////////

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // เพิ่มไลบรารี ArduinoJson
#define topic1 "MIIX/006/1"
const char* ssid = "Elite_Ultimate_2.4G";
const char* password = "24776996";
const char* mqtt_server = "broker.emqx.io";
int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // แปลง payload เป็น String
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // แปลง JSON
  StaticJsonDocument<200> doc;  // กำหนดขนาด document ให้พอดีข้อมูล
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // ดึงค่าจาก JSON object
  int value1 = doc["value1"];
  int value2 = doc["value2"];
  int device = doc["device"];

  Serial.print("value1 = ");
  Serial.println(value1);
  Serial.print("value2 = ");
  Serial.println(value2);
  Serial.print("device = ");
  Serial.println(device);

  // นำค่าที่ได้ไปใช้ต่อ เช่น ควบคุมอุปกรณ์หรือแสดงผล
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic1);  // subscribe topic ที่ต้องการ
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// LAB Send ///////////////////////////////////////////
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define topic1 "MIIX/006/1"
const char* ssid = "Elite_Ultimate_2.4G";
const char* password = "24776996";
const char* mqtt_server = "broker.emqx.io";
int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastSend = 0;
const long sendInterval = 5000;  // ส่งข้อมูลทุก 5 วินาที

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ถ้าต้องการ subscribe topic อื่น ๆ ก็ทำตรงนี้
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
  // ไม่ต้องใช้ callback เพราะไม่ subscribe ในโค้ดนี้
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ส่งข้อมูลทุก 5 วินาที
  if (millis() - lastSend > sendInterval) {
    lastSend = millis();

    // สร้าง JSON Document
    StaticJsonDocument<200> doc;
    doc["device"] = 1;                 // รหัสอุปกรณ์
    doc["value1"] = random(0, 101);   // ส่งค่า value1 แบบสุ่ม 0-100
    doc["value2"] = random(0, 201);   // ส่งค่า value2 แบบสุ่ม 0-200

    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    // ส่งข้อมูลไปยัง topic1
    if (client.publish(topic1, buffer, n)) {
      Serial.print("Published message: ");
      Serial.println(buffer);
    } else {
      Serial.println("Failed to publish message");
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////
