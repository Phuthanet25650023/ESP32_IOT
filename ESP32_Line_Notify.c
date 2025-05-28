#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArtronShop_LineMessaging.h>

#define WIFI_SSID "ชื่อ_WIFI.4G" // WiFi Name
#define WIFI_PASSWORD "รหัส_WIFI" // WiFi Password

#define LINE_TOKEN "Token" // Channel access token

WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

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

  if (LINE.send("User/Group ID", "Hello from ESP32 !")) { // Send "Hello from ESP32 !" to LINE with User/Group ID
    Serial.println("Send notify successful");
  } else {
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
  }
}

void loop() {
  
}



//////////////////////////////////////// Stricker ///////////////////////////////////////
  LINE_Messaging_Massage_Option_t option; // สร้างตัวแปร option

  // ดูรายการสติ๊กเกอร์ที่รองรับได้ที่ https://developers.line.biz/en/docs/messaging-api/sticker-list/
  option.sticker.package_id = 446; // สติ๊กเกอร์ Package ID
  option.sticker.id = 1988; // สติ๊กเกอร์ ID

  if (LINE.send("User ID/Group ID", "รถโดนขโมย", &option)) { // ส่งข้อความ "รถโดนขโมย" ไปที่ LINE
    Serial.println("Send notify successful"); // ส่งข้อความ "Send notify successful" ไปที่ Serial Monitor
  } else { // ถ้าส่งไม่สำเร็จ
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code); // ส่งข้อความ "Send notify fail" ไปที่ Serial Monitor
  }
