#include "LcdPcf8574.h"

#include <Wire.h>

LcdPcf8574::LcdPcf8574(const LcdConfig& config) : config_(config) {}

void LcdPcf8574::bindBus() const {
  Wire.begin(config_.sda, config_.scl);
  Wire.setClock(100000);
  Wire.setTimeOut(50);
}

uint8_t LcdPcf8574::scanAddress() const {
  // PCF8574: 0x20..0x27, PCF8574A: 0x38..0x3F
  for (uint8_t addr = 0x20; addr <= 0x27; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      return addr;
    }
  }
  for (uint8_t addr = 0x38; addr <= 0x3F; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      return addr;
    }
  }
  return 0;
}

bool LcdPcf8574::begin() {
  bindBus();

  address_ = scanAddress();
  if (address_ == 0) {
    // На шине никто не ответил — пины/питание/адрес
    address_ = config_.i2cAddr;
    online_ = false;
    Serial.printf("LCD I2C: no device on SDA=%u SCL=%u\n", config_.sda,
                  config_.scl);
    Serial.flush();
    return false;
  }

  Serial.printf("LCD I2C: found PCF8574 at 0x%02X (SDA=%u SCL=%u)\n", address_,
                config_.sda, config_.scl);
  Serial.flush();

  if (lcd_ == nullptr) {
    lcd_ = new LiquidCrystal_I2C(address_, config_.cols, config_.rows);
  }

  // init() внутри библиотеки вызывает Wire.begin() БЕЗ пинов
  // (на ESP32-C3 это часто GPIO8/9) — LCD init уходит «в никуда».
  lcd_->init();
  bindBus();

  // Повторяем init-последовательность HD44780 уже на правильных пинах
  lcd_->begin(config_.cols, config_.rows);
  bindBus();

  lcd_->backlight();
  lcd_->clear();
  lcd_->home();

  Wire.beginTransmission(address_);
  online_ = (Wire.endTransmission() == 0);
  return online_;
}

void LcdPcf8574::clear() {
  if (lcd_ != nullptr) {
    lcd_->clear();
  }
}

void LcdPcf8574::backlight(bool on) {
  if (lcd_ == nullptr) {
    return;
  }
  if (on) {
    lcd_->backlight();
  } else {
    lcd_->noBacklight();
  }
}

void LcdPcf8574::setCursor(uint8_t col, uint8_t row) {
  if (lcd_ != nullptr) {
    lcd_->setCursor(col, row);
  }
}

void LcdPcf8574::print(const char* text) {
  if (lcd_ != nullptr && text != nullptr) {
    lcd_->print(text);
  }
}

void LcdPcf8574::print(const String& text) {
  print(text.c_str());
}

void LcdPcf8574::show(const char* line1, const char* line2) {
  if (lcd_ == nullptr || !online_) {
    return;
  }

  // Без clear(): он гасит экран и даёт мигание.
  // Перезаписываем строки на месте и дополняем пробелами.
  auto writeLine = [this](uint8_t row, const char* text) {
    lcd_->setCursor(0, row);
    uint8_t col = 0;
    if (text != nullptr) {
      while (text[col] != '\0' && col < config_.cols) {
        lcd_->write(text[col]);
        ++col;
      }
    }
    while (col < config_.cols) {
      lcd_->write(' ');
      ++col;
    }
  };

  writeLine(0, line1);
  writeLine(1, line2);
}
