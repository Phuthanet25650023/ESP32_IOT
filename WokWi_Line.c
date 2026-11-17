#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===== WiFi =====
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ===== LINE Messaging API =====
const char* ACCESS_TOKEN = "*******";  // Channel access token จาก LINE Developers
const char* USER_ID      = "*******";  // ต้องเป็น User ID รูปแบบ Uxxxxxxxx... (33 ตัวอักษร)

// ===== Hardware =====
const int BUTTON_PIN = 4;    // ปุ่มกด (ต่อ GPIO4 กับ GND)
const int LED_PIN    = 2;    // LED แสดงสถานะการส่ง

void connectWiFi();
bool sendLineMessage(const String& message);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  connectWiFi();
  sendLineMessage("สวัสดี! ESP32 เริ่มทำงานแล้ว");
}

void loop() {
  // ต่อ WiFi ใหม่อัตโนมัติถ้าหลุด
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // กดปุ่ม -> ส่งข้อความ 1 ครั้ง (มี debounce)
  static bool lastState = HIGH;
  bool state = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && state == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      sendLineMessage("มีการกดปุ่มที่ ESP32 (uptime " + String(millis() / 1000) + " s)");
    }
  }
  lastState = state;
  delay(10);
}

void connectWiFi() {
  Serial.print("กำลังเชื่อมต่อ WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, 6);   // Wokwi-GUEST ใช้ channel 6 จะต่อได้เร็วขึ้น
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

bool sendLineMessage(const String& message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi ไม่ได้เชื่อมต่อ");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();            // ข้ามการตรวจใบรับรอง (เหมาะกับการทดลอง)

  HTTPClient http;
  if (!http.begin(client, "https://api.line.me/v2/bot/message/push")) {
    Serial.println("http.begin ล้มเหลว");
    return false;
  }
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + ACCESS_TOKEN);

  // สร้าง JSON payload
  JsonDocument doc;
  doc["to"] = USER_ID;
  JsonArray messages = doc["messages"].to<JsonArray>();
  JsonObject msg = messages.add<JsonObject>();
  msg["type"] = "text";
  msg["text"] = message;

  String payload;
  serializeJson(doc, payload);

  digitalWrite(LED_PIN, HIGH);
  int httpCode = http.POST(payload);
  digitalWrite(LED_PIN, LOW);

  bool ok = false;
  if (httpCode > 0) {
    Serial.printf("HTTP Code: %d\n", httpCode);
    String response = http.getString();
    if (httpCode == 200) {
      Serial.println("ส่งสำเร็จ: " + message);
      ok = true;
    } else {
      Serial.println("LINE ตอบกลับ: " + response);
    }
  } else {
    Serial.printf("ส่งล้มเหลว: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  return ok;
}
