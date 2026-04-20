////////////////////////////////// Scanner Mode WIFI_STA /////////////////////////////////
#include "WiFi.h"

void setup() {
    Serial.begin(115200);

    // กำหนดโหมดเป็น Station (ลูกข่าย) 
    // และตัดการเชื่อมต่อเดิมที่อาจค้างอยู่เพื่อให้การ Scan แม่นยำขึ้น
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("System Ready: WiFi Scanner");
}

void loop() {
    Serial.println("--- Scanning for networks... ---");

    // เริ่มทำการสแกน และคืนค่าเป็นจำนวน WiFi ที่พบ (n)
    // การสแกนแบบนี้จะหยุดรอจนกว่าจะเสร็จ (Blocking Mode)
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        Serial.println("No networks found.");
    } else {
        Serial.printf("%d networks found\n", n);
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        Serial.println("--------------------------------------------------------------");

        for (int i = 0; i < n; ++i) {
            // ใช้ Serial.printf เพื่อจัดการ Format ข้อความให้ตรงกันสวยงาม
            // %2d  = เลข 2 หลัก
            // %-32.32s = ข้อความชิดซ้าย ความกว้าง 32 ตัวอักษร
            // %4d  = เลข 4 หลัก (สำหรับ RSSI ที่เป็นค่าติดลบ)
            Serial.printf("%2d | %-32.32s | %4d | %2d | ", 
                          i + 1, 
                          WiFi.SSID(i).c_str(), 
                          WiFi.RSSI(i), 
                          WiFi.channel(i));

            // ตรวจสอบประเภทความปลอดภัย
            printEncryptionType(WiFi.encryptionType(i));
            Serial.println();
        }
    }

    // ล้างข้อมูลการสแกนออกจาก Memory เพื่อคืนค่า RAM
    WiFi.scanDelete();

    Serial.println("--------------------------------------------------------------\n");
    delay(5000); // พัก 5 วินาทีก่อนเริ่มสแกนใหม่
}

// แยกฟังก์ชันออกมาเพื่อให้อ่านโค้ดหลักได้ง่ายขึ้น
void printEncryptionType(wifi_auth_mode_t type) {
    switch (type) {
        case WIFI_AUTH_OPEN:            Serial.print("Open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        default:                        Serial.print("Unknown");
    }
}
