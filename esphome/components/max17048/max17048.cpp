#include "max17048.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace max17048 {

static const char *const TAG = "max17048";
//static const uint8_t max17048_REGISTER_CONVERSION = 0x00;
//static const uint8_t max17048_REGISTER_CONFIG = 0x01;

void MAX17048Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up max17048...");
  uint16_t value;
  /*
  if (!this->read_byte_16(max17048_REGISTER_CONVERSION, &value)) {
    this->mark_failed();
    return;
  }
  */
  ESP_LOGCONFIG(TAG, "Configuring max17048...");

  uint16_t config = 0;

  // Setup Gain
  //        0bxxxx000xxxxxxxxx
  config |= MAX17048_GAIN_6P144 << 9;  
  
  this->prev_config_ = config;
}
void MAX17048Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up max17048...");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with max17048 failed!");
  }

  for (auto *sensor : this->sensors_) {
    LOG_SENSOR("  ", "Sensor", sensor);
    //ESP_LOGCONFIG(TAG, "    Multiplexer: %u", sensor->get_multiplexer());
    ESP_LOGCONFIG(TAG, "    Gain: %u", sensor->get_gain());
    //ESP_LOGCONFIG(TAG, "    Resolution: %u", sensor->get_resolution());
  }
}
float MAX17048Component::request_measurement(MAX17048Sensor *sensor) {
  uint16_t config = this->prev_config_;
  /*
  // Multiplexer
  //        0bxBBBxxxxxxxxxxxx
  config &= 0b1000111111111111;
  config |= (sensor->get_multiplexer() & 0b111) << 12;
  */
  // Gain
  //        0bxxxxBBBxxxxxxxxx
  config &= 0b1111000111111111;
  config |= (sensor->get_gain() & 0b111) << 9;
  /*
  if (!this->continuous_mode_) {
    // Start conversion
    config |= 0b1000000000000000;
  }
  */

  if (!this->continuous_mode_ || this->prev_config_ != config) {
    /*
    if (!this->write_byte_16(max17048_REGISTER_CONFIG, config)) {
      this->status_set_warning();
      return NAN;
    }
    */
    this->prev_config_ = config;

    // about 1.2 ms with 860 samples per second
    delay(2);

    // in continuous mode, conversion will always be running, rely on the delay
    // to ensure conversion is taking place with the correct settings
    // can we use the rdy pin to trigger when a conversion is done?
    /*
    if (!this->continuous_mode_) {
      uint32_t start = millis();
      while (this->read_byte_16(max17048_REGISTER_CONFIG, &config) && (config >> 15) == 0) {
        if (millis() - start > 100) {
          ESP_LOGW(TAG, "Reading max17048 timed out");
          this->status_set_warning();
          return NAN;
        }
        yield();
      }
    }
    */
  }

  uint16_t raw_conversion;
  auto signed_conversion = static_cast<int16_t>(raw_conversion);

  float millivolts;
  millivolts = 3300.0;

  this->status_clear_warning();
  return millivolts;
}

float MAX17048Sensor::sample() { return this->parent_->request_measurement(this); }
void MAX17048Sensor::update() {
  float v = this->parent_->request_measurement(this);
  if (!std::isnan(v)) {
    //ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    ESP_LOGD(TAG, "max17048: Got Voltage=%fV", v);
    this->publish_state(v);
  }
}

}  // namespace max17048
}  // namespace esphome
