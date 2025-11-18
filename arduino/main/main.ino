#include "secrets.h"
#include <SPI.h>
#include <MFRC522.h>

void setup() {
  Serial.begin(9600);
  Serial.println("eFuse Mac: " + getEfuseMac());

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
}

String getEfuseMac() {
  uint64_t chipid = ESP.getEfuseMac();
  String mac = String((uint16_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
  mac.toUpperCase();
  return mac;
}