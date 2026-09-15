#https://manager.line.biz/
#https://developers.line.biz/
### https://www.imiconsystem.com/esp32-with-line-messaging-api/?srsltid=AfmBOoo-ySh40F-b_1P1iTI1HwPpgXoPcp47Gw0DSB4QBmL0gttn0MDb #####
#################################################################### LAB 1 #################################################################
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

const char* ssid = "Elite_Ultimate_2.4G";
const char* password = ""********";";
const char* ACCESS_TOKEN = ""********";";  // จาก LINE Developers
const char* USER_ID = ""********";";  // User ID ที่จะส่งไป (Your User ID)

void sendLineMessage(String message);// Function ส่งข้อความ


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");

  sendLineMessage("สวัสดี! จาก ESP32");
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
#################################################################### LAB 2 #################################################################
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  

const char* ssid = "Elite_Ultimate_2.4G";
const char* password = ""********";";
const char* ACCESS_TOKEN = ""********";";  // จาก LINE Developers
const char* USER_ID = ""********";";  // User ID ที่จะส่งไป (Your User ID)

void sendLineImage(String imageUrl); // เพิ่ม function ส่งรูป

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");


  sendLineImage("https://upload.wikimedia.org/wikipedia/commons/4/47/PNG_transparency_demonstration_1.png");
 
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
const char* password = ""********";";
const char* ACCESS_TOKEN = ""********";";  // จาก LINE Developers
const char* USER_ID = ""********";";  // User ID ที่จะส่งไป (Your User ID)


void sendLineSticker(String packageId, String stickerId);  // เพิ่ม function ส่งสติกเกอร์

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");

  sendLineSticker("446", "1988");
}

void loop() 
{

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



////////////////////////////////////////////// LINE_SERIAL_PORT ///////////////////////////////

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Elite_Ultimate_2.4G";
const char* password = ""********";";
const char* ACCESS_TOKEN = "********";  // จาก LINE Developers
const char* USER_ID = ""********";";  // User ID ที่จะส่งไป (Your User ID)

const size_t MAX_LINE_LEN = 1000;  // ความยาวข้อความสูงสุดที่รับจาก Serial (LINE รับได้ถึง 5000 ตัวอักษร)
String serialBuffer = "";          // เก็บตัวอักษรที่รับจาก Serial จนกว่าจะเจอ Enter

void connectWiFi();
bool sendLineText(const String& text);                        // เพิ่ม function ส่งข้อความ
bool sendLineSticker(String packageId, String stickerId);     // เพิ่ม function ส่งสติกเกอร์
bool pushLineMessage(JsonDocument& doc);
void handleSerialLine(String line);

void setup() {
  Serial.begin(115200);
  connectWiFi();

  sendLineSticker("446", "1988");

  Serial.println("พิมพ์ข้อความใน Serial Monitor แล้วกด Enter เพื่อส่งเข้า LINE");
  Serial.println("ส่งสติกเกอร์: sticker <packageId> <stickerId>  เช่น sticker 446 1988");
}

void loop()
{
  // อ่าน Serial แบบไม่บล็อก เก็บจนกว่าจะเจอ '\n' (ตั้ง Serial Monitor เป็น New Line)
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      handleSerialLine(serialBuffer);
      serialBuffer = "";
    } else if (c != '\r') {
      if (serialBuffer.length() < MAX_LINE_LEN) {
        serialBuffer += c;
      }
    }
  }

  // ถ้า WiFi หลุด ให้ต่อใหม่
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi หลุด กำลังเชื่อมต่อใหม่...");
    connectWiFi();
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void handleSerialLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  // คำสั่งส่งสติกเกอร์: sticker <packageId> <stickerId>
  if (line.startsWith("sticker ")) {
    String args = line.substring(8);
    args.trim();
    int space = args.indexOf(' ');
    if (space > 0) {
      String packageId = args.substring(0, space);
      String stickerId = args.substring(space + 1);
      stickerId.trim();
      sendLineSticker(packageId, stickerId);
    } else {
      Serial.println("รูปแบบไม่ถูกต้อง ใช้: sticker <packageId> <stickerId>");
    }
    return;
  }

  Serial.print("กำลังส่ง: ");
  Serial.println(line);
  sendLineText(line);
}

bool sendLineText(const String& text) {
  // สร้าง JSON payload สำหรับข้อความ
  DynamicJsonDocument doc(512 + text.length() * 2);
  doc["to"] = USER_ID;
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject msg = messages.createNestedObject();

  msg["type"] = "text";
  msg["text"] = text;

  bool ok = pushLineMessage(doc);
  Serial.println(ok ? "ส่งข้อความสำเร็จ!" : "ส่งข้อความล้มเหลว");
  return ok;
}

bool sendLineSticker(String packageId, String stickerId) {
  // สร้าง JSON payload สำหรับ Sticker
  DynamicJsonDocument doc(1024);
  doc["to"] = USER_ID;
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject msg = messages.createNestedObject();

  msg["type"] = "sticker";
  msg["packageId"] = packageId;
  msg["stickerId"] = stickerId;

  bool ok = pushLineMessage(doc);
  Serial.println(ok ? "ส่งสติกเกอร์สำเร็จ!" : "ส่งสติกเกอร์ล้มเหลว");
  return ok;
}

// ส่ง JSON ไปที่ LINE Messaging API (push message)
bool pushLineMessage(JsonDocument& doc) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ยังไม่ได้เชื่อมต่อ WiFi");
    return false;
  }

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin("https://api.line.me/v2/bot/message/push");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(ACCESS_TOKEN));

  int httpCode = http.POST(payload);
  bool ok = (httpCode == 200);

  if (httpCode > 0) {
    Serial.printf("HTTP Code: %d\n", httpCode);
    if (!ok) {
      Serial.println(http.getString());  // แสดงข้อความ error จาก LINE
    }
  } else {
    Serial.printf("เชื่อมต่อ LINE ไม่ได้: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return ok;
}

