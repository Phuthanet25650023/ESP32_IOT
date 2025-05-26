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
