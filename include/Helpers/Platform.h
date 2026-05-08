#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else  // non-Arduino (ESP-IDF or native)

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

using String = std::string;

// Flash-memory macros are no-ops outside AVR/Arduino
#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#define F(x) (x)

#ifdef ESP_PLATFORM
// ESP-IDF
#include "esp_timer.h"
#include "esp_random.h"
static inline unsigned long millis() {
  return (unsigned long)(esp_timer_get_time() / 1000ULL);
}
static inline long random(long max) {
  return (long)(esp_random() % (uint32_t)max);
}
#else
// Native/simulation fallbacks
#include <cstdlib>
static inline unsigned long millis() { return 0; }
static inline long random(long max) { return std::rand() % max; }
#endif  // ESP_PLATFORM

// Arduino Stream interface equivalent
class Stream {
 public:
  virtual ~Stream() = default;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual size_t write(const uint8_t *data, size_t size) = 0;
};

// Arduino IPAddress equivalent
class IPAddress {
 public:
  IPAddress() : m_addr(0) {}
  explicit IPAddress(uint32_t addr) : m_addr(addr) {}
  uint8_t operator[](int idx) const { return (m_addr >> (idx * 8)) & 0xFF; }
 private:
  uint32_t m_addr;
};

#endif  // ARDUINO
