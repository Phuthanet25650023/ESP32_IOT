////////////////////////////////////////////////////// LAB //////////////////////////////////////////////////////////
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#define PIN_RECEIVER 35
#define LED 33
byte Backblock[] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};
byte Temp[] = {
  B01110,
  B01010,
  B01010,
  B01010,
  B11101,
  B11101,
  B11111,
  B01110
};
byte Next[] = {
  0x00,
  0x1F,
  0x04,
  0x0E,
  0x1F,
  0x04,
  0x0E,
  0x1F
};
byte Rev[] = { 
  0x00,
  0x1F,
  0x0E,
  0x04,
  0x1F,
  0x0E,
  0x04,
  0x1F
};
byte Play[] = {
  0x18,
  0x1C,
  0x1E,
  0x1F,
  0x1F,
  0x1E,
  0x1C,
  0x18
};
byte Add[] = {
  0x00,
  0x00,
  0x04,
  0x04,
  0x1F,
  0x04,
  0x04,
  0x00
};
byte De[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x00,
  0x00,
  0x00
};

IRrecv receiver(PIN_RECEIVER);
LiquidCrystal_I2C lcd ( 0x27, 20, 4 );
int Status = 0,Vol=0,VLED=0; 

void setup() {
  pinMode(LED, OUTPUT);
  receiver.enableIRIn();
  lcd.init ( );
  lcd.createChar(0, Temp);
  lcd.createChar(1, Next);
  lcd.createChar(2, Rev);
  lcd.createChar(3, Play);
  lcd.createChar(4, Add);
  lcd.createChar(5, De);
  lcd.createChar(6, Backblock);
  lcd.home();
  lcd.setCursor ( 0, 0 );
  
  lcd.print("Status : ???? ");

}

void loop() {
  if (receiver.decode())
  {
    VLED=map(Vol, 0, 19, 0, 255);
    lcd.setCursor ( 0, 0 );
    if (receiver.decodedIRData.command == 162 && Status == 0)
    {
      lcd.clear();
      lcd.print("Status : Power On");
      lcd.backlight ( );
      Status = 1;
    }
    else if (receiver.decodedIRData.command == 162 && Status == 1)
    {
      lcd.clear();
      lcd.print("Status : Power Off");
      lcd.noBacklight();
      Status = 0;
    }
    if(Status)
    {
      
      lcd.clear();
      lcd.setCursor ( 0, 0 );
      lcd.print("Status : Power On");
      lcd.setCursor ( 0, 1 );
        switch (receiver.decodedIRData.command)
        {
          case 2:
            lcd.print("Volume ");
            lcd.print(Vol);
            Vol=Vol+1;
            //if( Vol>=12)Vol=12;
            break;
          case 152:
            lcd.print("Volume ");
            lcd.print(Vol);
            Vol=Vol-1;
            if( Vol<=0)Vol=0;
            
            break;
          case 168:
            lcd.write(3);
            break;
          case 176:
            lcd.write(0);
            break;
          case 144:
            lcd.write(1);
            break;
          case 224:
            lcd.write(2);
            break;
          case 34 :
            lcd.print("Zero");
            break;
          case 104 :
            lcd.print("Phuthanet");
            break;
          case 48 :
            lcd.print("Mekpayom");
            break;
          case 24 :
            lcd.print("6211850006");
            break;
          case 122 :
            lcd.print("Makanakorn");
            break;
          case 16 :
            lcd.print("University");
            break;
          case 56 :
            lcd.print("Computer");
            break;
          case 90 :
            lcd.print("Engineering");
            break;
          case 66 :
            lcd.print("Bankkok");
            break;
          case 74 :
            lcd.print("10530");
            break;
          case 82 :
            lcd.print("Hello");
            break;
        }
        Function_Volume();
        analogWrite(LED,VLED);
    }
    receiver.resume();

  }

  delay(10);
}
void Function_Volume()
{
  lcd.setCursor (0,2);
  
  
  lcd.setCursor (0,3);
  lcd.write(5);

  lcd.setCursor (1,3);
  for(int i=0;i<Vol;i++)lcd.write(6);
  lcd.setCursor (2,2);
  if(Vol<=0)lcd.print("Mute");
  else if(Vol>12)lcd.print("DANGER !!!");
  else if(Vol>=19)lcd.print("MAXIMUM !!!");

  lcd.setCursor (19,3);
  lcd.write(4);
}
