#include "adc_multi_reader.h"

namespace esphome {
namespace adc_multi_reader {

ADCMultiReader::ADCMultiReader(const std::vector<int> &pins) : pins_(pins) {
  readings_.resize(pins.size());
}

void ADCMultiReader::setup() {
  for (int pin : pins_) {
    ESP_LOGCONFIG(TAG, "Setting up ADC pin: GPIO%d", pin);
    pinMode(pin, INPUT);
  }
}

void ADCMultiReader::loop() {
  read_adc_values();
  for (size_t i = 0; i < pins_.size(); ++i) {
    ESP_LOGD("ADC Reader", "GPIO%d: %f", pins_[i], readings_[i]);
  }
}

void ADCMultiReader::read_adc_values() {
  for (size_t i = 0; i < pins_.size(); ++i) {
    readings_[i] = analogRead(pins_[i]) * (3.3 / 4096.0);  // Convert to voltage (assuming 12-bit ADC)
  }
}

}  // namespace adc_multi_reader
}  // namespace esphome
