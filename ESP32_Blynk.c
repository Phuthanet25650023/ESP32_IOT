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
