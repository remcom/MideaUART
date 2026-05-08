#pragma once

#ifdef USE_REMOTE_TRANSMITTER
#include "esphome/components/remote_base/midea_protocol.h"

namespace esphome::midea {

using remote_base::RemoteTransmitterBase;
using IrData = remote_base::MideaData;

class IrFollowMeData : public IrData {
 public:
  IrFollowMeData() : IrData({MIDEA_TYPE_FOLLOW_ME, 0x82, 0x48, 0x7F, 0x1F}) {}
  IrFollowMeData(const IrData &data) : IrData(data) {}
  IrFollowMeData(uint8_t temp, bool beeper = false) : IrFollowMeData() {
    this->set_temp(temp, false);
    this->set_beeper(beeper);
  }
  IrFollowMeData(uint8_t temp, bool fahrenheit, bool beeper) : IrFollowMeData() {
    this->set_temp(temp, fahrenheit);
    this->set_beeper(beeper);
  }

  uint8_t temp() const {
    if (this->fahrenheit())
      return this->get_value_(4) + 31;
    return this->get_value_(4) - 1;
  }
  void set_temp(uint8_t val, bool fahrenheit = false) {
    this->set_fahrenheit(fahrenheit);
    if (this->fahrenheit())
      val = esphome::clamp<uint8_t>(val, MIN_TEMP_F, MAX_TEMP_F) - 31;
    else
      val = esphome::clamp<uint8_t>(val, MIN_TEMP_C, MAX_TEMP_C) + 1;
    this->set_value_(4, val);
  }

  bool beeper() const { return this->get_value_(3, 128); }
  void set_beeper(bool val) { this->set_mask_(3, val, 128); }
  bool fahrenheit() const { return this->get_value_(2, 32); }
  void set_fahrenheit(bool val) { this->set_mask_(2, val, 32); }

 protected:
  static constexpr uint8_t MIN_TEMP_C = 0;
  static constexpr uint8_t MAX_TEMP_C = 37;
  static constexpr uint8_t MIN_TEMP_F = 32;
  static constexpr uint8_t MAX_TEMP_F = 99;
};

class IrSpecialData : public IrData {
 public:
  IrSpecialData(uint8_t code) : IrData({MIDEA_TYPE_SPECIAL, code, 0xFF, 0xFF, 0xFF}) {}
};

class IrTransmitter {
 public:
  void set_transmitter(RemoteTransmitterBase *transmitter) { this->transmitter_ = transmitter; }
  void transmit(IrData &data) {
    data.finalize();
    auto transmit = this->transmitter_->transmit();
    remote_base::MideaProtocol().encode(transmit.get_data(), data);
    transmit.perform();
  }

 protected:
  RemoteTransmitterBase *transmitter_{nullptr};
};

}  // namespace esphome::midea

#endif  // USE_REMOTE_TRANSMITTER
