#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArtronShop_LineMessaging.h>

#define WIFI_SSID "Elite_ULtimate_2.4G" // WiFi Name
#define WIFI_PASSWORD "24776996" // WiFi Password

#define LINE_TOKEN "fsmvBz/xXpsRxnyQrZTKPuc3Aw6TDJeR67Q5vcuUeD7dqcBoqxwtVDfYXTLqFxhC7/3AzJXnhUFqPUuu6TF2PEb5uJHnUwk9BJ61RNzzK1hm+FCi67hWMPUttq2SWLrk97LgF8aVJubZa5SUNeu3uQdB04t89/1O/w1cDnyilFU=" // Channel access token

WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((wifiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  LINE.begin(LINE_TOKEN);

  if (LINE.send("U30199332a9032acc6ff7b548e7246438", "Hello from ESP32 !")) { // Send "Hello from ESP32 !" to LINE with User/Group ID
    Serial.println("Send notify successful");
  } else {
    Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
  }
}

void loop() {
  
}
