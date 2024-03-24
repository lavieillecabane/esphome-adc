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

enum MAX17048Gain {
  MAX17048_GAIN_6P144 = 0b000,
  MAX17048_GAIN_4P096 = 0b001,
  MAX17048_GAIN_2P048 = 0b010,
  MAX17048_GAIN_1P024 = 0b011,
  MAX17048_GAIN_0P512 = 0b100,
  MAX17048_GAIN_0P256 = 0b101,
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

class MAX17048Sensor;

class MAX17048Component : public Component, public i2c::I2CDevice {
 public:
  void register_sensor(MAX17048Sensor *obj) { this->sensors_.push_back(obj); }
  /// Set up the internal sensor array.
  void setup() override;
  void dump_config() override;
  /// HARDWARE_LATE setup priority
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_continuous_mode(bool continuous_mode) { continuous_mode_ = continuous_mode; }

  /// Helper method to request a measurement from a sensor.
  float request_measurement(MAX17048Sensor *sensor);

 protected:
  std::vector<MAX17048Sensor *> sensors_;
  uint16_t prev_config_{0};
  bool continuous_mode_;
};

/// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
class MAX17048Sensor : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
  MAX17048Sensor(MAX17048Component *parent) : parent_(parent) {}
  void update() override;
  //void set_multiplexer(ADS1115Multiplexer multiplexer) { multiplexer_ = multiplexer; }
  void set_gain(MAX17048Gain gain) { gain_ = gain; }
  //void set_resolution(ADS1115Resolution resolution) { resolution_ = resolution; }
  float sample() override;
  //uint8_t get_multiplexer() const { return multiplexer_; }
  uint8_t get_gain() const { return gain_; }
  //uint8_t get_resolution() const { return resolution_; }

 protected:
  MAX17048Component *parent_;
  MAX17048Gain gain_;
  
};

}  // namespace max17048
}  // namespace esphome
