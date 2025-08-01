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
////////////////////////////////////
//   Include Libraries
////////////////////////////////////
#include <WiFi.h>                    // สำหรับเชื่อมต่อ WiFi
#include <MySQL_Connection.h>       // สำหรับเชื่อมต่อกับ MySQL Server
#include <MySQL_Cursor.h>           // สำหรับส่งคำสั่ง SQL (query)
#include <WebServer.h>              // สำหรับสร้าง HTTP Web Server บน ESP32

////////////////////////////////////
//   WiFi & MySQL Configuration
////////////////////////////////////
const char* ssid = "Elite_Ultimate_Archer_2.4G";   // ชื่อ WiFi
const char* password = "24776996";                 // รหัสผ่าน WiFi

IPAddress server_ip(192, 168, 0, 146);             // IP Address ของ MySQL Server
char user[] = "cartoon2477";                       // Username ของ MySQL
char password_mysql[] = "24776996";                // Password ของ MySQL
char database[] = "data-set";                      // ชื่อ Database ที่ใช้

////////////////////////////////////
//   Create Network & Server Objects
////////////////////////////////////
WiFiClient client;                                 // สร้าง client สำหรับเชื่อมต่อ MySQL
MySQL_Connection conn((Client *)&client);          // ตัวแปรเชื่อมต่อ MySQL
WebServer server(80);                              // สร้าง Web Server ที่ port 80

////////////////////////////////////
//   HTTP Request Handler Function
////////////////////////////////////
void handleRoot() {
  // ถ้ายังไม่ได้เชื่อมต่อ MySQL ให้พยายามเชื่อมต่อ
  if (!conn.connected()) {
    if (!conn.connect(server_ip, 3306, user, password_mysql, database)) {
      server.send(500, "text/plain", "MySQL connection failed");
      return;
    }
  }

  // สร้าง Cursor และ query ข้อมูลล่าสุดจากตาราง sensors
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("SELECT sensor_name, value, created_at FROM sensors ORDER BY created_at DESC LIMIT 5");

  column_names *cols = cur->get_columns(); // ดึงชื่อคอลัมน์

  // เริ่มสร้าง HTML เพื่อตอบกลับ
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta http-equiv="refresh" content="5"> <!-- รีเฟรชทุก 5 วินาที -->
      <title>Sensor Data</title>
      <style>
        body { font-family: Arial; background-color: #f0f0f0; padding: 20px; }
        h1 { color: #333; }
        table { border-collapse: collapse; width: 100%; max-width: 800px; margin: auto; background-color: white; }
        th, td { padding: 10px; border: 1px solid #ccc; text-align: center; }
        th { background-color: #4CAF50; color: white; }
        tr:nth-child(even) { background-color: #f9f9f9; }
      </style>
    </head>
    <body>
      <h1 align="center">Latest Sensor Data</h1>
      <table>
        <tr>
  )rawliteral";

  // เพิ่มหัวตารางจากชื่อคอลัมน์
  for (int i = 0; i < cols->num_fields; i++) {
    html += "<th>" + String(cols->fields[i]->name) + "</th>";
  }
  html += "</tr>";

  // วนลูปแสดงข้อมูลแต่ละแถว
  row_values *row = NULL;
  do {
    row = cur->get_next_row();
    if (row != NULL) {
      html += "<tr>";
      for (int i = 0; i < cols->num_fields; i++) {
        html += "<td>" + String(row->values[i] ? row->values[i] : "NULL") + "</td>";
      }
      html += "</tr>";
    }
  } while (row != NULL);

  // ปิด HTML
  html += "</table></body></html>";

  delete cur;                               // ลบ cursor คืนหน่วยความจำ
  server.send(200, "text/html", html);      // ส่ง HTML กลับให้ browser
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
  server.on("/", handleRoot);               // หากเข้าหน้าเว็บ root เรียกฟังก์ชัน handleRoot
  server.begin();                           // เริ่มต้น HTTP server
  Serial.println("HTTP server started");
}

////////////////////////////////////
//   Loop Function (ทำงานวนซ้ำ)
////////////////////////////////////
void loop() {

  server.handleClient();                 // ตรวจสอบว่ามี client เรียกหน้าเว็บหรือไม่

  delay(10000);                          // หน่วงเวลา 10 วินาที (จำลองส่งข้อมูลทุก 10 วิ)
}

////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////
  //   จำลองการส่งค่าจาก Sensor
  ////////////////////////////////////
  float value = random(20, 35);             // สุ่มค่าระหว่าง 20-35
  char query[256];                          // สร้าง buffer สำหรับ query SQL

  MySQL_Cursor *cur = new MySQL_Cursor(&conn);  // สร้าง Cursor ใหม่

  // สร้างคำสั่ง SQL INSERT ใส่ค่าลงในตาราง
  sprintf(query, "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor', %.2f)", value);

  // ถ้าเชื่อมต่อ MySQL สำเร็จ ให้ส่งข้อมูล
  if (conn.connected()) {
    Serial.println("Sending data...");
    cur->execute(query);
    Serial.println("Data inserted!");
  } else {
    Serial.println("MySQL not connected.");
  }

  delete cur;                            // ลบ cursor คืนหน่วยความจำ

  ////////////////////////////////////
  //   ตอบสนองคำขอ HTTP จาก browser
  ////////////////////////////////////

