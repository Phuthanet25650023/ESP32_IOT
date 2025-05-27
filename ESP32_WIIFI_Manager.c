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

  Serial.println("✅ เชื่อมต่อ WiFi แล้ว");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 🟢 เช็คว่ากดปุ่ม Reset หรือไม่
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.println("🔄 รีเซ็ต WiFi settings แล้วเข้าสู่ WiFiManager ใหม่...");
    delay(1000); // ป้องกันการกดค้าง
    WiFiManager wm;
    wm.resetSettings();  // ลบ WiFi ที่บันทึกไว้
    ESP.restart();       // รีบูตเพื่อเข้าโหมดตั้งค่าใหม่
  }

  // 🔁 เช็คการหลุด WiFi และเชื่อมใหม่
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi หลุด! กำลัง reconnect...");
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
  json += random(50, 151); // temperature
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
              document.getElementById('temp').textContent = data.temperature + ' C';
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
        <p class="temperature" id="temp">-- °C</p>
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
    Serial.println("❌ Failed to connect → Restarting...");
    ESP.restart();
  }

  Serial.println("✅ Connected to WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/temperature", handleTemperature);
  server.begin();
  Serial.println("🌐 WebServer started");
}

void loop() {
  // ปุ่ม Reset WiFi
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.println("🔄 Resetting WiFi settings...");
    delay(1000);
    WiFiManager wm;
    wm.resetSettings();
    ESP.restart();
  }

  // เช็ค WiFi หลุด
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    delay(5000);
  }

  server.handleClient();
  delay(100);
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// LAB 3 ///////////////////////////////////////////
