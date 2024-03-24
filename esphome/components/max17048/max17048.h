#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
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

// https://github.com/hideakitai/MAX17048/blob/master/MAX17048.h
enum class REG {
    VCELL = 0x02,  // voltage
    SOC = 0x04,    // percentage
    MODE = 0x06,
    VERSION = 0x08,
    HIBRT = 0x0A,
    CONFIG = 0x0C,
    VALRT = 0x14,
    CRATE = 0x16,
    VRESET_ID = 0x18,
    STATUS = 0x1A,
    TABLE = 0x40,
    CMD = 0xFE
};
enum class ALERT
{
    RI = (1 << 0),  // Reset indicator
    VH = (1 << 1),  // Voltage high alert
    VL = (1 << 2),  // Voltage low alert
    VR = (1 << 3),  // Voltage reset alert
    HD = (1 << 4),  // SOC low alert
    SC = (1 << 5)   // SOC change alert
};

//class MAX17048Component : public PollingComponent, public i2c::I2CDevice {
/// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
class MAX17048Component : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
  MAX17048Sensor(MAX17048Component *parent) : parent_(parent) {}
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void update() override;
  void set_voltage_sensor(sensor::Sensor *vs) { voltage_sensor_ = vs; }
  
 protected:
  sensor::Sensor *voltage_sensor_{nullptr};
  MAX17048Component *parent_;
  //ADS1115Multiplexer multiplexer_;
  //ADS1115Gain gain_;
  //ADS1115Resolution resolution_;
};

}  // namespace max17048
}  // namespace esphome
