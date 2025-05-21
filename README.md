# ESP32_IOT

int LED_PIN[] = {19, 18, 5, 17};
int State[5][4] =
{
  {0, 1, 0, 1},
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1},
};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PIN[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(LED_PIN[j], State[i][j]);
    }
    delay(1000); // รอ 1 วินาที
  }
}
