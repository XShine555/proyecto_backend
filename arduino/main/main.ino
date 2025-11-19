#include "secrets.h"
#include <SPI.h>
#include <MFRC522.h>

void setup() {
  Serial.begin(9600);
  Serial.println("eFuse Mac: " + getEfuseMac());

  setupLed();    // initialize LED (PIN 15)
  setupLcd();
  setupRfid();
  setupAWS();
  
  connectToWiFi();
}

void loop() {
  loopWiFi();
  loopAWS();

  loopRfid();
  loopLcd();

  loopLed();     // handle LED state machine
}

String getEfuseMac() {
  uint64_t chipid = ESP.getEfuseMac();
  String mac = String((uint16_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
  mac.toUpperCase();
  return mac;
}