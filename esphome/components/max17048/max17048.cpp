#include "max17048.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#define MAX1704X_VCELL_REG 0x02   // Register that holds cell voltage
#define MAX1704X_SOC_REG 0x04     // Register that holds cell state of charge
#define MAX1704X_MODE_REG 0x06    // Register that manages mode
#define MAX1704X_VERSION_REG 0x08 // Register that has IC version
#define MAX1704X_HIBRT_REG 0x0A   // Register that manages hibernation
#define MAX1704X_CONFIG_REG 0x0C  // Register that manages configuration
#define MAX1704X_VALERT_REG 0x14  // Register that holds voltage alert values
#define MAX1704X_CRATE_REG 0x16   // Register that holds cell charge rate
#define MAX1704X_VRESET_REG 0x18  // Register that holds reset voltage setting
#define MAX1704X_CHIPID_REG 0x19  // Register that holds semi-unique chip ID
#define MAX1704X_STATUS_REG 0x1A  // Register that holds current alert/status

namespace esphome {
namespace max17048 {

static const char *const TAG = "max17048";
//static const uint8_t max17048_REGISTER_CONVERSION = 0x00;
//static const uint8_t max17048_REGISTER_CONFIG = 0x01;

void MAX17048Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up max17048...");
  
  uint16_t getICversion(void);
  uint8_t getChipID(void);
  uint16_t value;
  if (!this->read_byte_16(MAX1704X_CHIPID_REG, &value)) {
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "    CHIPID_REG: %u", value);
  //ESP_LOGCONFIG(TAG, "    CHIPID_REG: %u", value);
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

  // Gain
  //        0bxxxxBBBxxxxxxxxx
  config &= 0b1111000111111111;
  config |= (sensor->get_gain() & 0b111) << 9;

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
  
  // Get battery voltage (MAX1704X_VCELL_REG)
  uint16_t raw_conversion_vcell;
  //if (!this->read_byte_16(MAX17048_VCELL, &raw_conversion_vcell)) {
  if (!this->read_byte_16(MAX1704X_VCELL_REG, &raw_conversion_vcell)) {
    this->status_set_warning();
    return NAN;
  }
  // [D][max17048:123]:     VCELL_REG: 50720
  ESP_LOGD(TAG, "    VCELL_REG: %u", raw_conversion_vcell);
    
  auto signed_conversion_vcell = static_cast<int16_t>(raw_conversion_vcell);
  float voltage = (signed_conversion_vcell)* 78.125 / 1000000;  // Floating point value read in Volts
  //float voltage = 3.75;
  ESP_LOGD(TAG, "    float voltage: %.3f V", voltage);
  
  
  // Get battery state in percent (0-100%)
  // Floating point value from 0 to 100.0
  uint16_t raw_conversion_soc;
  if (!this->read_byte_16(MAX1704X_SOC_REG, &raw_conversion_soc)) {
    this->status_set_warning();
    return NAN;
  }
  // [D][max17048:128]:     SOC_REG: 18244
  ESP_LOGD(TAG, "    SOC_REG: %u", raw_conversion_soc);
    
  auto signed_conversion_soc = static_cast<int16_t>(raw_conversion_soc);
  float percent = (signed_conversion_soc)/ 256.0;
  
  // [D][max17048:132]:     percent: 71.265625 %
  ESP_LOGD(TAG, "    percent: %.1f %", percent);
  
  this->status_clear_warning();
  return voltage;
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
