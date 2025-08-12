#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArtronShop_LineMessaging.h>

#define BUTTON_PIN 0 // ขา GPIO ที่ต่อปุ่ม
#define WIFI_SSID "Elite_Ultimate_Archer_2.4G" // WiFi Name
#define WIFI_PASSWORD "24776996" // WiFi Password
#define LINE_TOKEN "TOKEN_LINE"  // Channel access token
#define User "U30199332a9032acc6ff7b548e7246438" //userId / Group
WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((wifiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  LINE.begin(LINE_TOKEN);

  if (LINE.send(User, "Hello from ESP32 !")) {  // Send "Hello from ESP32 !" to LINE with User/Group ID
    Serial.println("Send notify successful");
  } else {
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
  }
}

void loop() {

  int buttonState = digitalRead(BUTTON_PIN);  // อ่านสถานะปุ่ม

  if (buttonState == LOW) {  // ถ้าปุ่มถูกกด (Active Low)
    if (LINE.send(User, "TEST Button !")) {  // Send "Hello from ESP32 !" to LINE with User/Group ID
      Serial.println("Send notify successful");
    } else {
      Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
    }
  }
}



//////////////////////////////////////// Stricker ///////////////////////////////////////
  LINE_Messaging_Massage_Option_t option; // สร้างตัวแปร option

  // ดูรายการสติ๊กเกอร์ที่รองรับได้ที่ https://developers.line.biz/en/docs/messaging-api/sticker-list/
  option.sticker.package_id = 789; // สติ๊กเกอร์ Package ID
  option.sticker.id = 10884; // สติ๊กเกอร์ ID

  if (LINE.send("User ID/Group ID", "ข้อความ", &option)) { // ส่งข้อความ "ข้อความ" ไปที่ LINE
    Serial.println("Send notify successful"); // ส่งข้อความ "Send notify successful" ไปที่ Serial Monitor
  } else { // ถ้าส่งไม่สำเร็จ
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code); // ส่งข้อความ "Send notify fail" ไปที่ Serial Monitor
  }


////////////////////////////////// รูปภาพ ///////////////////////////////////////////////
  LINE_Messaging_Massage_Option_t option; // สร้างตัวแปร option

  option.image.url = "https://img5.pic.in.th/file/secure-sv1/imagebb0cd15de6ef0b4b.png"; // ลิ้งรูป ไฟล์ JPEG ขนาดไม่เกิน 2048×2048px

  if (LINE.send("User ID/Group ID", "รถโดยขโมย", &option)) { // ส่งข้อความ "รถโดนขโมย" ไปที่ LINE
    Serial.println("Send notify successful");
  } else {
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
  }
