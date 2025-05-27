/////////////////////////////////////////  LAB 3_1 Client ///////////////////////////////////////////
#include <WiFi.h>          // ไลบรารีสำหรับใช้งาน WiFi กับ ESP32

// ============================
// 📌 ส่วนที่ 1: กำหนดค่าต่าง ๆ
// ============================

// 🔐 ชื่อ WiFi และรหัสผ่านที่ต้องการเชื่อมต่อ
const char* ssid     = "Elite_Ultimate_2.4G";
const char* password = "24776996";

// 🌐 ตั้งค่า IP แบบ Static IP
IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// 🕒 ตัวแปรสำหรับเช็คเวลาการเชื่อมต่อ WiFi
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // หน่วย ms (เช็คทุก 5 วินาที)

// ============================
// 🔁 ส่วนที่ 2: ฟังก์ชัน Reconnect WiFi
// ============================
void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi หลุด! พยายามเชื่อมต่อใหม่...");
    WiFi.disconnect();  // ตัดการเชื่อมต่อก่อน เพื่อให้แน่ใจว่า clean
    WiFi.begin(ssid, password);
  }
}

// ============================
// ⚙️ ส่วนที่ 3: ตั้งค่าเริ่มต้นใน setup()
// ============================
void setup() {
  Serial.begin(115200);             // เริ่มต้น Serial Monitor

  // ลองตั้ง Static IP ถ้าไม่สำเร็จจะใช้ DHCP แทน
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("❌ ตั้งค่า Static IP ไม่สำเร็จ! ใช้ DHCP แทน");
  }

  WiFi.begin(ssid, password);       // เริ่มเชื่อมต่อ WiFi
  Serial.print("🔌 กำลังเชื่อมต่อ WiFi");

  // รอจนกว่าจะเชื่อมต่อ
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ เชื่อมต่อ WiFi สำเร็จ");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ ไม่สามารถเชื่อมต่อ WiFi ได้");
  }
}

// ============================
// 🔁 ส่วนที่ 4: ทำงานวนซ้ำใน loop()
// ============================
void loop() {
  // เช็คสถานะ WiFi ทุก 5 วินาที
  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();
    reconnectWiFi(); // เรียกฟังก์ชัน Reconnect ถ้า WiFi หลุด
  }

  // 🧪 ตัวอย่างการใช้งาน: พิมพ์ IP ทุก ๆ 10 วินาที (หรือปรับตามต้องการ)
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime >= 10000) {
    lastPrintTime = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("📡 ยังเชื่อมต่ออยู่: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("🚫 ยังไม่ได้เชื่อมต่อ WiFi");
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////
