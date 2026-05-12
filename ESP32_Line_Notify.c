#https://manager.line.biz/
#https://developers.line.biz/
### https://www.imiconsystem.com/esp32-with-line-messaging-api/?srsltid=AfmBOoo-ySh40F-b_1P1iTI1HwPpgXoPcp47Gw0DSB4QBmL0gttn0MDb #####
#################################################################### LAB 2 #################################################################
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  

const char* ssid = "Elite_Ultimate_2.4G";
const char* password = "24776996";
const char* ACCESS_TOKEN = "hjlXUd6E4YnlVxCM1AsFK2SS+fgVVu1cuMtB1PF7S1uDQHTNIN/s4qPT0uHZ2oqCMO4QS/XaiNj/36dPCLVIg5c+C3EYDwjHIHTel8cB0PlF2n4d3VyZoPtGbCKzSsqMHZQPg5o58DeaFpPfz3J/PQdB04t89/1O/w1cDnyilFU=";  // จาก LINE Developers
const char* USER_ID = "U097413564a070c32b48472ef1233fc7f";  // User ID ที่จะส่งไป (Your User ID)

void sendLineImage(String imageUrl); // เพิ่ม function ส่งข้อความ

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");

  sendLineMessage("สวัสดี! จาก ESP32");
  sendLineImage("https://upload.wikimedia.org/wikipedia/commons/4/47/PNG_transparency_demonstration_1.png");
  sendLineSticker("446", "1988");
}

void loop() 
{

}

void sendLineMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.line.me/v2/bot/message/push");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(ACCESS_TOKEN));

    // สร้าง JSON payload
    DynamicJsonDocument doc(1024);
    doc["to"] = USER_ID;
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject msg = messages.createNestedObject();
    msg["type"] = "text";
    msg["text"] = message;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      Serial.printf("ส่งสำเร็จ! Code: %d\n", httpCode);
      if (httpCode == 200) {
        String response = http.getString();
        Serial.println(response);
      }
    } else {
      Serial.println("ส่งล้มเหลว");
    }
    http.end();
  }
}
#################################################################### LAB 3 #################################################################
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

const char* ssid = "Elite_Ultimate_2.4G";
const char* password = "24776996";
const char* ACCESS_TOKEN = "hjlXUd6E4YnlVxCM1AsFK2SS+fgVVu1cuMtB1PF7S1uDQHTNIN/s4qPT0uHZ2oqCMO4QS/XaiNj/36dPCLVIg5c+C3EYDwjHIHTel8cB0PlF2n4d3VyZoPtGbCKzSsqMHZQPg5o58DeaFpPfz3J/PQdB04t89/1O/w1cDnyilFU=";  // จาก LINE Developers
const char* USER_ID = "U097413564a070c32b48472ef1233fc7f";  // User ID ที่จะส่งไป (Your User ID)

void sendLineImage(String imageUrl);  // เพิ่ม function ส่งรูป
void sendLineMessage(String message);
void sendLineSticker(String packageId, String stickerId);  // เพิ่ม function ส่งสติกเกอร์

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");

  sendLineMessage("สวัสดี! จาก ESP32");
  sendLineImage("https://upload.wikimedia.org/wikipedia/commons/4/47/PNG_transparency_demonstration_1.png");
  sendLineSticker("446", "1988");
}

void loop() 
{

}

void sendLineMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.line.me/v2/bot/message/push");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(ACCESS_TOKEN));

    // สร้าง JSON payload
    DynamicJsonDocument doc(1024);
    doc["to"] = USER_ID;
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject msg = messages.createNestedObject();
    msg["type"] = "text";
    msg["text"] = message;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      Serial.printf("ส่งสำเร็จ! Code: %d\n", httpCode);
      if (httpCode == 200) {
        String response = http.getString();
        Serial.println(response);
      }
    } else {
      Serial.println("ส่งล้มเหลว");
    }
    http.end();
  }
}

void sendLineImage(String imageUrl) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.line.me/v2/bot/message/push");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(ACCESS_TOKEN));

    // สร้าง JSON payload สำหรับรูปภาพ
    DynamicJsonDocument doc(1024);
    doc["to"] = USER_ID;
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject msg = messages.createNestedObject();
    
    msg["type"] = "image";
    msg["originalContentUrl"] = imageUrl; // URL รูปจริง
    msg["previewImageUrl"] = imageUrl;    // URL รูปพรีวิว (ใช้ตัวเดียวกันได้ถ้าไม่ใหญ่เกินไป)

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      Serial.printf("ส่งรูปภาพสำเร็จ! Code: %d\n", httpCode);
    } else {
      Serial.printf("ส่งรูปภาพล้มเหลว Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void sendLineSticker(String packageId, String stickerId) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.line.me/v2/bot/message/push");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(ACCESS_TOKEN));

    // สร้าง JSON payload สำหรับ Sticker
    DynamicJsonDocument doc(1024);
    doc["to"] = USER_ID;
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject msg = messages.createNestedObject();
    
    msg["type"] = "sticker";
    msg["packageId"] = packageId;
    msg["stickerId"] = stickerId;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      Serial.printf("ส่งสติกเกอร์สำเร็จ! Code: %d\n", httpCode);
    } else {
      Serial.println("ส่งสติกเกอร์ล้มเหลว");
    }
    http.end();
  }
}
