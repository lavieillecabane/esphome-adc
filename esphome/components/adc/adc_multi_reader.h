#ifndef ADC_MULTI_READER_H
#define ADC_MULTI_READER_H

#include "esphome.h"
#include <vector>

namespace esphome {
namespace adc_multi_reader {

class ADCMultiReader : public Component {
 public:
  ADCMultiReader(const std::vector<int> &pins);
  void setup() override;
  void loop() override;

 private:
  std::vector<int> pins_;
  std::vector<float> readings_;
  void read_adc_values();
};

}  // namespace adc_multi_reader
}  // namespace esphome

#endif  // ADC_MULTI_READER_H
