#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// กำหนดขา GPIO สำหรับปุ่ม
const int buttonPin1 = 17;
const int buttonPin2 = 16;

void setup() {
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();
  Serial.begin(115200);

  // กำหนดโหมดของปุ่มเป็น input
  pinMode(buttonPin1, INPUT_PULLUP); // ใช้ pull-up เพื่อให้ปุ่มทำงานได้ดี
  pinMode(buttonPin2, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Start Up ");
  Serial.println("Hello, ESP32!");
}

void loop() {
  // อ่านค่าปุ่ม (LOW = กด)
  bool button1Pressed = digitalRead(buttonPin1) == LOW;
  bool button2Pressed = digitalRead(buttonPin2) == LOW;

  // แสดงสถานะของปุ่มบน Serial
  if (button1Pressed) {
    Serial.println("Button 1 (GPIO 17) Pressed");
    lcd.setCursor(0, 1);
    lcd.print("Button 1 Pressed   ");
  }

  if (button2Pressed) {
    Serial.println("Button 2 (GPIO 16) Pressed");
    lcd.setCursor(0, 2);
    lcd.print("Button 2 Pressed   ");
  }

  // ถ้าไม่มีปุ่มถูกกด ให้ล้างข้อความ
  if (!button1Pressed) {
    lcd.setCursor(0, 1);
    lcd.print("                  "); // ล้างแถว
  }
  if (!button2Pressed) {
    lcd.setCursor(0, 2);
    lcd.print("                  "); // ล้างแถว
  }

  delay(200); // หน่วงเวลาเล็กน้อยเพื่อลดการแสดงผลซ้ำ
}
////////////////////////////////////////////////////////////////////////////////
