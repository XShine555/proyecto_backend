/* arduino/main/led_handler.ino */
#define LED_PIN 15
#define LEDC_CHANNEL 0
#define LEDC_FREQ 5000
#define LEDC_RESOLUTION 8

enum LedState {
  LED_IDLE,
  LED_SUCCESS,
  LED_ERROR_BLINK
};

static LedState ledState = LED_IDLE;
static unsigned long ledStateUntil = 0;
static int ledErrorBlinkCount = 0;
static bool ledErrorBlinkOn = false;

void setupLed() {
  pinMode(LED_PIN, OUTPUT);
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RESOLUTION);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  ledcWrite(LEDC_CHANNEL, 0);
  logInfo("LED", String("Initialized on pin ") + String(LED_PIN));
}

static void _ledSetIntensity(uint8_t value) {
  ledcWrite(LEDC_CHANNEL, value);
}

void ledSuccess() {
  ledState = LED_SUCCESS;
  ledStateUntil = millis() + 1000UL;
  _ledSetIntensity(255);
}

void ledError() {
  ledState = LED_ERROR_BLINK;
  ledErrorBlinkCount = 0;
  ledErrorBlinkOn = true;
  ledStateUntil = millis() + 100UL;
  _ledSetIntensity(255);
}

void loopLed() {
  unsigned long now = millis();
  switch (ledState) {
    case LED_IDLE:
      break;
    case LED_SUCCESS:
      if ((long)(now - ledStateUntil) >= 0) {
        _ledSetIntensity(0);
        ledState = LED_IDLE;
      }
      break;
    case LED_ERROR_BLINK:
      if ((long)(now - ledStateUntil) >= 0) {
        if (ledErrorBlinkOn) {
          _ledSetIntensity(0);
          ledErrorBlinkOn = false;
          ledStateUntil = now + 100UL;
        } else {
          ledErrorBlinkCount++;
          if (ledErrorBlinkCount >= 3) {
            ledState = LED_IDLE;
            _ledSetIntensity(0);
          } else {
            _ledSetIntensity(255);
            ledErrorBlinkOn = true;
            ledStateUntil = now + 100UL;
          }
        }
      }
      break;
  }
}
