-- สร้างตาราง sensors
CREATE TABLE sensors (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_name VARCHAR(100) NOT NULL,
    value FLOAT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- เพิ่มข้อมูลลงในตาราง sensors
INSERT INTO sensors (sensor_name, value) VALUES 
('Temperature Sensor', 25.5),
('Humidity Sensor', 60.2),
('Light Sensor', 300.0);




//////////////////////////// lab 1 /////////////////////////////////////// ส่งข้อมูลเข้า Phpmyadmin
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// WiFi
const char* ssid = "Elite_Ultimate_2.4G";
const char* password = "24776996";

// MySQL Server 192.168.81.244
IPAddress server_ip(192, 168, 1, 147); // IP ของ MySQL Server
char user[] = "test";
char password_mysql[] = "12345678";
char database[] = "data-set";

WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  Serial.print("Connecting to MySQL server...");
  while (!conn.connect(server_ip, 3306, user, password_mysql)) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("connected!");
}

void loop() {
  float value = random(20, 35); // จำลองค่าจาก sensor
  char query[256];
  
  // เพิ่มเลือกฐานข้อมูลก่อน (optional แต่แนะนำให้ชัดเจน)
  /*MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("USE test");*/
  snprintf(query, sizeof(query), "USE `%s`", database);
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute(query);  

  // สร้างคำสั่ง INSERT เข้ากับตาราง sensors
  sprintf(query, "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor', %.2f)", value);

  if (conn.connected()) {
    Serial.println("Sending data...");
    cur->execute(query);
    Serial.println("Data inserted!");
  } else {
    Serial.println("MySQL not connected.");
  }

  delete cur;
  delay(10000); // ทุก 10 วินาที
}
////////////////////////////////////////////////////////////////////////
///////////////////////////// lab 2 /////////////////////////////////////// ดึงข้อมูล และส่งข้อมูล ผ่าน Phpmyadmin แสดงข้อมูลผ่าน Web

#include <WiFi.h>                    // สำหรับเชื่อมต่อ WiFi
#include <MySQL_Connection.h>       // สำหรับเชื่อมต่อกับ MySQL Server
#include <MySQL_Cursor.h>           // สำหรับส่งคำสั่ง SQL (query)
#include <WebServer.h>              // สำหรับสร้าง HTTP Web Server บน ESP32

////////////////////////////////////
//   WiFi & MySQL Configuration
////////////////////////////////////
const char* ssid = "Elite_Ultimate_2.4G";   // ชื่อ WiFi
const char* password = "24776996";                 // รหัสผ่าน WiFi

IPAddress server_ip(192, 168, 1, 135);             // IP Address ของ MySQL Server
char user[] = "test";                       // Username ของ MySQL
char password_mysql[] = "12345678";                // Password ของ MySQL
char database[] = "data-set";                      // ชื่อ Database ที่ใช้

////////////////////////////////////
//   Create Network & Server Objects
////////////////////////////////////
WiFiClient client;                                 // สร้าง client สำหรับเชื่อมต่อ MySQL
MySQL_Connection conn((Client *)&client);          // ตัวแปรเชื่อมต่อ MySQL
WebServer server(80);                              // สร้าง Web Server ที่ port 80

////////////////////////////////////
//   Sensor List
////////////////////////////////////
const char* sensorNames[] = {"TempSensor1", "TempSensor2", "TempSensor3"};
const int numSensors = 3;

////////////////////////////////////
//   JSON Data Endpoint (/data)
//   -> ใช้โดย JavaScript fetch() เพื่ออัปเดตหน้าเว็บแบบ Real-Time
//      โดยไม่ต้อง reload ทั้งหน้า
////////////////////////////////////
void handleData() {
  // ถ้ายังไม่ได้เชื่อมต่อ MySQL ให้พยายามเชื่อมต่อ
  if (!conn.connected()) {
    if (!conn.connect(server_ip, 3306, user, password_mysql, database)) {
      server.send(500, "application/json", "{\"error\":\"MySQL connection failed\"}");
      return;
    }
  }

  char query[320];   // query แบบรวม subquery ยาว ~280 ตัวอักษร ต้องเผื่อให้พอ (เดิม 256 ทำให้ overflow!)
  String json = "{\"sensors\":[";

  for (int i = 0; i < numSensors; i++) {
    String latestValue = "null";
    String latestTime  = "\"\"";
    String maxV = "null", minV = "null", avgV = "null";

    // ---- รวมค่าล่าสุด + MAX/MIN/AVG ไว้ใน query เดียว ลด round-trip ไป MySQL ----
    if (conn.connected()) {
      MySQL_Cursor *cur = new MySQL_Cursor(&conn);
      snprintf(query, sizeof(query),
        "SELECT "
        "(SELECT value FROM sensors WHERE sensor_name='%s' ORDER BY created_at DESC LIMIT 1), "
        "(SELECT created_at FROM sensors WHERE sensor_name='%s' ORDER BY created_at DESC LIMIT 1), "
        "MAX(value), MIN(value), AVG(value) "
        "FROM sensors WHERE sensor_name='%s'",
        sensorNames[i], sensorNames[i], sensorNames[i]);
      if (cur->execute(query)) {
        cur->get_columns();
        row_values *row = cur->get_next_row();
        if (row != NULL) {
          latestValue = row->values[0] ? String(row->values[0]) : "null";
          latestTime  = "\"" + String(row->values[1] ? row->values[1] : "") + "\"";
          maxV        = row->values[2] ? String(row->values[2]) : "null";
          minV        = row->values[3] ? String(row->values[3]) : "null";
          avgV        = row->values[4] ? String(row->values[4]) : "null";
        }
        while (cur->get_next_row() != NULL) {}   // ดึงจนหมดแถวเพื่ออ่าน EOF ปิด result set ให้ครบ
      }
      // execute() ล้มเหลว (เช่น connection timeout) -> ข้ามไปเลย ไม่แตะ get_columns()/get_next_row()
      delete cur;
    }

    json += "{";
    json += "\"name\":\"" + String(sensorNames[i]) + "\",";
    json += "\"latest\":" + latestValue + ",";
    json += "\"time\":" + latestTime + ",";
    json += "\"max\":" + maxV + ",";
    json += "\"min\":" + minV + ",";
    json += "\"avg\":" + avgV;
    json += "}";
    if (i < numSensors - 1) json += ",";
  }
  json += "]}";

  server.send(200, "application/json", json);
}

////////////////////////////////////
//   HTML Page (/) - Static, โหลดครั้งเดียว
//   ข้อมูลจริงจะถูกดึงผ่าน fetch('/data') แบบ Real-Time
////////////////////////////////////
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Sensor Dashboard</title>
<style>
  :root {
    --bg: #0f172a;
    --card: #1e293b;
    --card-border: #334155;
    --text: #e2e8f0;
    --muted: #94a3b8;
    --accent: #38bdf8;
    --max: #f87171;
    --min: #60a5fa;
    --avg: #34d399;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: 'Segoe UI', Arial, sans-serif;
    background: radial-gradient(circle at top, #1e293b, #0f172a 70%);
    color: var(--text);
    min-height: 100vh;
    padding: 24px;
  }
  header {
    max-width: 1100px;
    margin: 0 auto 24px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    flex-wrap: wrap;
    gap: 12px;
  }
  h1 {
    margin: 0;
    font-size: 1.6rem;
    letter-spacing: 0.5px;
  }
  .status {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 0.9rem;
    color: var(--muted);
  }
  .dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: #ef4444;
    box-shadow: 0 0 8px #ef4444;
    transition: background 0.3s, box-shadow 0.3s;
  }
  .dot.online {
    background: #22c55e;
    box-shadow: 0 0 8px #22c55e;
  }
  #cards {
    max-width: 1100px;
    margin: 0 auto;
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
    gap: 20px;
  }
  .card {
    background: var(--card);
    border: 1px solid var(--card-border);
    border-radius: 16px;
    padding: 20px;
    box-shadow: 0 4px 16px rgba(0,0,0,0.3);
    transition: transform 0.2s;
  }
  .card:hover { transform: translateY(-4px); }
  .card h2 {
    margin: 0 0 4px;
    font-size: 1.05rem;
    color: var(--accent);
  }
  .card .time {
    font-size: 0.75rem;
    color: var(--muted);
    margin-bottom: 12px;
  }
  .card .latest {
    font-size: 2.6rem;
    font-weight: 700;
    margin-bottom: 16px;  
  }
  .stats {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 8px;
    text-align: center;
  }
  .stat {
    background: rgba(255,255,255,0.04);
    border-radius: 10px;
    padding: 8px 4px;
  }
  .stat .label {
    display: block;
    font-size: 0.7rem;
    color: var(--muted);
    margin-bottom: 4px;
  }
  .stat .value {
    display: block;
    font-size: 1.1rem;
    font-weight: 600;
  }
  .stat .value.max { color: var(--max); }
  .stat .value.min { color: var(--min); }
  .stat .value.avg { color: var(--avg); }
  footer {
    max-width: 1100px;
    margin: 20px auto 0;
    text-align: right;
    font-size: 0.75rem;
    color: var(--muted);
  }
</style>
</head>
<body>
  <header>
    <h1>🌡️ Sensor Dashboard</h1>
    <div class="status"><span id="dot" class="dot"></span><span id="statusText">Connecting...</span></div>
  </header>

  <div id="cards"></div>

  <footer id="updated">Last update: -</footer>

<script>
const REFRESH_MS = 2000;

function fmt(v) {
  return (v === null || v === undefined || v === '') ? '--' : Number(v).toFixed(2);
}

function render(sensors) {
  const container = document.getElementById('cards');
  container.innerHTML = '';
  sensors.forEach(s => {
    const card = document.createElement('div');
    card.className = 'card';
    card.innerHTML =
      '<h2>' + s.name + '</h2>' +
      '<div class="time">' + (s.time || '') + '</div>' +
      '<div class="latest">' + fmt(s.latest) + '</div>' +
      '<div class="stats">' +
        '<div class="stat"><span class="label">MAX</span><span class="value max">' + fmt(s.max) + '</span></div>' +
        '<div class="stat"><span class="label">MIN</span><span class="value min">' + fmt(s.min) + '</span></div>' +
        '<div class="stat"><span class="label">AVG</span><span class="value avg">' + fmt(s.avg) + '</span></div>' +
      '</div>';
    container.appendChild(card);
  });
  document.getElementById('updated').textContent = 'Last update: ' + new Date().toLocaleTimeString();
}

function setStatus(online) {
  document.getElementById('dot').className = online ? 'dot online' : 'dot';
  document.getElementById('statusText').textContent = online ? 'Online' : 'Offline';
}

async function fetchData() {
  try {
    const res = await fetch('/data');
    const data = await res.json();
    render(data.sensors);
    setStatus(true);
  } catch (e) {
    setStatus(false);
  }
}

fetchData();
setInterval(fetchData, REFRESH_MS);
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

////////////////////////////////////
//   Setup Function (เรียกครั้งเดียว)
////////////////////////////////////
void setup() {
  Serial.begin(115200);                     // เริ่ม serial monitor
  WiFi.begin(ssid, password);               // เริ่มเชื่อมต่อ WiFi
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {   // รอจนกว่าจะเชื่อมต่อ WiFi สำเร็จ
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());           // แสดง IP ของ ESP32

  // เชื่อมต่อ MySQL และระบุ database ตั้งแต่ต้น
  if (conn.connect(server_ip, 3306, user, password_mysql, database)) {
    Serial.println("MySQL connected");
  } else {
    Serial.println("MySQL connection failed");
  }

  // ตั้งค่า HTTP route
  server.on("/", handleRoot);               // หน้าเว็บหลัก (static HTML)
  server.on("/data", handleData);           // API คืนค่า JSON สำหรับอัปเดต Real-Time
  server.begin();                           // เริ่มต้น HTTP server
  Serial.println("HTTP server started");
  Serial.printf("Free heap at startup: %u bytes\n", ESP.getFreeHeap());
}

////////////////////////////////////
//   Loop Function (ทำงานวนซ้ำ)
//   ใช้ millis() แทน delay() เพื่อไม่ให้ web server ค้าง
//   ระหว่างรอ insert ข้อมูล -> ทำให้หน้าเว็บตอบสนอง Real-Time จริง ๆ
////////////////////////////////////
unsigned long lastInsertMs = 0;
const unsigned long INSERT_INTERVAL_MS = 10000;   // insert ทุก 10 วินาที

void loop() {
  server.handleClient();   // ต้องเรียกทุกรอบ ห้ามมี delay ยาว ๆ คั่น ไม่งั้นเว็บจะค้าง

  unsigned long now = millis();
  if (now - lastInsertMs >= INSERT_INTERVAL_MS) {
    lastInsertMs = now;
    insertSensorData();
  }
}

void insertSensorData() {
  if (!conn.connected()) {
    Serial.println("MySQL not connected.");
    return;
  }

  float value1 = random(0, 1000);
  float value2 = random(1000, 3000);
  float value3 = random(3000, 4095);
  char query[256];

  Serial.printf("Sending data... (free heap: %u bytes)\n", ESP.getFreeHeap());
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);

  snprintf(query, sizeof(query), "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor1', %.2f)", value1);
  cur->execute(query);
  snprintf(query, sizeof(query), "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor2', %.2f)", value2);
  cur->execute(query);
  snprintf(query, sizeof(query), "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor3', %.2f)", value3);
  cur->execute(query);

  delete cur;
  Serial.printf("Data inserted! (free heap: %u bytes)\n", ESP.getFreeHeap());
}
