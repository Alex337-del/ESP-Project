#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

struct LcdConfig {
  uint8_t sda;
  uint8_t scl;
  uint8_t i2cAddr;  // запасной адрес, если автопоиск не найдёт
  uint8_t cols;
  uint8_t rows;
};

class LcdPcf8574 {
 public:
  explicit LcdPcf8574(const LcdConfig& config);

  // I2C + поиск PCF8574 + init LCD. true — модуль ответил на шине.
  bool begin();

  uint8_t address() const { return address_; }
  bool isOnline() const { return online_; }

  void clear();
  void backlight(bool on = true);
  void setCursor(uint8_t col, uint8_t row);
  void print(const char* text);
  void print(const String& text);

  // Две строки сразу (для 1602/2004)
  void show(const char* line1, const char* line2 = "");

  LiquidCrystal_I2C* driver() { return lcd_; }

 private:
  void bindBus() const;
  uint8_t scanAddress() const;

  LcdConfig config_;
  LiquidCrystal_I2C* lcd_ = nullptr;
  uint8_t address_ = 0;
  bool online_ = false;
};
