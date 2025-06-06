//////////////////////////// lab 1 ///////////////////////////////////////
/*#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// WiFi
const char* ssid = "Elite_Ultimate_Archer_2.4G";
const char* password = "24776996";

// MySQL Server 192.168.81.244
IPAddress server_ip(192, 168, 81, 128); // IP ของ MySQL Server
char user[] = "cartoon2477";
char password_mysql[] = "24776996";
char database[] = "test";

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
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("USE test");

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
///////////////////////////// lab 2 ///////////////////////////////////////
#include <WiFi.h>                    // สำหรับเชื่อมต่อ WiFi
#include <MySQL_Connection.h>       // สำหรับเชื่อมต่อ MySQL Server
#include <MySQL_Cursor.h>           // สำหรับส่ง query SQL
#include <WebServer.h>              // สำหรับสร้าง HTTP Web Server บน ESP32

// ✅ ตั้งค่า WiFi
const char* ssid = "Elite_Ultimate_Archer_2.4G";     // ชื่อ WiFi
const char* password = "24776996";                   // รหัสผ่าน WiFi

// ✅ ตั้งค่า MySQL Server
IPAddress server_ip(192, 168, 0, 146);                // IP ของ MySQL Server (เครื่องปลายทาง)
char user[] = "cartoon2477";                         // ชื่อผู้ใช้ MySQL
char password_mysql[] = "24776996";                  // รหัสผ่าน MySQL
char database[] = "test";                            // ชื่อฐานข้อมูล

// ✅ สร้างตัวแปรสำหรับเชื่อมต่อ
WiFiClient client;
MySQL_Connection conn((Client *)&client);
WebServer server(80);                                // สร้าง Web Server ที่พอร์ต 80

// ✅ ฟังก์ชันเมื่อเข้าเว็บ root "/"
void handleRoot() {
  // ถ้ายังไม่เชื่อมต่อ MySQL ให้เชื่อมใหม่
  if (!conn.connected()) {
    if (!conn.connect(server_ip, 3306, user, password_mysql)) {
      server.send(500, "text/plain", "MySQL connection failed");
      return;
    }
  }

  // สร้าง Cursor และรัน query ดึงข้อมูล
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("USE test");    // เลือก database
  cur->execute("SELECT sensor_name, value, created_at FROM sensors ORDER BY created_at DESC LIMIT 5");

  column_names *cols = cur->get_columns();  // เอาชื่อคอลัมน์

  // ✅ เริ่มสร้าง HTML
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta http-equiv="refresh" content="5"><!-- ทำให้รีเฟรชทุกๆ 5 วิ -->
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

  // ✅ เพิ่มหัวตารางจากชื่อคอลัมน์
  for (int i = 0; i < cols->num_fields; i++) {
    html += "<th>" + String(cols->fields[i]->name) + "</th>";
  }
  html += "</tr>";

  // ✅ แสดงข้อมูลในตาราง
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

  html += "</table></body></html>";

  delete cur;  // ลบ cursor เพื่อคืนหน่วยความจำ
  server.send(200, "text/html", html);  // ส่งเว็บให้ browser
}

// ✅ ฟังก์ชัน setup() เริ่มทำงานเมื่อเปิดเครื่อง
void setup() {
  Serial.begin(115200);                // เริ่ม serial monitor
  WiFi.begin(ssid, password);          // เชื่อมต่อ WiFi
  Serial.print("Connecting to WiFi");

  // รอจนกว่าจะเชื่อมต่อสำเร็จ
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());      // แสดง IP ที่ได้

  // เชื่อมต่อ MySQL ตอนเริ่มต้น
  if (conn.connect(server_ip, 3306, user, password_mysql)) {
    Serial.println("MySQL connected");
  } else {
    Serial.println("MySQL connection failed");
  }
  
  // ตั้ง route ของเว็บ
  server.on("/", handleRoot);
  server.begin();                      // เริ่ม HTTP server
  Serial.println("HTTP server started");
}

// ✅ ฟังก์ชัน loop() จะทำงานตลอดเวลา
void loop() {
  //////////////////////////////////////  เพิ่มข้อมูล   /////////////////////////////////////
  float value = random(20, 35); // จำลองค่าจาก sensor
  char query[256];

  // เพิ่มเลือกฐานข้อมูลก่อน (optional แต่แนะนำให้ชัดเจน)
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("USE test");

  // สร้างคำสั่ง INSERT เข้ากับตาราง sensors
  sprintf(query, "INSERT INTO sensors (sensor_name, value) VALUES ('TempSensor', %.2f)", value); //ข้อความก่อนจะทำการ InSert 

  if (conn.connected()) {
    Serial.println("Sending data...");
    cur->execute(query);
    Serial.println("Data inserted!");
  } else {
    Serial.println("MySQL not connected.");
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  server.handleClient();               // รอและตอบสนองคำขอจาก browser
  delay(10000);
}

////////////////////////////////////////////////////////////////////////
