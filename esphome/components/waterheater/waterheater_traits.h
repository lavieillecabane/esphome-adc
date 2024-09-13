#pragma once

#include "esphome/core/helpers.h"
#include "waterheater_mode.h"
#include <set>

namespace esphome {
namespace waterheater {

/** This class contains all static data for waterheater devices.
 *
 * All waterheater devices must support these features:
 *  - OFF mode
 *  - Target Temperature
 *
 * All other properties and modes are optional and the integration must mark
 * each of them as supported by setting the appropriate flag here.
 *
 *  - supports current temperature - if the waterheater device supports reporting a current temperature
 *  - supports two point target temperature - if the waterheater device's target temperature should be
 *     split in target_temperature_low and target_temperature_high instead of just the single target_temperature
 *  - supports modes:
 *    - auto mode (automatic control)
 *    - cool mode (lowers current temperature)
 *    - heat mode (increases current temperature)
 *    - dry mode (removes humidity from air)
 *    - fan mode (only turns on fan)
 *  - supports action - if the waterheater device supports reporting the active
 *    current action of the device with the action property.
 *  - supports fan modes - optionally, if it has a fan which can be configured in different ways:
 *    - on, off, auto, high, medium, low, middle, focus, diffuse, quiet
 *  - supports swing modes - optionally, if it has a swing which can be configured in different ways:
 *    - off, both, vertical, horizontal
 *
 * This class also contains static data for the waterheater device display:
 *  - visual min/max temperature - tells the frontend what range of temperatures the waterheater device
 *     should display (gauge min/max values)
 *  - temperature step - the step with which to increase/decrease target temperature.
 *     This also affects with how many decimal places the temperature is shown
 */
class WaterheaterTraits {
 public:
  bool get_supports_current_temperature() const { return supports_current_temperature_; }
  void set_supports_current_temperature(bool supports_current_temperature) {
    supports_current_temperature_ = supports_current_temperature;
  }
  bool get_supports_current_humidity() const { return supports_current_humidity_; }
  void set_supports_current_humidity(bool supports_current_humidity) {
    supports_current_humidity_ = supports_current_humidity;
  }
  bool get_supports_two_point_target_temperature() const { return supports_two_point_target_temperature_; }
  void set_supports_two_point_target_temperature(bool supports_two_point_target_temperature) {
    supports_two_point_target_temperature_ = supports_two_point_target_temperature;
  }
  bool get_supports_target_humidity() const { return supports_target_humidity_; }
  void set_supports_target_humidity(bool supports_target_humidity) {
    supports_target_humidity_ = supports_target_humidity;
  }
  void set_supported_modes(std::set<WaterheaterMode> modes) { supported_modes_ = std::move(modes); }
  void add_supported_mode(WaterheaterMode mode) { supported_modes_.insert(mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_auto_mode(bool supports_auto_mode) { set_mode_support_(CLIMATE_MODE_AUTO, supports_auto_mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_cool_mode(bool supports_cool_mode) { set_mode_support_(CLIMATE_MODE_COOL, supports_cool_mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_heat_mode(bool supports_heat_mode) { set_mode_support_(CLIMATE_MODE_HEAT, supports_heat_mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_heat_cool_mode(bool supported) { set_mode_support_(CLIMATE_MODE_HEAT_COOL, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_fan_only_mode(bool supports_fan_only_mode) {
    set_mode_support_(CLIMATE_MODE_FAN_ONLY, supports_fan_only_mode);
  }
  ESPDEPRECATED("This method is deprecated, use set_supported_modes() instead", "v1.20")
  void set_supports_dry_mode(bool supports_dry_mode) { set_mode_support_(CLIMATE_MODE_DRY, supports_dry_mode); }
  bool supports_mode(WaterheaterMode mode) const { return supported_modes_.count(mode); }
  const std::set<WaterheaterMode> &get_supported_modes() const { return supported_modes_; }

  void set_supports_action(bool supports_action) { supports_action_ = supports_action; }
  bool get_supports_action() const { return supports_action_; }

  void set_supported_fan_modes(std::set<WaterheaterFanMode> modes) { supported_fan_modes_ = std::move(modes); }
  void add_supported_fan_mode(WaterheaterFanMode mode) { supported_fan_modes_.insert(mode); }
  void add_supported_custom_fan_mode(const std::string &mode) { supported_custom_fan_modes_.insert(mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_on(bool supported) { set_fan_mode_support_(CLIMATE_FAN_ON, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_off(bool supported) { set_fan_mode_support_(CLIMATE_FAN_OFF, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_auto(bool supported) { set_fan_mode_support_(CLIMATE_FAN_AUTO, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_low(bool supported) { set_fan_mode_support_(CLIMATE_FAN_LOW, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_medium(bool supported) { set_fan_mode_support_(CLIMATE_FAN_MEDIUM, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_high(bool supported) { set_fan_mode_support_(CLIMATE_FAN_HIGH, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_middle(bool supported) { set_fan_mode_support_(CLIMATE_FAN_MIDDLE, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_focus(bool supported) { set_fan_mode_support_(CLIMATE_FAN_FOCUS, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_fan_modes() instead", "v1.20")
  void set_supports_fan_mode_diffuse(bool supported) { set_fan_mode_support_(CLIMATE_FAN_DIFFUSE, supported); }
  bool supports_fan_mode(WaterheaterFanMode fan_mode) const { return supported_fan_modes_.count(fan_mode); }
  bool get_supports_fan_modes() const { return !supported_fan_modes_.empty() || !supported_custom_fan_modes_.empty(); }
  const std::set<WaterheaterFanMode> &get_supported_fan_modes() const { return supported_fan_modes_; }

  void set_supported_custom_fan_modes(std::set<std::string> supported_custom_fan_modes) {
    supported_custom_fan_modes_ = std::move(supported_custom_fan_modes);
  }
  const std::set<std::string> &get_supported_custom_fan_modes() const { return supported_custom_fan_modes_; }
  bool supports_custom_fan_mode(const std::string &custom_fan_mode) const {
    return supported_custom_fan_modes_.count(custom_fan_mode);
  }

  void set_supported_presets(std::set<WaterheaterPreset> presets) { supported_presets_ = std::move(presets); }
  void add_supported_preset(WaterheaterPreset preset) { supported_presets_.insert(preset); }
  void add_supported_custom_preset(const std::string &preset) { supported_custom_presets_.insert(preset); }
  bool supports_preset(WaterheaterPreset preset) const { return supported_presets_.count(preset); }
  bool get_supports_presets() const { return !supported_presets_.empty(); }
  const std::set<waterheater::WaterheaterPreset> &get_supported_presets() const { return supported_presets_; }

  void set_supported_custom_presets(std::set<std::string> supported_custom_presets) {
    supported_custom_presets_ = std::move(supported_custom_presets);
  }
  const std::set<std::string> &get_supported_custom_presets() const { return supported_custom_presets_; }
  bool supports_custom_preset(const std::string &custom_preset) const {
    return supported_custom_presets_.count(custom_preset);
  }

  void set_supported_swing_modes(std::set<WaterheaterSwingMode> modes) { supported_swing_modes_ = std::move(modes); }
  void add_supported_swing_mode(WaterheaterSwingMode mode) { supported_swing_modes_.insert(mode); }
  ESPDEPRECATED("This method is deprecated, use set_supported_swing_modes() instead", "v1.20")
  void set_supports_swing_mode_off(bool supported) { set_swing_mode_support_(CLIMATE_SWING_OFF, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_swing_modes() instead", "v1.20")
  void set_supports_swing_mode_both(bool supported) { set_swing_mode_support_(CLIMATE_SWING_BOTH, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_swing_modes() instead", "v1.20")
  void set_supports_swing_mode_vertical(bool supported) { set_swing_mode_support_(CLIMATE_SWING_VERTICAL, supported); }
  ESPDEPRECATED("This method is deprecated, use set_supported_swing_modes() instead", "v1.20")
  void set_supports_swing_mode_horizontal(bool supported) {
    set_swing_mode_support_(CLIMATE_SWING_HORIZONTAL, supported);
  }
  bool supports_swing_mode(WaterheaterSwingMode swing_mode) const { return supported_swing_modes_.count(swing_mode); }
  bool get_supports_swing_modes() const { return !supported_swing_modes_.empty(); }
  const std::set<WaterheaterSwingMode> &get_supported_swing_modes() const { return supported_swing_modes_; }

  float get_visual_min_temperature() const { return visual_min_temperature_; }
  void set_visual_min_temperature(float visual_min_temperature) { visual_min_temperature_ = visual_min_temperature; }
  float get_visual_max_temperature() const { return visual_max_temperature_; }
  void set_visual_max_temperature(float visual_max_temperature) { visual_max_temperature_ = visual_max_temperature; }
  float get_visual_target_temperature_step() const { return visual_target_temperature_step_; }
  float get_visual_current_temperature_step() const { return visual_current_temperature_step_; }
  void set_visual_target_temperature_step(float temperature_step) {
    visual_target_temperature_step_ = temperature_step;
  }
  void set_visual_current_temperature_step(float temperature_step) {
    visual_current_temperature_step_ = temperature_step;
  }
  void set_visual_temperature_step(float temperature_step) {
    visual_target_temperature_step_ = temperature_step;
    visual_current_temperature_step_ = temperature_step;
  }
  int8_t get_target_temperature_accuracy_decimals() const;
  int8_t get_current_temperature_accuracy_decimals() const;

  float get_visual_min_humidity() const { return visual_min_humidity_; }
  void set_visual_min_humidity(float visual_min_humidity) { visual_min_humidity_ = visual_min_humidity; }
  float get_visual_max_humidity() const { return visual_max_humidity_; }
  void set_visual_max_humidity(float visual_max_humidity) { visual_max_humidity_ = visual_max_humidity; }

 protected:
  void set_mode_support_(waterheater::WaterheaterMode mode, bool supported) {
    if (supported) {
      supported_modes_.insert(mode);
    } else {
      supported_modes_.erase(mode);
    }
  }
  void set_fan_mode_support_(waterheater::WaterheaterFanMode mode, bool supported) {
    if (supported) {
      supported_fan_modes_.insert(mode);
    } else {
      supported_fan_modes_.erase(mode);
    }
  }
  void set_swing_mode_support_(waterheater::WaterheaterSwingMode mode, bool supported) {
    if (supported) {
      supported_swing_modes_.insert(mode);
    } else {
      supported_swing_modes_.erase(mode);
    }
  }

  bool supports_current_temperature_{false};
  bool supports_current_humidity_{false};
  bool supports_two_point_target_temperature_{false};
  bool supports_target_humidity_{false};
  std::set<waterheater::WaterheaterMode> supported_modes_ = {waterheater::CLIMATE_MODE_OFF};
  bool supports_action_{false};
  std::set<waterheater::WaterheaterFanMode> supported_fan_modes_;
  std::set<waterheater::WaterheaterSwingMode> supported_swing_modes_;
  std::set<waterheater::WaterheaterPreset> supported_presets_;
  std::set<std::string> supported_custom_fan_modes_;
  std::set<std::string> supported_custom_presets_;

  float visual_min_temperature_{10};
  float visual_max_temperature_{30};
  float visual_target_temperature_step_{0.1};
  float visual_current_temperature_step_{0.1};
  float visual_min_humidity_{30};
  float visual_max_humidity_{99};
};

}  // namespace waterheater
}  // namespace esphome
