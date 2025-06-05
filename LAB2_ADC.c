/////////////////////////////////////LAB2_0////////////////////////////////////
int LED_PIN = 16;
int vol=12;
float k1=0.98,volume=0;
void filter_sink_Voltage_V(float value) {
  volume = ((value) * (1 - k1)) + volume * k1;
} //Low Pass Filter

void setup() {
  Serial.begin (115200); // เริ่มต้น Serial Communication เพื่อดูค่าบน Serial Monitor ที่ baud rate 115200
  pinMode(LED_PIN, OUTPUT);
  pinMode(vol,INPUT);// ตั้งค่า GPIO 34 เป็น INPUT สำหรับอ่านค่า Analog

}
void loop() {
  filter_sink_Voltage_V(analogRead(vol)); // ค่าอยู่ในช่วง 0 - 4095 (12-bit)
  int Data_volume= map(volume, 0, 4095, 0, 255); // // แปลงจาก 12 บิต เป็น 8 บิต
  analogWrite(LED_PIN,Data_volume); 
  Serial.print("Raw ADC: ");
  Serial.print(volume);
  Serial.print(" | Mapped: ");
  Serial.println(Data_volume);
  delay(100); // รอ 1 วินาที
  
}
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// LAB2_1////////////////////////////////////
int LED_PIN[] = {19, 18, 5, 17 ,16};
int vol=34;

void setup() {
  Serial.begin (115200); // เริ่มต้น Serial Communication เพื่อดูค่าบน Serial Monitor ที่ baud rate 115200

  for (int i = 0; i < 4; i++) {
    pinMode(LED_PIN[i], OUTPUT);
  }
  pinMode(vol,INPUT);// ตั้งค่า GPIO 34 เป็น INPUT สำหรับอ่านค่า Analog
  
}
void loop() {
  int volume=analogRead(vol); // ค่าอยู่ในช่วง 0 - 4095 (12-bit)
  int Data_volume= map(volume, 0, 4095, 0, 255); // // แปลงจาก 12 บิต เป็น 8 บิต
  analogWrite(LED_PIN[0],Data_volume); 
  Serial.print("Raw ADC: ");
  Serial.print(volume);
  Serial.print(" | Mapped: ");
  Serial.println(Data_volume);
  delay(100); // รอ 1 วินาที
  
}
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// LAB2_2 ////////////////////////////////////
int LED_PIN[] = {19, 18, 5, 17 ,16};  // กำหนดขา GPIO สำหรับ LED 5 ตัว (แต่ใช้แค่ 4 ตัวในโค้ด)
int vol=34;                           // กำหนดขา GPIO 34 สำหรับอ่านค่า Analog (volume sensor)
int TRIGGER_PIN = 25;                 // ขา Trigger ของเซ็นเซอร์วัดระยะทาง Ultrasonic
int ECHO_PIN = 33;                    // ขา Echo ของเซ็นเซอร์วัดระยะทาง Ultrasonic

void setup() {
  Serial.begin (115200);              // เริ่มต้นการสื่อสาร Serial ที่ baud rate 115200 เพื่อส่งข้อมูลขึ้น Serial Monitor
  
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PIN[i], OUTPUT);     // ตั้งค่าขา LED ทั้ง 4 ขาเป็น OUTPUT เพื่อควบคุม LED
  }
  
  pinMode(vol, INPUT);               // ตั้งค่าขา GPIO 34 เป็น INPUT สำหรับอ่านค่าสัญญาณ Analog (volume sensor)
  pinMode(TRIGGER_PIN, OUTPUT);      // ตั้งค่าขา Trigger เป็น OUTPUT เพื่อส่งสัญญาณ Ultrasonic
  pinMode(ECHO_PIN, INPUT);          // ตั้งค่าขา Echo เป็น INPUT เพื่อรับสัญญาณ Ultrasonic ที่สะท้อนกลับ
}

void loop() {
  int volume = procress();            // เรียกฟังก์ชัน procress() เพื่อวัดระยะทางจากเซ็นเซอร์ Ultrasonic
  
  int Data_volume = map(volume, 0, 400, 0, 255);  // แปลงค่าระยะทาง (0-400 cm) ให้อยู่ในช่วง 0-255 (สำหรับ PWM)
  
  analogWrite(LED_PIN[0], Data_volume); // ปรับความสว่าง LED ตัวแรกตามค่าที่แปลงได้ (PWM)
  
  Serial.print("cm : ");               // แสดงข้อความ "cm : " บน Serial Monitor
  Serial.print(volume);                // แสดงค่าระยะทางที่วัดได้
  Serial.print(" | Mapped: ");         // แสดงข้อความ " | Mapped: "
  Serial.println(Data_volume);         // แสดงค่าที่ถูกแปลงเป็นช่วง 0-255 และขึ้นบรรทัดใหม่
  
  delay(100);                         // หน่วงเวลา 100 มิลลิวินาที (0.1 วินาที) ก่อนทำงานรอบถัดไป
}

int procress() {
  digitalWrite(TRIGGER_PIN, LOW);     // ปิดสัญญาณ Trigger เพื่อเตรียมส่งคลื่น Ultrasonic
  delayMicroseconds(2);               // หน่วงเวลา 2 ไมโครวินาที
  
  digitalWrite(TRIGGER_PIN, HIGH);    // ส่งสัญญาณ Trigger สูง เป็นเวลา 10 ไมโครวินาที เพื่อปล่อยคลื่น Ultrasonic
  delayMicroseconds(10);              // หน่วงเวลา 10 ไมโครวินาที
  digitalWrite(TRIGGER_PIN, LOW);     // ปิดสัญญาณ Trigger
  
  long durasi = pulseIn(ECHO_PIN, HIGH);  // วัดระยะเวลาที่ Echo ขึ้นเป็น HIGH (หน่วยไมโครวินาที) คือเวลาคลื่นสะท้อนกลับ
  
  return durasi * 0.0343 / 2;          // แปลงเวลาที่วัดได้เป็นระยะทาง (เซนติเมตร)
  // 0.0343 = ความเร็วเสียง (cm/microsecond)  หาร 2 เพราะเวลาที่วัดเป็นไป-กลับ
}
///////////////////////////////////////////////////////////////////////////////
