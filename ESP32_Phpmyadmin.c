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
const char* password = "********";

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
const char* password = "*********";                 // รหัสผ่าน WiFi

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
//   Clear-All Endpoint (/clear) - ลบข้อมูลเซนเซอร์ทั้งหมดในตาราง
////////////////////////////////////
void handleClear() {
  if (!conn.connected()) {
    if (!conn.connect(server_ip, 3306, user, password_mysql, database)) {
      server.send(500, "application/json", "{\"success\":false,\"error\":\"MySQL connection failed\"}");
      return;
    }
  }

  MySQL_Cursor *cur = new MySQL_Cursor(&conn);
  bool ok = cur->execute("DELETE FROM sensors");
  delete cur;

  if (ok) {
    Serial.println("All sensor data cleared.");
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    Serial.println("Failed to clear sensor data.");
    server.send(500, "application/json", "{\"success\":false,\"error\":\"DELETE failed\"}");
  }
}

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
  @keyframes pulseFade {
    0%   { opacity: 0.35; }
    100% { opacity: 1; }
  }
  .pulse { animation: pulseFade 0.5s ease; }
  .chart-card {
    max-width: 1100px;
    margin: 20px auto 0;
    background: var(--card);
    border: 1px solid var(--card-border);
    border-radius: 16px;
    padding: 20px;
    box-shadow: 0 4px 16px rgba(0,0,0,0.3);
  }
  .chart-head {
    display: flex;
    justify-content: space-between;
    align-items: center;
    flex-wrap: wrap;
    gap: 10px;
    margin-bottom: 12px;
  }
  .chart-head h2 {
    margin: 0;
    font-size: 1.05rem;
    color: var(--accent);
  }
  .chart-legend {
    display: flex;
    gap: 14px;
    flex-wrap: wrap;
  }
  .legend-item {
    display: flex;
    align-items: center;
    gap: 6px;
    font-size: 0.8rem;
    color: var(--muted);
  }
  .legend-dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    flex-shrink: 0;
  }
  .chart-wrap {
    position: relative;
  }
  #chart {
    width: 100%;
    height: 260px;
    display: block;
  }
  .chart-tooltip {
    position: absolute;
    pointer-events: none;
    background: #0f172a;
    border: 1px solid var(--card-border);
    border-radius: 8px;
    padding: 8px 10px;
    font-size: 0.75rem;
    color: var(--text);
    box-shadow: 0 4px 12px rgba(0,0,0,0.4);
    white-space: nowrap;
    transform: translate(-50%, -110%);
    z-index: 5;
  }
  .chart-tooltip .row { display: flex; align-items: center; gap: 6px; margin-top: 2px; }
  .chart-tooltip .row .dot { width: 8px; height: 8px; border-radius: 50%; flex-shrink: 0; }
  .chart-tooltip .time { color: var(--muted); font-size: 0.7rem; margin-bottom: 2px; }
  .btn-danger {
    background: #ef4444;
    color: #fff;
    border: none;
    padding: 8px 14px;
    border-radius: 8px;
    font-size: 0.85rem;
    font-family: inherit;
    cursor: pointer;
    transition: background 0.2s;
  }
  .btn-danger:hover { background: #dc2626; }
  .btn-danger:disabled { opacity: 0.5; cursor: not-allowed; }
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
    <button id="clearBtn" class="btn-danger">ล้างข้อมูลทั้งหมด</button>
  </header>

  <div id="cards"></div>

  <div class="chart-card">
    <div class="chart-head">
      <h2>แนวโน้มค่าที่วัดได้ (Trend)</h2>
      <div id="chartLegend" class="chart-legend"></div>
    </div>
    <div class="chart-wrap">
      <canvas id="chart"></canvas>
      <div id="chartTooltip" class="chart-tooltip" hidden></div>
    </div>
  </div>

  <footer id="updated">Last update: -</footer>

<script>
const REFRESH_MS = 2000;

function fmt(v) {
  return (v === null || v === undefined || v === '') ? '--' : Number(v).toFixed(2);
}

let cardsBuilt = false;

// สร้างโครงการ์ดแค่ครั้งเดียว (ไม่ทำลาย/สร้าง DOM ใหม่ทุกรอบ -> ไม่กระพริบ)
function buildCards(sensors) {
  const container = document.getElementById('cards');
  container.innerHTML = '';
  sensors.forEach(s => {
    const card = document.createElement('div');
    card.className = 'card';
    card.dataset.name = s.name;
    card.innerHTML =
      '<h2>' + s.name + '</h2>' +
      '<div class="time" data-f="time"></div>' +
      '<div class="latest" data-f="latest">--</div>' +
      '<div class="stats">' +
        '<div class="stat"><span class="label">MAX</span><span class="value max" data-f="max">--</span></div>' +
        '<div class="stat"><span class="label">MIN</span><span class="value min" data-f="min">--</span></div>' +
        '<div class="stat"><span class="label">AVG</span><span class="value avg" data-f="avg">--</span></div>' +
      '</div>';
    container.appendChild(card);
  });
  cardsBuilt = true;
}

// อัปเดตเฉพาะข้อความในการ์ดที่มีอยู่แล้ว พร้อม pulse เบา ๆ ตอนค่าเปลี่ยน
function updateField(card, field, value, pulse) {
  const el = card.querySelector('[data-f="' + field + '"]');
  if (!el || el.textContent === value) return;
  el.textContent = value;
  if (pulse) {
    el.classList.remove('pulse');
    void el.offsetWidth;   // force reflow เพื่อให้ animation เล่นใหม่ทุกครั้ง
    el.classList.add('pulse');
  }
}

function render(sensors) {
  if (!cardsBuilt) buildCards(sensors);

  sensors.forEach(s => {
    const card = document.querySelector('.card[data-name="' + s.name + '"]');
    if (!card) return;
    updateField(card, 'time', s.time || '');
    updateField(card, 'latest', fmt(s.latest), true);
    updateField(card, 'max', fmt(s.max), true);
    updateField(card, 'min', fmt(s.min), true);
    updateField(card, 'avg', fmt(s.avg), true);
  });

  document.getElementById('updated').textContent = 'Last update: ' + new Date().toLocaleTimeString();
}

function setStatus(online) {
  document.getElementById('dot').className = online ? 'dot online' : 'dot';
  document.getElementById('statusText').textContent = online ? 'Online' : 'Offline';
}

////////////////////////////////////
//   Trend Line Chart (Canvas, ไม่พึ่ง library ภายนอก)
////////////////////////////////////
const CHART_COLORS = ['#3987e5', '#d95926', '#199e70'];   // categorical palette (dark-mode steps)
const CHART_SURFACE = '#1e293b';
const CHART_GRID = '#334155';
const CHART_AXIS = '#475569';
const CHART_MUTED = '#94a3b8';
const CHART_TEXT = '#e2e8f0';
const MAX_POINTS = 20;

const history = {};   // { sensorName: [{t: epochMs, v: number}] }
let legendBuilt = false;
let hoverPx = null;

function parseServerTime(s) {
  const iso = s.indexOf('T') >= 0 ? s : s.replace(' ', 'T');
  const ms = Date.parse(iso);
  return isNaN(ms) ? Date.now() : ms;
}

function updateHistory(sensors) {
  sensors.forEach(s => {
    if (s.latest === null || s.latest === undefined || !s.time) return;
    const arr = history[s.name] || (history[s.name] = []);
    const last = arr[arr.length - 1];
    if (!last || last.timeStr !== s.time) {
      arr.push({ t: parseServerTime(s.time), v: Number(s.latest), timeStr: s.time });
      if (arr.length > MAX_POINTS) arr.shift();
    }
  });
}

function renderLegend(names) {
  const el = document.getElementById('chartLegend');
  el.innerHTML = names.map((name, i) => {
    const color = CHART_COLORS[i % CHART_COLORS.length];
    return '<span class="legend-item"><span class="legend-dot" style="background:' + color + '"></span>' + name + '</span>';
  }).join('');
  legendBuilt = true;
}

function niceStep(rawStep) {
  const pow = Math.pow(10, Math.floor(Math.log10(rawStep || 1)));
  const n = rawStep / pow;
  let f;
  if (n <= 1) f = 1; else if (n <= 2) f = 2; else if (n <= 5) f = 5; else f = 10;
  return f * pow;
}

function drawChart() {
  const canvas = document.getElementById('chart');
  const tooltip = document.getElementById('chartTooltip');
  const ctx = canvas.getContext('2d');
  const dpr = window.devicePixelRatio || 1;
  const cssW = canvas.clientWidth || 600;
  const cssH = canvas.clientHeight || 260;
  if (canvas.width !== Math.round(cssW * dpr) || canvas.height !== Math.round(cssH * dpr)) {
    canvas.width = Math.round(cssW * dpr);
    canvas.height = Math.round(cssH * dpr);
  }
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  ctx.clearRect(0, 0, cssW, cssH);

  const names = Object.keys(history);
  if (!legendBuilt && names.length) renderLegend(names);

  const allPoints = names.flatMap(n => history[n]);
  if (allPoints.length < 2) {
    ctx.fillStyle = CHART_MUTED;
    ctx.font = '13px "Segoe UI", Arial, sans-serif';
    ctx.fillText('กำลังรอข้อมูล...', 12, 20);
    tooltip.hidden = true;
    return;
  }

  const padL = 54, padR = 16, padT = 12, padB = 26;
  const plotW = Math.max(10, cssW - padL - padR);
  const plotH = Math.max(10, cssH - padT - padB);

  let vMin = Math.min(...allPoints.map(p => p.v));
  let vMax = Math.max(...allPoints.map(p => p.v));
  if (vMin === vMax) { vMin -= 1; vMax += 1; }
  const vPad = (vMax - vMin) * 0.12;
  vMin -= vPad; vMax += vPad;

  const tMin = Math.min(...allPoints.map(p => p.t));
  let tMax = Math.max(...allPoints.map(p => p.t));
  if (tMin === tMax) tMax = tMin + 1;

  const xOf = t => padL + (t - tMin) / (tMax - tMin) * plotW;
  const yOf = v => padT + (1 - (v - vMin) / (vMax - vMin)) * plotH;

  // ---- Y gridlines + labels (ปัดเป็นตัวเลขกลม ๆ) ----
  const rawStep = (vMax - vMin) / 4;
  const step = niceStep(rawStep) || 1;
  const firstTick = Math.ceil(vMin / step) * step;
  ctx.strokeStyle = CHART_GRID;
  ctx.lineWidth = 1;
  ctx.fillStyle = CHART_MUTED;
  ctx.font = '11px "Segoe UI", Arial, sans-serif';
  ctx.textAlign = 'right';
  ctx.textBaseline = 'middle';
  for (let v = firstTick; v <= vMax; v += step) {
    const y = yOf(v);
    ctx.beginPath();
    ctx.moveTo(padL, y);
    ctx.lineTo(padL + plotW, y);
    ctx.stroke();
    ctx.fillText(v.toFixed(step < 1 ? 2 : 0), padL - 8, y);
  }

  // ---- X ticks (เวลา) ----
  ctx.textAlign = 'center';
  ctx.textBaseline = 'top';
  const XTICKS = Math.min(5, allPoints.length);
  for (let i = 0; i <= XTICKS; i++) {
    const t = tMin + (tMax - tMin) * i / XTICKS;
    const x = xOf(t);
    const label = new Date(t).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    ctx.fillText(label, Math.min(Math.max(x, padL + 20), padL + plotW - 20), padT + plotH + 8);
  }

  // ---- แกน ----
  ctx.strokeStyle = CHART_AXIS;
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(padL, padT + plotH);
  ctx.lineTo(padL + plotW, padT + plotH);
  ctx.stroke();
  ctx.beginPath();
  ctx.moveTo(padL, padT);
  ctx.lineTo(padL, padT + plotH);
  ctx.stroke();

  // ---- เส้นกราฟ + จุด (Point) ของทั้ง 3 เซนเซอร์ ----
  names.forEach((name, idx) => {
    const pts = history[name];
    if (!pts || pts.length === 0) return;
    const color = CHART_COLORS[idx % CHART_COLORS.length];

    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.lineJoin = 'round';
    ctx.lineCap = 'round';
    ctx.beginPath();
    pts.forEach((p, i) => {
      const x = xOf(p.t), y = yOf(p.v);
      if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    });
    ctx.stroke();

    pts.forEach(p => {
      const x = xOf(p.t), y = yOf(p.v);
      ctx.beginPath();
      ctx.arc(x, y, 6, 0, Math.PI * 2);
      ctx.fillStyle = CHART_SURFACE;      // surface ring คั่นจุดกับเส้น
      ctx.fill();
      ctx.beginPath();
      ctx.arc(x, y, 4, 0, Math.PI * 2);
      ctx.fillStyle = color;
      ctx.fill();
    });
  });

  // ---- Hover crosshair + tooltip ----
  if (hoverPx !== null && hoverPx >= padL && hoverPx <= padL + plotW) {
    const tAtHover = tMin + (hoverPx - padL) / plotW * (tMax - tMin);

    ctx.strokeStyle = CHART_MUTED;
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(hoverPx, padT);
    ctx.lineTo(hoverPx, padT + plotH);
    ctx.stroke();

    let rows = '';
    let anyTimeStr = '';
    names.forEach((name, idx) => {
      const pts = history[name];
      if (!pts || pts.length === 0) return;
      let nearest = pts[0];
      pts.forEach(p => { if (Math.abs(p.t - tAtHover) < Math.abs(nearest.t - tAtHover)) nearest = p; });
      anyTimeStr = nearest.timeStr;
      const color = CHART_COLORS[idx % CHART_COLORS.length];
      rows += '<div class="row"><span class="dot" style="background:' + color + '"></span>' +
              name + ': ' + nearest.v.toFixed(2) + '</div>';
    });

    tooltip.innerHTML = '<div class="time">' + anyTimeStr + '</div>' + rows;
    tooltip.hidden = false;
    tooltip.style.left = hoverPx + 'px';
    tooltip.style.top = padT + 'px';
  } else {
    tooltip.hidden = true;
  }
}

const chartCanvasEl = document.getElementById('chart');
chartCanvasEl.addEventListener('mousemove', (e) => {
  const rect = chartCanvasEl.getBoundingClientRect();
  hoverPx = e.clientX - rect.left;
  drawChart();
});
chartCanvasEl.addEventListener('mouseleave', () => {
  hoverPx = null;
  drawChart();
});
window.addEventListener('resize', drawChart);

async function fetchData() {
  try {
    const res = await fetch('/data');
    const data = await res.json();
    render(data.sensors);
    updateHistory(data.sensors);
    drawChart();
    setStatus(true);
  } catch (e) {
    setStatus(false);
  }
}

////////////////////////////////////
//   ปุ่มล้างข้อมูลทั้งหมดในฐานข้อมูล
////////////////////////////////////
document.getElementById('clearBtn').addEventListener('click', async () => {
  if (!confirm('ยืนยันลบข้อมูลเซนเซอร์ทั้งหมดในฐานข้อมูล?\nการกระทำนี้ไม่สามารถย้อนกลับได้')) return;

  const btn = document.getElementById('clearBtn');
  btn.disabled = true;
  btn.textContent = 'กำลังลบ...';

  try {
    const res = await fetch('/clear', { method: 'POST' });
    const data = await res.json();
    if (data.success) {
      // เคลียร์สถานะฝั่ง browser ให้ตรงกับฐานข้อมูลทันที
      Object.keys(history).forEach(k => delete history[k]);
      document.querySelectorAll('.card').forEach(card => {
        updateField(card, 'time', '');
        updateField(card, 'latest', '--');
        updateField(card, 'max', '--');
        updateField(card, 'min', '--');
        updateField(card, 'avg', '--');
      });
      drawChart();
      alert('ลบข้อมูลทั้งหมดเรียบร้อยแล้ว');
    } else {
      alert('ลบข้อมูลไม่สำเร็จ: ' + (data.error || 'unknown error'));
    }
  } catch (e) {
    alert('เชื่อมต่อกับ ESP32 ไม่สำเร็จ');
  } finally {
    btn.disabled = false;
    btn.textContent = 'ล้างข้อมูลทั้งหมด';
  }
});

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
  server.on("/clear", HTTP_POST, handleClear); // API ลบข้อมูลเซนเซอร์ทั้งหมด
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
