#include "RfidRc522.h"

#include <SPI.h>

String RfidUid::toHex(char separator) const {
  String out;
  out.reserve(static_cast<unsigned int>(size) * 3);
  for (byte i = 0; i < size; ++i) {
    if (i > 0 && separator != '\0') {
      out += separator;
    }
    if (bytes[i] < 0x10) {
      out += '0';
    }
    out += String(bytes[i], HEX);
  }
  out.toUpperCase();
  return out;
}

bool RfidUid::equals(const byte* other, byte otherSize) const {
  if (other == nullptr || size != otherSize) {
    return false;
  }
  for (byte i = 0; i < size; ++i) {
    if (bytes[i] != other[i]) {
      return false;
    }
  }
  return true;
}

RfidRc522::RfidRc522(const RfidPins& pins)
    : pins_(pins), mfrc522_(pins.ss, pins.rst) {}

bool RfidRc522::begin() {
  pinMode(pins_.rst, OUTPUT);
  digitalWrite(pins_.rst, LOW);
  delay(50);
  digitalWrite(pins_.rst, HIGH);
  delay(50);

  SPI.begin(pins_.sck, pins_.miso, pins_.mosi, pins_.ss);
  // На длинных проводах/клонах стабильнее низкая частота
  SPI.setFrequency(4000000);

  mfrc522_.PCD_Init();
  delay(50);
  mfrc522_.PCD_SetAntennaGain(mfrc522_.RxGain_max);
  mfrc522_.PCD_AntennaOn();

  return isOnline();
}

byte RfidRc522::firmwareVersion() {
  return mfrc522_.PCD_ReadRegister(MFRC522::VersionReg);
}

bool RfidRc522::isOnline() {
  const byte version = firmwareVersion();
  return version != 0x00 && version != 0xFF;
}

bool RfidRc522::poll(RfidUid& uid) {
  const unsigned long now = millis();
  if ((now - lastReadMs_) < debounceMs_) {
    return false;
  }
  if (!tryRead(uid)) {
    return false;
  }
  lastReadMs_ = now;
  return true;
}

bool RfidRc522::tryRead(RfidUid& uid) {
  if (!mfrc522_.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!mfrc522_.PICC_ReadCardSerial()) {
    return false;
  }

  uid.size = mfrc522_.uid.size;
  if (uid.size > kRfidUidMaxSize) {
    uid.size = kRfidUidMaxSize;
  }
  for (byte i = 0; i < uid.size; ++i) {
    uid.bytes[i] = mfrc522_.uid.uidByte[i];
  }
  for (byte i = uid.size; i < kRfidUidMaxSize; ++i) {
    uid.bytes[i] = 0;
  }

  // Halt обязателен: иначе PICC_IsNewCardPresent() не сработает повторно,
  // пока метка лежит на антенне.
  mfrc522_.PICC_HaltA();
  mfrc522_.PCD_StopCrypto1();
  return true;
}

bool RfidRc522::isAllowed(const RfidUid& uid,
                          const byte allowedList[][4],
                          size_t allowedCount) const {
  if (allowedList == nullptr || allowedCount == 0 || uid.size != 4) {
    return false;
  }
  for (size_t i = 0; i < allowedCount; ++i) {
    if (uid.equals(allowedList[i], 4)) {
      return true;
    }
  }
  return false;
}
