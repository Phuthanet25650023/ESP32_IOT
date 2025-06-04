/*********************** ส่วนที่ 1: ตั้งค่าการ Debug ***********************/
#ifdef ENABLE_DEBUG
   #define DEBUG_ESP_PORT Serial       // กำหนดให้ใช้ Serial สำหรับแสดงข้อมูล debug
   #define NODEBUG_WEBSOCKETS         // ปิด debug ของ WebSocket (ลด log)
   #define NDEBUG                     // ปิด assert() เพื่อไม่ให้หยุดโปรแกรมตอน error
#endif 

/*********************** ส่วนที่ 2: Include ไลบรารีที่จำเป็น ***********************/
#include <Arduino.h>                 // ไลบรารีพื้นฐานของ Arduino

#if defined(ESP8266)
  #include <ESP8266WiFi.h>           // ไลบรารี WiFi สำหรับ ESP8266
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>                  // ไลบรารี WiFi สำหรับ ESP32 หรือ RP2040
#endif

#include "SinricPro.h"               // ไลบรารีหลักของ SinricPro
#include "SinricProSwitch.h"         // ไลบรารีสำหรับอุปกรณ์แบบ Switch

/*********************** ส่วนที่ 3: กำหนดค่าคงที่ ***********************/
#define WIFI_SSID         "Elite_Ultimate_2.4G"   // ชื่อ WiFi ที่ต้องการเชื่อมต่อ
#define WIFI_PASS         "24776996"              // รหัสผ่าน WiFi
#define APP_KEY           "f53ebfbe-62b8-4fc7-b7f0-f43bd1494bd5"           // APP_KEY จาก SinricPro
#define APP_SECRET        "78d0066b-6a07-42c4-be07-caf9942d24b0-7a69deca-f61a-48ad-956f-abbcdc9c983e"           // APP_SECRET จาก SinricPro

// รหัสอุปกรณ์และขา GPIO สำหรับ Relay แต่ละตัว
#define SWITCH_ID_1       "68295348947cbabd201cc24e"
#define RELAYPIN_1        16

#define SWITCH_ID_2       "68295362dc4a25d5c3cf7af4"
#define RELAYPIN_2        4

#define SWITCH_ID_3       "6826e7e6bddfc53e33f5e464"
#define RELAYPIN_3        2

#define BAUD_RATE         115200                  // ความเร็ว Serial Monitor

/*********************** ส่วนที่ 4: Callback สำหรับแต่ละ Switch ***********************/
// ฟังก์ชัน callback สำหรับควบคุม switch ตัวที่ 1
bool onPowerState1(const String &deviceId, bool &state) {
  Serial.printf("Device 1 turned %s", state ? "on" : "off");   // แสดงสถานะเปิด/ปิด
  digitalWrite(RELAYPIN_1, state ? HIGH : LOW);                // สั่งเปิด/ปิด relay
  return true; // แจ้งว่า handle เรียบร้อย
}

// ฟังก์ชัน callback สำหรับ switch ตัวที่ 2
bool onPowerState2(const String &deviceId, bool &state) {
  Serial.printf("Device 2 turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_2, state ? HIGH : LOW);
  return true;
}

// ฟังก์ชัน callback สำหรับ switch ตัวที่ 3
bool onPowerState3(const String &deviceId, bool &state) {
  Serial.printf("Device 3 turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_3, state ? HIGH : LOW);
  return true;
}

/*********************** ส่วนที่ 5: ฟังก์ชันเชื่อมต่อ WiFi ***********************/
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);     // ปิด sleep mode ป้องกันการตัดการเชื่อมต่อ
    WiFi.setAutoReconnect(true);            // ให้ reconnect อัตโนมัติเมื่อ WiFi หลุด
  #elif defined(ESP32)
    WiFi.setSleep(false);                   // ปิด sleep mode
    WiFi.setAutoReconnect(true);            // เปิด auto reconnect
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);         // เริ่มเชื่อมต่อ WiFi

  while (WiFi.status() != WL_CONNECTED) {   // รอจนกว่าจะเชื่อมต่อสำเร็จ
    Serial.printf(".");
    delay(250);
  }

  // แสดง IP เมื่อเชื่อมต่อเสร็จแล้ว
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

/*********************** ส่วนที่ 6: ฟังก์ชันตั้งค่า SinricPro ***********************/
void setupSinricPro() {
  // กำหนดขา GPIO ให้เป็น Output สำหรับควบคุม Relay
  pinMode(RELAYPIN_1, OUTPUT);
  pinMode(RELAYPIN_2, OUTPUT);
  pinMode(RELAYPIN_3, OUTPUT);
    
  // ลงทะเบียนอุปกรณ์แต่ละตัวกับ SinricPro
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);    // กำหนด callback เมื่อสั่งเปิด/ปิด

  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerState2);

  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  mySwitch3.onPowerState(onPowerState3);
  
  // กำหนด callback เมื่อเชื่อมต่อหรือหลุดจาก SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });

  // เริ่มต้นใช้งาน SinricPro ด้วย Key และ Secret
  SinricPro.begin(APP_KEY, APP_SECRET);
}

/*********************** ส่วนที่ 7: setup() เริ่มทำงานครั้งแรก ***********************/
void setup() {
  Serial.begin(BAUD_RATE);                  // เปิดใช้งาน Serial
  Serial.printf("\r\n\r\n");
  setupWiFi();                              // เชื่อมต่อ WiFi
  setupSinricPro();                         // ตั้งค่า SinricPro
}

/*********************** ส่วนที่ 8: loop() ทำงานวนตลอดเวลา ***********************/
void loop() {
  // ตรวจสอบสถานะ WiFi ถ้าหลุดให้พยายามเชื่อมต่อใหม่
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi]: Disconnected. Attempting reconnect...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(1000); // หน่วงเวลารอเชื่อมต่อ
    return;      // ออกจาก loop ชั่วคราว
  }

  SinricPro.handle();  // ประมวลผล event จาก SinricPro
}
