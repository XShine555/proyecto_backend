#include <pgmspace.h>

const char ESP32_WIFI_SSID[] = "";
const char ESP32_WIFI_PASSWORD[] = "";
const char ESP32_AWS_IOT_ENDPOINT[] = "";

// AmazonRootCA1.pem
const char ESP32_AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Certificate.pem.crt
const char ESP32_AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// Private.pem.key
const char ESP32_AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";