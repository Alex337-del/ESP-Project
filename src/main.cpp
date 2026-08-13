#include <Arduino.h>
#include <Wire.h>
#include <GyverBME280.h>

#include "LcdPcf8574.h"
#include "config.h"

GyverBME280 bme;

LcdPcf8574 lcd({
    Config::kI2cSdaPin,
    Config::kI2cSclPin,
    Config::kLcdI2cAddr,
    Config::kLcdCols,
    Config::kLcdRows,
});

void showOnLcd(float tempC, float humidity) {
  static char lastLine1[17] = {};
  static char lastLine2[17] = {};

  char line1[17];
  char line2[17];
  snprintf(line1, sizeof(line1), "Temp: %.1f C", tempC);
  snprintf(line2, sizeof(line2), "Hum:  %.1f %%", humidity);

  // Не дергаем LCD, если текст не изменился
  if (strcmp(line1, lastLine1) == 0 && strcmp(line2, lastLine2) == 0) {
    return;
  }

  strncpy(lastLine1, line1, sizeof(lastLine1));
  strncpy(lastLine2, line2, sizeof(lastLine2));
  lastLine1[16] = '\0';
  lastLine2[16] = '\0';

  lcd.show(line1, line2);
}

void setup() {
  Serial.begin(Config::kSerialBaud);
  delay(Config::kUsbCdcSettleMs);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-C3 + GyverBME280 + LCD");
  Serial.printf("I2C SDA=GPIO%u SCL=GPIO%u\n",
                Config::kI2cSdaPin, Config::kI2cSclPin);
  Serial.flush();

  Wire.begin(Config::kI2cSdaPin, Config::kI2cSclPin);
  Wire.setClock(100000);

  if (lcd.begin()) {
    Serial.printf("LCD OK, addr 0x%02X\n", lcd.address());
    lcd.show("BME280", "Waiting...");
  } else {
    Serial.println("LCD FAIL");
  }

  if (bme.begin(Config::kBmeI2cAddr)) {
    Serial.println("BME280 OK");
  } else if (bme.begin(0x77)) {
    Serial.println("BME280 OK (addr 0x77)");
  } else {
    Serial.println("BME280 FAIL");
    lcd.show("BME280 FAIL", "Check wiring");
  }
  Serial.flush();
}

void loop() {
  const float tempC = bme.readTemperature();
  const float humidity = bme.readHumidity();
  const float pressurePa = bme.readPressure();

  showOnLcd(tempC, humidity);

  Serial.println("---------- BME280 ----------");
  Serial.printf("Temp:     %.2f C\n", tempC);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Pressure: %.2f hPa\n", pressurePa / 100.0f);
  Serial.println("----------------------------");
  Serial.flush();

  delay(Config::kBmeReadIntervalMs);
}
