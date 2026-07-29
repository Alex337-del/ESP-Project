#pragma once

#include <Arduino.h>
#include <MFRC522.h>

// Максимальный размер UID у MIFARE (обычно 4 или 7 байт)
constexpr byte kRfidUidMaxSize = 10;

struct RfidUid {
  byte bytes[kRfidUidMaxSize] = {};
  byte size = 0;

  String toHex(char separator = ' ') const;
  bool equals(const byte* other, byte otherSize) const;
};

struct RfidPins {
  uint8_t ss;
  uint8_t rst;
  uint8_t sck;
  uint8_t miso;
  uint8_t mosi;
};

class RfidRc522 {
 public:
  explicit RfidRc522(const RfidPins& pins);

  // Инициализация SPI и модуля. true — чип отвечает на шине.
  bool begin();

  // Неблокирующий опрос новой метки (с debounce). true — uid заполнен.
  bool poll(RfidUid& uid);

  // Быстрое чтение для режима «держит, пока метка рядом».
  // После чтения делает Halt, чтобы метку можно было читать снова.
  bool tryRead(RfidUid& uid);

  // Версия прошивки чипа (0x91/0x92 типично; 0x00/0xFF — нет связи)
  byte firmwareVersion();

  bool isOnline();

  // Проверка UID по белому списку 4-байтных UID.
  // allowedCount = sizeof(list) / sizeof(list[0])
  bool isAllowed(const RfidUid& uid,
                 const byte allowedList[][4],
                 size_t allowedCount) const;

  const RfidPins& pins() const { return pins_; }
  MFRC522& driver() { return mfrc522_; }

 private:
  RfidPins pins_;
  MFRC522 mfrc522_;
  unsigned long lastReadMs_ = 0;
  unsigned long debounceMs_ = 800;
};
