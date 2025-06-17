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
void DataMoveL()
{
  lcd.scrollDisplayLeft(); 
  delay(250);
}
void DataMoveR()
{
  lcd.scrollDisplayRight(); 
  delay(250);
}

//////////////////////////////////////////////////////////////////////////////
//https://maxpromer.github.io/LCD-Character-Creator/
//https://chareditor.com/



#include <LiquidCrystal_I2C.h>

// สร้างตัวอักษรพิเศษ love1 และ love2
byte love1[] = { B01010, B11111, B01110, B00100, B00000, B00000, B01010, B11111};
byte love2[] = { B01110, B00100, B00000, B00000, B01010, B11111, B01110, B00100};
byte vlove[]={B00000, B00000, B01010, B11111, B11111, B01110, B00100, B00000};
byte loveinterrupt1[] {B10101, B00000, B10001, B11011, B11111, B11111, B10101, B00000};
byte loveinterrupt2[] {B10001, B11011, B11111, B11111, B10101, B00000, B10001, B11011};
// สร้างวัตถุ LCD (I2C address 0x27, จอ 20x4)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();           // เริ่มต้น LCD
  lcd.backlight();      // เปิดไฟพื้นหลัง
  lcd.createChar(4, vlove); 
  lcd.createChar(0, love1);  // สร้างตัวอักษรที่ตำแหน่ง 0
  lcd.createChar(1, love2);  // สร้างตัวอักษรที่ตำแหน่ง 1
  lcd.createChar(2, loveinterrupt1);  // สร้างตัวอักษรที่ตำแหน่ง 0
  lcd.createChar(3, loveinterrupt2);  // สร้างตัวอักษรที่ตำแหน่ง 1
}

void loop() {
  switch (random(5))
  {
    case 1:
      Switch2();
      break;
    case 2:
      Switch3();
      break;
    case 3:
      Switch4();
      break;
    case 0:
      Switch1();
      break;
    case 4:
      Switch5();
      break;
    lcd.clear();

  }

}
void Switch5()
{
  for (int i = 16 ; i > 0 ; i--)
  {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.write(byte(2));
    lcd.setCursor(i, 1);
    lcd.write(byte(3));
    delay(100);
  }
}
void Switch4()
{
  lcd.clear();
  for (int i = 0 ; i < 32 ; i++)
  {
    lcd.setCursor(random(100) % 16, 0);
    lcd.write(byte(4));
    lcd.setCursor(random(100) % 16, 1);
    lcd.write(byte(4));
    delay(100);
  }

}
void Switch3()
{
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.write(byte(0));
    lcd.setCursor(i, 1);
    lcd.write(byte(1));
    delay(100);
  }
}
void Switch2()
{
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.setCursor(i, 0);
    if (i % 2 == 0) lcd.write(byte(0));

    lcd.setCursor(i, 1);
    if (i % 2 == 0) lcd.write(byte(1));

    //delay(100);
  }
  delay(500);
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.setCursor(i, 0);
    if (i % 2 == 0) lcd.write(byte(2));

    lcd.setCursor(i, 1);
    if (i % 2 == 0) lcd.write(byte(3));

    //delay(100);
  }
  delay(500);
}

void Switch1()
{
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.setCursor(i, 0);
    if (i % 2 == 0) lcd.write(byte(0));
    else lcd.write(byte(2));
    lcd.setCursor(i, 1);
    if (i % 2 == 0) lcd.write(byte(1));
    else lcd.write(byte(3));
    //delay(100);
  }
  delay(500);
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.setCursor(i, 0);
    if (i % 2 == 0) lcd.write(byte(2));
    else lcd.write(byte(0));
    lcd.setCursor(i, 1);
    if (i % 2 == 0) lcd.write(byte(3));
    else lcd.write(byte(1));
    //delay(100);
  }
  delay(500);
}

//////////////////////////////////////////////////////////////////////////
