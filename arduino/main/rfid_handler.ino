#include <ArduinoJson.h>

// Forward declarations for LED control
void ledSuccess();
void ledError();

#define RFID_LOG_PREFIX "RFID"
#define RFID_MESSAGE_COOLDOWN 5000

#define RFID_SDA_PIN 4
#define RFID_RST_PIN 16
#define RFID_SCK_PIN 18
#define RFID_MOSI_PIN 23
#define RFID_MISO_PIN 19
MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);

String rfidLastUidChecked = "";
bool rfidIsWaitingForMessage = false;
unsigned long rfidWaitingMessageTime = 0;

void rfidOnAwsMessageReceived(JsonDocument& doc);

void rfidOnAwsMessageReceived(JsonDocument& doc) {
  logDebug(RFID_LOG_PREFIX, "Message received.");
  rfidIsWaitingForMessage = false;
  rfidWaitingMessageTime = 0;

  bool isAllowed = doc["isAllowed"].as<bool>();
  if (isAllowed) {
    String username = doc["username"].as<String>();
    setLcdMessage("Bienvenido " + username, 0, 5000);
    ledSuccess();
  }
  else {
    setLcdMessage("Acceso denegado.", 0, 5000);
    ledError();
  }
}

void setupRfid() {
  logInfo(RFID_LOG_PREFIX, "Preparing RFID Reader.");
  SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SDA_PIN); 
  rfid.PCD_Init();
  awsSuscribeOnMessage(rfidOnAwsMessageReceived);
}

String tryReadRfid() {
  String uid = "";

  if (!isWiFiConnected())
    return uid;

  if (!rfid.PICC_IsNewCardPresent()) {
    rfid.PCD_Init();
    return uid;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    ledError();
    rfid.PCD_Init();
    return uid;
  }

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  return uid;
}

void loopRfid() {
  unsigned long now = millis();
  if (rfidIsWaitingForMessage) {
    if (now - rfidWaitingMessageTime < RFID_MESSAGE_COOLDOWN)
      return;
    rfidIsWaitingForMessage = false;
    rfidWaitingMessageTime = 0;
    setLcdMessage("Error.", 0, 2500);
    ledError();
  }

  String uid = tryReadRfid();
  if (uid == "")
    return;
  
  if (uid == rfidLastUidChecked) {
    logDebug(RFID_LOG_PREFIX, "Readed card with Uid: " + uid + ", but was the last card present.");
    return;
  }
  rfidLastUidChecked = uid;
  logDebug(RFID_LOG_PREFIX, "Readed card with Uid: " + uid);
  awsSendUidCode(uid.c_str());
  rfidIsWaitingForMessage = true;
  rfidWaitingMessageTime = now;
  setLcdMessage("Comprobando permisos...", 0, RFID_MESSAGE_COOLDOWN);
}