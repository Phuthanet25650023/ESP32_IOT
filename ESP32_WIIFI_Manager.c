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
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// WIFI MOD AP+STATION ///////////////////////////////////////
#include <WiFi.h>

// --- ตั้งค่าสำหรับโหมด Station (ไปต่อ WiFi บ้าน) ---
const char* sta_ssid = "Elite_Ultimate_2.4G";
const char* sta_password = "24776996";

// --- ตั้งค่าสำหรับโหมด Access Point (ปล่อยสัญญาณเอง) ---
const char* ap_ssid = "ESP32_Control_Panel";
const char* ap_password = "12345678";

// กำหนด Static IP สำหรับโหมด AP (เพื่อให้จำเลข IP ง่ายๆ)
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void initWiFi() {
  // 1. ตั้งโหมดเป็นทั้ง AP และ Station
  WiFi.mode(WIFI_AP_STA);

  // 2. ตั้งค่าและเริ่มปล่อยสัญญาณ WiFi (Access Point)
  // ควรเรียก softAPConfig ก่อน softAP
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("\n--- Access Point Started ---");
  Serial.printf("SSID: %s\n", ap_ssid);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // 3. เริ่มเชื่อมต่อกับ WiFi บ้าน (Station)
  Serial.println("\n--- Connecting to Station WiFi ---");
  WiFi.begin(sta_ssid, sta_password);

  int attempts = 0;
  // วนลูปรอการเชื่อมต่อ แต่มี Timeout (20 วินาที) เพื่อไม่ให้เครื่องค้าง
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("Station IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Station WiFi (Timeout)");
    // ถึงแม้จะต่อ WiFi บ้านไม่ได้ แต่สัญญาณ AP ที่ปล่อยออกมาจะยังใช้งานได้อยู่
  }
}

void setup() {
  Serial.begin(115200);
  initWiFi();
}

void loop() {
  // ใส่ Code ควบคุมอื่นๆ ที่นี่
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  LAB 3_1 Client ///////////////////////////////////////////
#include <WiFi.h>          // ไลบรารีสำหรับใช้งาน WiFi กับ ESP32

// ============================
//  ส่วนที่ 1: กำหนดค่าต่าง ๆ
// ============================

//  ชื่อ WiFi และรหัสผ่านที่ต้องการเชื่อมต่อ
const char* ssid     = "Elite_Ultimate_2.4G";
const char* password = "24776996";

//  ตั้งค่า IP แบบ Static IP
IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

//  ตัวแปรสำหรับเช็คเวลาการเชื่อมต่อ WiFi
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // หน่วย ms (เช็คทุก 5 วินาที)

// ============================
//  ส่วนที่ 2: ฟังก์ชัน Reconnect WiFi
// ============================
void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" WiFi หลุด! พยายามเชื่อมต่อใหม่...");
    WiFi.disconnect();  // ตัดการเชื่อมต่อก่อน เพื่อให้แน่ใจว่า clean
    WiFi.begin(ssid, password);
  }
}

// ============================
//  ส่วนที่ 3: ตั้งค่าเริ่มต้นใน setup()
// ============================
void setup() {
  Serial.begin(115200);             // เริ่มต้น Serial Monitor

  // ลองตั้ง Static IP ถ้าไม่สำเร็จจะใช้ DHCP แทน
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println(" ตั้งค่า Static IP ไม่สำเร็จ! ใช้ DHCP แทน");
  }

  WiFi.begin(ssid, password);       // เริ่มเชื่อมต่อ WiFi
  Serial.print(" กำลังเชื่อมต่อ WiFi");

  // รอจนกว่าจะเชื่อมต่อ
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n เชื่อมต่อ WiFi สำเร็จ");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n ไม่สามารถเชื่อมต่อ WiFi ได้");
  }
}

// ============================
//  ส่วนที่ 4: ทำงานวนซ้ำใน loop()
// ============================
void loop() {
  // เช็คสถานะ WiFi ทุก 5 วินาที
  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();
    reconnectWiFi(); // เรียกฟังก์ชัน Reconnect ถ้า WiFi หลุด
  }

  //  ตัวอย่างการใช้งาน: พิมพ์ IP ทุก ๆ 10 วินาที (หรือปรับตามต้องการ)
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime >= 10000) {
    lastPrintTime = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(" ยังเชื่อมต่ออยู่: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println(" ยังไม่ได้เชื่อมต่อ WiFi");
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// LAB 3_2 ///////////////////////////////////////////
#include <WiFi.h>
#include <WiFiManager.h>

#define RESET_BUTTON 0 // ใช้ GPIO 0 เป็นปุ่ม Reset WiFi

void setup() {
  Serial.begin(115200);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  WiFiManager wm;

  // ตั้งค่า Static IP ถ้าต้องการ
  IPAddress staticIP(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  wm.setSTAStaticIPConfig(staticIP, gateway, subnet, dns);

  // เริ่มการเชื่อมต่อ WiFi หรือเปิดโหมด AP
  if (!wm.autoConnect("ESP32_Setup", "12345678")) {
    Serial.println("เชื่อมต่อ WiFi ไม่สำเร็จ → รีสตาร์ท");
    ESP.restart();
  }

  Serial.println(" เชื่อมต่อ WiFi แล้ว");
  Serial.println(WiFi.localIP());
}

void loop() {
  //  เช็คว่ากดปุ่ม Reset หรือไม่
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.println(" รีเซ็ต WiFi settings แล้วเข้าสู่ WiFiManager ใหม่...");
    delay(1000); // ป้องกันการกดค้าง
    WiFiManager wm;
    wm.resetSettings();  // ลบ WiFi ที่บันทึกไว้
    ESP.restart();       // รีบูตเพื่อเข้าโหมดตั้งค่าใหม่
  }

  // 🔁 เช็คการหลุด WiFi และเชื่อมใหม่
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" WiFi หลุด! กำลัง reconnect...");
    WiFi.reconnect();
    delay(5000);
  }

  // วางฟังก์ชันหลักของคุณตรงนี้ เช่น อ่าน sensor, MQTT, ฯลฯ
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// LAB 3_3 ///////////////////////////////////////////
#include <WiFi.h>
#include <WiFiManager.h>
#include <WebServer.h>

#define RESET_BUTTON 0 // GPIO 0 = ปุ่ม Reset WiFi

WebServer server(80); // สร้าง WebServer ที่พอร์ต 80

float temperature = 0; // ตัวแปรจำลองอุณหภูมิ (สามารถแทนด้วย sensor จริงได้)

// API endpoint: /temperature → ตอบกลับ JSON
void handleTemperature() {
  String json = "{ \"temperature\": ";
  json += random(100, 300) / 10.0; // temperature
  json += " }";
  server.send(200, "application/json", json);
}
void handleTemperature2() {
  String json = "{ \"temperature\": ";
  json += random(200, 300) / 10.0; // temperature
  json += " }";
  server.send(200, "application/json", json);
}
void handleTemperature3() {
  String json = "{ \"temperature\": ";
  json += random(300, 1000) / 10.0; // temperature
  json += " }";
  server.send(200, "application/json", json);
}

// Root webpage → HTML สวยงาม
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Temperature Monitor</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          background: #f0f0f5;
          color: #333;
          margin: 0;
          padding: 0;
        }
        header {
          background: #3f51b5;
          color: white;
          padding: 20px 0;
          font-size: 24px;
          font-weight: bold;
        }
        .content {
          margin-top: 40px;
        }
        .temperature {
          font-size: 48px;
          margin: 20px 0;
          color: #e91e63;
        }
        .api {
          margin-top: 30px;
          font-size: 16px;
        }
        a {
          color: #3f51b5;
          text-decoration: none;
        }
        a:hover {
          text-decoration: underline;
        }
        footer {
          margin-top: 50px;
          font-size: 14px;
          color: #999;
        }
      </style>
      <script>
        function fetchTemperature() {
          fetch('/temperature')
            .then(response => response.json())
            .then(data => {
              document.getElementById('temp1').textContent ="Data "+ data.temperature + ' C';
            });

          fetch('/temperature2')
            .then(response => response.json())
            .then(data => {
              document.getElementById('temp2').textContent ="Temp "+ data.temperature + ' C';
            });

          fetch('/temperature3')
            .then(response => response.json())
            .then(data => {
              document.getElementById('temp3').textContent = "Sensor "+data.temperature + ' C';
            });
        }
        setInterval(fetchTemperature, 3000); // ดึงข้อมูลทุก 5 วินาที
        window.onload = fetchTemperature;    // ดึงข้อมูลทันทีเมื่อโหลด
      </script>


    </head>
    <body>
      <header>
        ESP32 Temperature Monitor
      </header>
      <div class="content">
        <p class="temperature" id="temp1">-- °C</p>
        <p class="temperature" id="temp2">-- °C</p>
        <p class="temperature" id="temp3">-- °C</p>
        <div class="api">
          API Endpoint: <a href="/temperature">/temperature</a>
        </div>
      </div>
      <footer>
        &copy; 2025 ESP32 Project
      </footer>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}


void setup() {
  Serial.begin(115200);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  WiFiManager wm;

  // Optional: Static IP
  IPAddress staticIP(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  wm.setSTAStaticIPConfig(staticIP, gateway, subnet, dns);

  if (!wm.autoConnect("ESP32_Setup", "12345678")) {
    Serial.println(" Failed to connect → Restarting...");
    ESP.restart();
  }

  Serial.println(" Connected to WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/temperature", handleTemperature);
  server.on("/temperature2", handleTemperature2);
  server.on("/temperature3", handleTemperature3);
  server.begin();
  Serial.println(" WebServer started");
}

void loop() {
  // ปุ่ม Reset WiFi
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.println(" Resetting WiFi settings...");
    delay(1000);
    WiFiManager wm;
    wm.resetSettings();
    ESP.restart();
  }

  // เช็ค WiFi หลุด
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    delay(5000);
  }

  server.handleClient();
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// LAB 3 ///////////////////////////////////////////
