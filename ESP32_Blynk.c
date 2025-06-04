/////////////////////////////////////////////////////////////////////////// LAB 01 //////////////////////////////////////////////////////////////////////////
/*************************************************************

  ตัวอย่างง่ายๆ สำหรับส่งและรับข้อมูลกับ Blynk  
  รองรับบอร์ด ESP8266 และ ESP32 โดยใช้ #define กำหนดบอร์ด

*************************************************************/

/* กรอกข้อมูลจาก Blynk Device Info */
#define BLYNK_TEMPLATE_ID           "TMPL6N52EScLc"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "j_pcDvPSFqHM8KC5Yq0vB9Pk1B6CnIOx"

/* เลือกบอร์ดโดยเอาคอมเมนต์ออกเฉพาะบรรทัดเดียวที่ใช้ */
//#define ESP8266
#define ESP32

/* คอมเมนต์บรรทัดนี้เพื่อปิดการแสดงผลข้อความ Debug */
#define BLYNK_PRINT Serial

#if defined(ESP8266)
  #include <ESP8266WiFi.h>          // ไลบรารี WiFi สำหรับ ESP8266
  #include <BlynkSimpleEsp8266.h>   // ไลบรารี Blynk สำหรับ ESP8266
#elif defined(ESP32)
  #include <WiFi.h>                 // ไลบรารี WiFi สำหรับ ESP32
  #include <BlynkSimpleEsp32.h>     // ไลบรารี Blynk สำหรับ ESP32
#else
  #error "กรุณากำหนดบอร์ด ESP8266 หรือ ESP32 ในโค้ด"
#endif

// ตั้งชื่อ WiFi และรหัสผ่าน
char ssid[] = "Elite_Ultimate_Archer_2.4G";
char pass[] = "24776996";

BlynkTimer timer;

// ฟังก์ชันอ่านค่าจาก Virtual Pin 0 เมื่อค่ามีการเปลี่ยนแปลง
BLYNK_WRITE(V0)
{
  int value = param.asInt();
  Blynk.virtualWrite(V1, value);  // ส่งค่าที่รับมาไปที่ Virtual Pin 1
}

// ฟังก์ชันเรียกเมื่อเชื่อมต่อกับ Blynk Cloud สำเร็จ
BLYNK_CONNECTED()
{
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// ฟังก์ชันส่งค่าเวลาทำงานของบอร์ด (วินาที) ไปที่ Virtual Pin 2 ทุกวินาที
void myTimerEvent()
{
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  // เริ่มเชื่อมต่อ Blynk กับ WiFi
  timer.setInterval(1000L, myTimerEvent);     // ตั้งเวลาส่งข้อมูลทุก 1 วินาที
}

void loop()
{
  Blynk.run();
  timer.run();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////// LAB 02 //////////////////////////////////////////////////////////////////////////
/*************************************************************
  ตัวอย่างแยก Sensor 5 ตัว ทำงานแยกกัน ส่งค่าขึ้น Blynk
  อ่านค่าจาก ADC ขา A0 ถึง A4 บน ESP32
*************************************************************/

#define BLYNK_TEMPLATE_ID           "TMPL6N52EScLc"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "j_pcDvPSFqHM8KC5Yq0vB9Pk1B6CnIOx"

// เลือกบอร์ด ESP32 หรือ ESP8266
//#define ESP8266
#define ESP32

#define BLYNK_PRINT Serial

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <BlynkSimpleEsp32.h>
#else
  #error "กรุณากำหนดบอร์ด ESP8266 หรือ ESP32"
#endif

char ssid[] = "Elite_Ultimate_Archer_2.4G";
char pass[] = "24776996";

BlynkTimer timer;

// กำหนดขา ADC (ESP32 ใช้ GPIO แทน A0-A4)
const int sensorPins[5] = {36, 39, 34, 35, 32};  // A0=GPIO36, A1=GPIO39, A2=GPIO34, A3=GPIO35, A4=GPIO32

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void connectBlynk() {
  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), pass);
  Serial.println("Connecting to Blynk...");
  while (!Blynk.connected()) {
    Blynk.run();
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nBlynk connected!");
}

// ส่งค่า ADC sensor 1-5 แยกฟังก์ชัน
void sendSensor1() {
  int val = random(0, 100);//analogRead(sensorPins[0]);
  Blynk.virtualWrite(V10, val);
  Serial.print("Sensor1 (A0) = "); Serial.println(val);
}

void sendSensor2() {
  int val = random(0, 1000);//analogRead(sensorPins[1]);
  Blynk.virtualWrite(V11, val);
  Serial.print("Sensor2 (A1) = "); Serial.println(val);
}

void sendSensor3() {
  int val = random(0, 2000);//analogRead(sensorPins[2]);
  Blynk.virtualWrite(V12, val);
  Serial.print("Sensor3 (A2) = "); Serial.println(val);
}

void sendSensor4() {
  int val = random(0, 3000);//analogRead(sensorPins[3]);
  Blynk.virtualWrite(V13, val);
  Serial.print("Sensor4 (A3) = "); Serial.println(val);
}



void setup() {
  Serial.begin(115200);
  connectWiFi();

  // ไม่ต้องใช้ randomSeed เพราะอ่านค่าจริง

  connectBlynk();

  timer.setInterval(10000L, sendSensor1);    // Sensor 1 ทุก 10 วินาที
  timer.setInterval(11000L, sendSensor2);    // Sensor 2 ทุก 11 วินาที
  timer.setInterval(12000L, sendSensor3);    // Sensor 3 ทุก 12 วินาที
  timer.setInterval(12500L, sendSensor4);    // Sensor 4 ทุก 12.5 วินาที
 
}

void loop() {
  Blynk.run();
  timer.run();
}

