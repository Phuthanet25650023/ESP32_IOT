///////////////////////////// lab 1 ///////////////////////////////////////
#include <WiFi.h>
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
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WebServer.h>

// WiFi
const char* ssid = "Elite_Ultimate_Archer_2.4G";
const char* password = "24776996";

// MySQL Server
IPAddress server_ip(192, 168, 81, 128);
char user[] = "cartoon2477";
char password_mysql[] = "24776996";
char database[] = "test";

WiFiClient client;
MySQL_Connection conn((Client *)&client);
WebServer server(80);

void handleRoot() {
  if (!conn.connected()) {
    if (!conn.connect(server_ip, 3306, user, password_mysql)) {
      server.send(500, "text/plain", "MySQL connection failed");
      return;
    }
  }

  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  cur->execute("USE test");
  cur->execute("SELECT id, sensor_name, value, created_at FROM sensors ORDER BY created_at DESC LIMIT 5");
  column_names *cols = cur->get_columns();

  // HTML + CSS + Auto-refresh
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta http-equiv="refresh" content="5">
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

  // Table headers
  for (int i = 0; i < cols->num_fields; i++) {
    html += "<th>" + String(cols->fields[i]->name) + "</th>";
  }
  html += "</tr>";

  // Table rows
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

  delete cur;
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (conn.connect(server_ip, 3306, user, password_mysql)) {
    Serial.println("MySQL connected");
  } else {
    Serial.println("MySQL connection failed");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
////////////////////////////////////////////////////////////////////////
