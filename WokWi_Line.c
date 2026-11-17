/*
 * WokWi_Line.c
 * ESP32 ส่งข้อความแจ้งเตือนไปยัง LINE ผ่าน LINE Messaging API (Push Message)
 * - ส่งข้อความทักทายตอนบอร์ดเริ่มทำงาน
 * - ส่งข้อความทุกครั้งที่กดปุ่ม (GPIO4)
 * - LED (GPIO2) ติดระหว่างกำลังส่งข้อมูล
 */

#include <WiFi.h>              // ไลบรารีสำหรับเชื่อมต่อ WiFi ของ ESP32
#include <WiFiClientSecure.h>  // ไลบรารีสำหรับเชื่อมต่อแบบเข้ารหัส (HTTPS/TLS)
#include <HTTPClient.h>        // ไลบรารีสำหรับส่งคำขอ HTTP (GET/POST)
#include <ArduinoJson.h>       // ไลบรารีสำหรับสร้างและอ่านข้อมูลรูปแบบ JSON (ใช้เวอร์ชัน 7)

// ===== WiFi =====
const char* ssid = "Wokwi-GUEST";  // ชื่อ WiFi (SSID) ของเครื่องจำลอง Wokwi
const char* password = "";         // รหัสผ่าน WiFi (Wokwi-GUEST ไม่มีรหัสผ่าน)

// ===== LINE Messaging API =====
const char* ACCESS_TOKEN = "*******"; // Channel access token จาก LINE Developers ใช้ยืนยันสิทธิ์การส่ง
const char* USER_ID = "*******";      // ID ผู้รับ ต้องเป็น User ID รูปแบบ Uxxxxxxxx... (33 ตัวอักษร)

// ===== Hardware =====
const int BUTTON_PIN = 4; // ขาที่ต่อปุ่มกด (ต่อ GPIO4 กับ GND)
const int LED_PIN = 2;    // ขาที่ต่อ LED แสดงสถานะการส่ง

void connectWiFi();                          // ประกาศต้นแบบฟังก์ชันเชื่อมต่อ WiFi
bool sendLineMessage(const String& message); // ประกาศต้นแบบฟังก์ชันส่งข้อความ LINE (คืนค่า true ถ้าสำเร็จ)

// setup() ทำงานครั้งเดียวตอนบอร์ดเริ่มทำงาน
void setup() {
  Serial.begin(115200);              // เปิด Serial Monitor ที่ความเร็ว 115200 bps สำหรับดูข้อความ debug
  pinMode(BUTTON_PIN, INPUT_PULLUP); // ตั้งขาปุ่มเป็นอินพุตแบบ pull-up ภายใน (ปกติ = HIGH, กดแล้ว = LOW)
  pinMode(LED_PIN, OUTPUT);          // ตั้งขา LED เป็นเอาต์พุต

  connectWiFi();                                  // เชื่อมต่อ WiFi ก่อนเริ่มส่งข้อมูล
  sendLineMessage("สวัสดี! ESP32 เริ่มทำงานแล้ว"); // ส่งข้อความแจ้งว่าบอร์ดเริ่มทำงานแล้ว
}

// loop() ทำงานวนซ้ำตลอดเวลา
void loop() {
  // ต่อ WiFi ใหม่อัตโนมัติถ้าหลุด
  if (WiFi.status() != WL_CONNECTED) { // ตรวจสอบว่า WiFi ยังเชื่อมต่ออยู่หรือไม่
    connectWiFi();                     // ถ้าหลุด ให้เชื่อมต่อใหม่
  }

  // กดปุ่ม -> ส่งข้อความ 1 ครั้ง (มี debounce)
  static bool lastState = HIGH;          // เก็บสถานะปุ่มรอบก่อน (static = ค่าไม่หายเมื่อจบ loop รอบนี้)
  bool state = digitalRead(BUTTON_PIN);  // อ่านสถานะปุ่มปัจจุบัน
  if (lastState == HIGH && state == LOW) { // ตรวจจับขอบขาลง: เพิ่งเปลี่ยนจาก "ไม่กด" เป็น "กด"
    delay(50);                             // หน่วงเวลา 50 ms เพื่อกันสัญญาณเด้งของหน้าสัมผัสปุ่ม (debounce)
    if (digitalRead(BUTTON_PIN) == LOW) {  // อ่านซ้ำ ถ้ายังกดอยู่จริง ถือว่าเป็นการกดที่ถูกต้อง
      // ส่งข้อความพร้อมบอกเวลาที่บอร์ดทำงานมา (millis()/1000 = วินาที)
      sendLineMessage("มีการกดปุ่มที่ ESP32 (uptime " + String(millis() / 1000) + " s)");
    }
  }
  lastState = state; // บันทึกสถานะปุ่มไว้เทียบในรอบถัดไป
  delay(10);         // หน่วงเล็กน้อยเพื่อลดภาระ CPU
}

// ฟังก์ชันเชื่อมต่อ WiFi และรอจนกว่าจะเชื่อมต่อสำเร็จ
void connectWiFi() {
  Serial.print("กำลังเชื่อมต่อ WiFi");   // แสดงข้อความสถานะบน Serial Monitor
  WiFi.mode(WIFI_STA);                   // ตั้งโหมดเป็น Station (ESP32 เป็นอุปกรณ์ลูกข่ายที่ไปต่อกับเราเตอร์)
  WiFi.begin(ssid, password, 6);         // เริ่มเชื่อมต่อ; Wokwi-GUEST ใช้ channel 6 ระบุไว้จะต่อได้เร็วขึ้น
  while (WiFi.status() != WL_CONNECTED) { // วนรอจนกว่าจะเชื่อมต่อสำเร็จ
    delay(500);                          // รอ 0.5 วินาทีต่อรอบ
    Serial.print(".");                   // พิมพ์จุดเพื่อแสดงว่ายังรออยู่
  }
  Serial.println("\nเชื่อมต่อ WiFi สำเร็จ"); // แจ้งว่าเชื่อมต่อแล้ว
  Serial.print("IP: ");                   // แสดงหัวข้อ IP
  Serial.println(WiFi.localIP());         // แสดง IP Address ที่ได้รับจากเราเตอร์
}

// ฟังก์ชันส่งข้อความไปยัง LINE ผ่าน Push Message API
bool sendLineMessage(const String& message) {
  if (WiFi.status() != WL_CONNECTED) {   // ตรวจสอบก่อนว่ามี WiFi หรือไม่
    Serial.println("WiFi ไม่ได้เชื่อมต่อ"); // ถ้าไม่มี แจ้งเตือน
    return false;                        // และยกเลิกการส่ง
  }

  WiFiClientSecure client; // สร้างตัวเชื่อมต่อแบบ HTTPS
  client.setInsecure();    // ข้ามการตรวจใบรับรอง SSL (เหมาะกับการทดลอง ไม่ควรใช้ในงานจริง)

  HTTPClient http;         // สร้างออบเจกต์สำหรับส่งคำขอ HTTP
  if (!http.begin(client, "https://api.line.me/v2/bot/message/push")) { // เริ่มต่อไปยัง endpoint ของ LINE Push API
    Serial.println("http.begin ล้มเหลว"); // ถ้าเริ่มไม่ได้ แจ้งข้อผิดพลาด
    return false;                        // และยกเลิกการส่ง
  }
  http.addHeader("Content-Type", "application/json");                   // ระบุว่าข้อมูลที่ส่งเป็น JSON
  http.addHeader("Authorization", String("Bearer ") + ACCESS_TOKEN);    // ใส่ token เพื่อยืนยันสิทธิ์กับ LINE

  // สร้าง JSON payload รูปแบบ: {"to":"Uxxx","messages":[{"type":"text","text":"..."}]}
  JsonDocument doc;                                         // สร้างเอกสาร JSON ว่าง
  doc["to"] = USER_ID;                                      // กำหนดผู้รับข้อความ
  JsonArray messages = doc["messages"].to<JsonArray>();     // สร้างอาร์เรย์ "messages"
  JsonObject msg = messages.add<JsonObject>();              // เพิ่มออบเจกต์ข้อความ 1 ชิ้นลงในอาร์เรย์
  msg["type"] = "text";                                     // ชนิดข้อความเป็นตัวอักษร
  msg["text"] = message;                                    // เนื้อหาข้อความที่จะส่ง

  String payload;                 // ตัวแปรเก็บ JSON ในรูปข้อความ
  serializeJson(doc, payload);    // แปลง JSON document เป็น String เพื่อนำไปส่ง

  digitalWrite(LED_PIN, HIGH);     // เปิด LED แสดงว่ากำลังส่ง
  int httpCode = http.POST(payload); // ส่งคำขอ POST พร้อม payload และรับรหัสตอบกลับ HTTP
  digitalWrite(LED_PIN, LOW);      // ปิด LED เมื่อส่งเสร็จ

  bool ok = false;                 // ตัวแปรเก็บผลการส่ง (เริ่มต้น = ไม่สำเร็จ)
  if (httpCode > 0) {              // httpCode > 0 = เซิร์ฟเวอร์ตอบกลับมา (ติดต่อได้)
    Serial.printf("HTTP Code: %d\n", httpCode); // แสดงรหัสตอบกลับ
    String response = http.getString();          // อ่านข้อความตอบกลับจาก LINE
    if (httpCode == 200) {                       // 200 = ส่งสำเร็จ
      Serial.println("ส่งสำเร็จ: " + message);    // แสดงข้อความที่ส่งไป
      ok = true;                                 // บันทึกว่าสำเร็จ
    } else {                                     // รหัสอื่น เช่น 400 (ข้อมูลผิด), 401 (token ผิด)
      Serial.println("LINE ตอบกลับ: " + response); // แสดงรายละเอียดข้อผิดพลาดจาก LINE
    }
  } else {                         // httpCode <= 0 = ติดต่อเซิร์ฟเวอร์ไม่ได้ (ปัญหาเครือข่าย/การเชื่อมต่อ)
    Serial.printf("ส่งล้มเหลว: %s\n", http.errorToString(httpCode).c_str()); // แปลงรหัสผิดพลาดเป็นข้อความ
  }
  http.end();   // ปิดการเชื่อมต่อและคืนหน่วยความจำ
  return ok;    // คืนผลการส่ง (true = สำเร็จ, false = ล้มเหลว)
}
