#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/optional.h"

namespace esphome {
namespace vcnl4020 {

using esphome::i2c::ErrorCode;

class vcnl4020Component : public PollingComponent, public i2c::I2CDevice {
 public:
  //
  // EspHome framework functions
  //
  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void dump_config() override;
  void update() override;
  void loop() override;

};

}  // namespace vcnl4020
}  // namespace esphome
