#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#define AWS_IOT_LOG_PREFIX "AWS-IoT"
#define AWS_IOT_LOOP_TIME 5000
#define AWS_IOT_THINGNAME "ESP_EquipA_1"
#define AWS_IOT_SUBSCRIBE_TOPIC "institut/resposta/ESP_EquipA_1"
#define AWS_IOT_PUBLISH_TOPIC "institut/registre/ESP_EquipA_1"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

unsigned long awsIotLoopTime = 0;
typedef void (*AWSCallback)(JsonDocument&);
void awsSuscribeOnMessage(AWSCallback func);

AWSCallback awsFuncOnMessage = nullptr;

void setupAWS() {
  logInfo(AWS_IOT_LOG_PREFIX, "Preparing AWS...");
  net.setCACert(ESP32_AWS_CERT_CA);
  net.setCertificate(ESP32_AWS_CERT_CRT);
  net.setPrivateKey(ESP32_AWS_CERT_PRIVATE);

  client.begin(ESP32_AWS_IOT_ENDPOINT, 8883, net);
  client.onMessage(awsOnMessageReceived);
}

void keepAlive() {
  unsigned long now = millis();
  if (now - awsIotLoopTime < AWS_IOT_LOOP_TIME)
    return;

  if (!isConnectedToAws()) {
    logError("AWS-IoT", "Tried to keep alive a disconnected client.");
    return;
  }

  awsIotLoopTime = now;
  client.loop();
}

void awsSendUidCode(const char* uid) {
  if (!isConnectedToAws()) {
    logError(AWS_IOT_LOG_PREFIX, "Tried to send Uid Code but the client was not connected.");
    return;
  }

  JsonDocument doc;
  doc["thing_name"] = AWS_IOT_THINGNAME;
  doc["device_id"] = getEfuseMac();
  doc["rfid_id"] = uid;

  String output;
  serializeJson(doc, output);

  client.publish(AWS_IOT_PUBLISH_TOPIC, output);
  logInfo(AWS_IOT_LOG_PREFIX, "Sended Uid Code message.");
}

void awsOnMessageReceived(String &topic, String &payload) {
  logDebug(AWS_IOT_LOG_PREFIX, "Message received: " + payload);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    logError(AWS_IOT_LOG_PREFIX, "An error has ocurred while deserializing the Json.");
    return;
  }

  if (awsFuncOnMessage != nullptr) {
    awsFuncOnMessage(doc);
  }
}

void connectToAWS() {
  logInfo(AWS_IOT_LOG_PREFIX, "Trying to connect to Aws-IoT");

  if (client.connect(AWS_IOT_THINGNAME)) {
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    logInfo(AWS_IOT_LOG_PREFIX, "Successfully connected to AWS-IoT and subscribed to the topic.");
  } else {
    logError(AWS_IOT_LOG_PREFIX, "Error while trying to connect to AWS-IoT.");
  }
}

bool isConnectedToAws() {
  return client.connected();
}

void awsSuscribeOnMessage(AWSCallback func) {
  awsFuncOnMessage = func;
}

void loopAWS() {
  if (!isWiFiConnected())
    return;

  if (client.connected()) {
    keepAlive();
    return;
  }

  connectToAWS();
}