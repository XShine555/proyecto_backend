#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_LOG_PREFIX "LCD"

unsigned long lcdFirstRowMessageTime = 0;
unsigned long lcdSecondRowMessageTime = 0;
unsigned short lcdFirstMessageNextIndex = 0;
unsigned short lcdSecondMessageNextIndex = 0;
String lcdFirstMessage = "";
String lcdFirstMessageAnim = "";
String lcdSecondMessage = "";
String lcdSecondMessageAnim = "";

#define LCD_ANIM_DISPLAY_TIME 300
unsigned long lcdFirstRowLcdAnimTime = 0;
unsigned long lcdSecondRowLcdAnimTime = 0;

#define LCD_SDA_PIN 21
#define LCD_SCL_PIN 22
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

void setupLcd() {
  logInfo(LCD_LOG_PREFIX, "Preparing Lcd...");
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
}

void setLcdMessage(String message, int row, int time) {
  clearLcdMessage(row);
  if (row == 0) {
    lcdFirstMessage = message;
    lcdFirstRowMessageTime = millis() + time;
  } else {
    lcdSecondMessage = message;
    lcdSecondRowMessageTime = millis() + time;
  }
  lcd.setCursor(0, row);
  lcd.print(message);
}

void clearLcdMessage(int row) {
  if (row == 0) {
    lcdFirstMessage = "";
    lcdFirstRowLcdAnimTime = 0;
    lcdFirstRowMessageTime = 0;
    lcdFirstMessageNextIndex = 0;
  } else {
    lcdSecondMessage = "";
    lcdSecondRowLcdAnimTime = 0;
    lcdSecondRowMessageTime = 0;
    lcdSecondMessageNextIndex = 0;
  }
  lcd.setCursor(0, row);
  for (int col = 0; col < LCD_COLS; col++) {
    lcd.print(" ");
  }
}

void loopLcd() {
  unsigned long now = millis();
  if (lcdFirstMessage.length() > 0) {
    if (now > lcdFirstRowMessageTime) {
      clearLcdMessage(0);
    } else {
      if (lcdFirstMessage.length() > LCD_COLS && now > lcdFirstRowLcdAnimTime) {
        lcdFirstMessageAnim = moveToNextChar(lcdFirstMessage, lcdFirstMessageNextIndex);
        lcd.setCursor(0, 0);
        lcd.print(lcdFirstMessageAnim);
        lcdFirstRowLcdAnimTime = now + LCD_ANIM_DISPLAY_TIME;
      } else if (lcdFirstMessage.length() <= LCD_COLS) {
        lcd.setCursor(0, 0);
        lcd.print(lcdFirstMessage);
      }
    }
  }

  if (lcdSecondMessage.length() > 0) {
    if (now > lcdSecondRowMessageTime) {
      clearLcdMessage(1);
    } else {
      if (lcdSecondMessage.length() > LCD_COLS && now > lcdSecondRowLcdAnimTime) {
        lcdSecondMessageAnim = moveToNextChar(lcdSecondMessage, lcdSecondMessageNextIndex);
        lcd.setCursor(0, 1);
        lcd.print(lcdSecondMessageAnim);
        lcdSecondRowLcdAnimTime = now + LCD_ANIM_DISPLAY_TIME;
      } else if (lcdSecondMessage.length() <= LCD_COLS) {
        lcd.setCursor(0, 1);
        lcd.print(lcdSecondMessage);
      }
    }
  }
}

String moveToNextChar(String originalMessage, unsigned short &nextIndex) {
  String messageWithSpace = originalMessage + " "; 
  unsigned short len = messageWithSpace.length();

  String window = "";

  if (len <= LCD_COLS) {
    window = originalMessage;
    nextIndex = 0;
    return window;
  }

  if (nextIndex + LCD_COLS <= len) {
    window = messageWithSpace.substring(nextIndex, nextIndex + LCD_COLS);
  } else {
    window = messageWithSpace.substring(nextIndex);
    window += messageWithSpace.substring(0, LCD_COLS - window.length());
  }

  nextIndex++;
  if (nextIndex >= len) {
    nextIndex = 0;
  }

  return window;
}