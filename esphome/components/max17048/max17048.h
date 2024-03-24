#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace max17048 {

enum MAX17048Config {
  MAX17048_ADDRESS = 0x36,
  MAX17048_VCELL = 0x02,  // voltage
  MAX17048_SOC = 0x04,    // percentage
  MAX17048_MODE = 0x06,
  MAX17048_VERSION = 0x08,
  MAX17048_CONFIG = 0x0c,
  MAX17048_COMMAND = 0xfe,
};

class MAX17048Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void update() override;
  void set_voltage_sensor(sensor::Sensor *vs) { voltage_sensor_ = vs; }
  
 protected:
  sensor::Sensor *voltage_sensor_{nullptr};
};

}  // namespace max17048
}  // namespace esphome
