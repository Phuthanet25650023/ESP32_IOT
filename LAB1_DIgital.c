////////////////////////////////// LAB1_1 /////////////////////////////////////////

int LED_PIN[] = {19, 18, 5, 17}; // กำหนดขา LED
int State[5][4] =
{
  {0, 1, 0, 1},
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {1, 0, 0, 1},
}; //กำหนด รูปแบบการกระพริบ 0 ดับ 1 ติด

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
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// LAB1_2 /////////////////////////////////////////
int LED_PIN[] = {19, 18, 5, 17 ,?};
int buttonPin1 = 26;
int buttonPin2 = 27;

int State[5][?] =
{
  {0, 1, 0, 1, 0},
  {1, 0, 0, 0, 1},
  {0, 1, 0, 0, 0},
  {0, 0, 1, 0, 1},
  {0, 0, 0, 1, 0},
};

void setup() {
  Serial.begin (115200);
  for (int i = 0; i < ?; i++) {
    pinMode(LED_PIN[i], OUTPUT);
  }
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLDOWN);
}
//digitalRead(buttonPin);
void loop() {
  for (int i = 0; i < 5; i++) {
    
    for (int j = 0; j < 5; j++) {
      digitalWrite(LED_PIN[j], State[i][j]);
    }
    if (digitalRead(buttonPin1) == LOW) 
    {
      Serial.println ("SLOW");
      delay(2000);
    }
    else if (digitalRead(buttonPin2) == HIGH)
    {
      Serial.println ("SPEED");
      delay(200);
    }
    else delay(1000); // รอ 1 วินาที
  }
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
