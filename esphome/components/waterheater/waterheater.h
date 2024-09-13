#pragma once

#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "esphome/core/log.h"
#include "waterheater_mode.h"
#include "waterheater_traits.h"

namespace esphome {
namespace waterheater {

#define LOG_CLIMATE(prefix, type, obj) \
  if ((obj) != nullptr) { \
    ESP_LOGCONFIG(TAG, "%s%s '%s'", prefix, LOG_STR_LITERAL(type), (obj)->get_name().c_str()); \
  }

class Waterheater;

/** This class is used to encode all control actions on a waterheater device.
 *
 * It is supposed to be used by all code that wishes to control a waterheater device (mqtt, api, lambda etc).
 * Create an instance of this class by calling `id(waterheater_device).make_call();`. Then set all attributes
 * with the `set_x` methods. Finally, to apply the changes call `.perform();`.
 *
 * The integration that implements the waterheater device receives this instance with the `control` method.
 * It should check all the properties it implements and apply them as needed. It should do so by
 * getting all properties it controls with the getter methods in this class. If the optional value is
 * set (check with `.has_value()`) that means the user wants to control this property. Get the value
 * of the optional with the star operator (`*call.get_mode()`) and apply it.
 */
class WaterheaterCall {
 public:
  explicit WaterheaterCall(Waterheater *parent) : parent_(parent) {}

  /// Set the mode of the waterheater device.
  WaterheaterCall &set_mode(WaterheaterMode mode);
  /// Set the mode of the waterheater device.
  WaterheaterCall &set_mode(optional<WaterheaterMode> mode);
  /// Set the mode of the waterheater device based on a string.
  WaterheaterCall &set_mode(const std::string &mode);
  /// Set the target temperature of the waterheater device.
  WaterheaterCall &set_target_temperature(float target_temperature);
  /// Set the target temperature of the waterheater device.
  WaterheaterCall &set_target_temperature(optional<float> target_temperature);
  /** Set the low point target temperature of the waterheater device
   *
   * For waterheater devices with two point target temperature control
   */
  WaterheaterCall &set_target_temperature_low(float target_temperature_low);
  /** Set the low point target temperature of the waterheater device
   *
   * For waterheater devices with two point target temperature control
   */
  WaterheaterCall &set_target_temperature_low(optional<float> target_temperature_low);
  /** Set the high point target temperature of the waterheater device
   *
   * For waterheater devices with two point target temperature control
   */
  WaterheaterCall &set_target_temperature_high(float target_temperature_high);
  /** Set the high point target temperature of the waterheater device
   *
   * For waterheater devices with two point target temperature control
   */
  WaterheaterCall &set_target_temperature_high(optional<float> target_temperature_high);
  /// Set the target humidity of the waterheater device.
  WaterheaterCall &set_target_humidity(float target_humidity);
  /// Set the target humidity of the waterheater device.
  WaterheaterCall &set_target_humidity(optional<float> target_humidity);
  /// Set the fan mode of the waterheater device.
  WaterheaterCall &set_fan_mode(WaterheaterFanMode fan_mode);
  /// Set the fan mode of the waterheater device.
  WaterheaterCall &set_fan_mode(optional<WaterheaterFanMode> fan_mode);
  /// Set the fan mode of the waterheater device based on a string.
  WaterheaterCall &set_fan_mode(const std::string &fan_mode);
  /// Set the fan mode of the waterheater device based on a string.
  WaterheaterCall &set_fan_mode(optional<std::string> fan_mode);
  /// Set the swing mode of the waterheater device.
  WaterheaterCall &set_swing_mode(WaterheaterSwingMode swing_mode);
  /// Set the swing mode of the waterheater device.
  WaterheaterCall &set_swing_mode(optional<WaterheaterSwingMode> swing_mode);
  /// Set the swing mode of the waterheater device based on a string.
  WaterheaterCall &set_swing_mode(const std::string &swing_mode);
  /// Set the preset of the waterheater device.
  WaterheaterCall &set_preset(WaterheaterPreset preset);
  /// Set the preset of the waterheater device.
  WaterheaterCall &set_preset(optional<WaterheaterPreset> preset);
  /// Set the preset of the waterheater device based on a string.
  WaterheaterCall &set_preset(const std::string &preset);
  /// Set the preset of the waterheater device based on a string.
  WaterheaterCall &set_preset(optional<std::string> preset);

  void perform();

  const optional<WaterheaterMode> &get_mode() const;
  const optional<float> &get_target_temperature() const;
  const optional<float> &get_target_temperature_low() const;
  const optional<float> &get_target_temperature_high() const;
  const optional<float> &get_target_humidity() const;
  const optional<WaterheaterFanMode> &get_fan_mode() const;
  const optional<WaterheaterSwingMode> &get_swing_mode() const;
  const optional<std::string> &get_custom_fan_mode() const;
  const optional<WaterheaterPreset> &get_preset() const;
  const optional<std::string> &get_custom_preset() const;

 protected:
  void validate_();

  Waterheater *const parent_;
  optional<WaterheaterMode> mode_;
  optional<float> target_temperature_;
  optional<float> target_temperature_low_;
  optional<float> target_temperature_high_;
  optional<float> target_humidity_;
  optional<WaterheaterFanMode> fan_mode_;
  optional<WaterheaterSwingMode> swing_mode_;
  optional<std::string> custom_fan_mode_;
  optional<WaterheaterPreset> preset_;
  optional<std::string> custom_preset_;
};

/// Struct used to save the state of the waterheater device in restore memory.
/// Make sure to update RESTORE_STATE_VERSION when changing the struct entries.
struct WaterheaterDeviceRestoreState {
  WaterheaterMode mode;
  bool uses_custom_fan_mode{false};
  union {
    WaterheaterFanMode fan_mode;
    uint8_t custom_fan_mode;
  };
  bool uses_custom_preset{false};
  union {
    WaterheaterPreset preset;
    uint8_t custom_preset;
  };
  WaterheaterSwingMode swing_mode;
  union {
    float target_temperature;
    struct {
      float target_temperature_low;
      float target_temperature_high;
    };
  } __attribute__((packed));
  float target_humidity;

  /// Convert this struct to a waterheater call that can be performed.
  WaterheaterCall to_call(Waterheater *waterheater);
  /// Apply these settings to the waterheater device.
  void apply(Waterheater *waterheater);
} __attribute__((packed));

/**
 * WaterheaterDevice - This is the base class for all waterheater integrations. Each integration
 * needs to extend this class and implement two functions:
 *
 *  - get_traits() - return the static traits of the waterheater device
 *  - control(WaterheaterDeviceCall call) - Apply the given changes from call.
 *
 * To write data to the frontend, the integration must first set the properties using
 * this->property = value; (for example this->current_temperature = 42.0;); then the integration
 * must call this->publish_state(); to send the entire state to the frontend.
 *
 * The entire state of the waterheater device is encoded in public properties of the base class (current_temperature,
 * mode etc). These are read-only for the user and rw for integrations. The reason these are public
 * is for simple access to them from lambdas `if (id(my_waterheater).mode == waterheater::CLIMATE_MODE_HEAT_COOL) ...`
 */
class Waterheater : public EntityBase {
 public:
  Waterheater() {}

  /// The active mode of the waterheater device.
  WaterheaterMode mode{CLIMATE_MODE_OFF};

  /// The active state of the waterheater device.
  WaterheaterAction action{CLIMATE_ACTION_OFF};

  /// The current temperature of the waterheater device, as reported from the integration.
  float current_temperature{NAN};

  /// The current humidity of the waterheater device, as reported from the integration.
  float current_humidity{NAN};

  union {
    /// The target temperature of the waterheater device.
    float target_temperature;
    struct {
      /// The minimum target temperature of the waterheater device, for waterheater devices with split target temperature.
      float target_temperature_low{NAN};
      /// The maximum target temperature of the waterheater device, for waterheater devices with split target temperature.
      float target_temperature_high{NAN};
    };
  };

  /// The target humidity of the waterheater device.
  float target_humidity;

  /// The active fan mode of the waterheater device.
  optional<WaterheaterFanMode> fan_mode;

  /// The active swing mode of the waterheater device.
  WaterheaterSwingMode swing_mode;

  /// The active custom fan mode of the waterheater device.
  optional<std::string> custom_fan_mode;

  /// The active preset of the waterheater device.
  optional<WaterheaterPreset> preset;

  /// The active custom preset mode of the waterheater device.
  optional<std::string> custom_preset;

  /** Add a callback for the waterheater device state, each time the state of the waterheater device is updated
   * (using publish_state), this callback will be called.
   *
   * @param callback The callback to call.
   */
  void add_on_state_callback(std::function<void(Waterheater &)> &&callback);

  /**
   * Add a callback for the waterheater device configuration; each time the configuration parameters of a waterheater device
   * is updated (using perform() of a WaterheaterCall), this callback will be called, before any on_state callback.
   *
   * @param callback The callback to call.
   */
  void add_on_control_callback(std::function<void(WaterheaterCall &)> &&callback);

  /** Make a waterheater device control call, this is used to control the waterheater device, see the WaterheaterCall description
   * for more info.
   * @return A new WaterheaterCall instance targeting this waterheater device.
   */
  WaterheaterCall make_call();

  /** Publish the state of the waterheater device, to be called from integrations.
   *
   * This will schedule the waterheater device to publish its state to all listeners and save the current state
   * to recover memory.
   */
  void publish_state();

  /** Get the traits of this waterheater device with all overrides applied.
   *
   * Traits are static data that encode the capabilities and static data for a waterheater device such as supported
   * modes, temperature range etc.
   */
  WaterheaterTraits get_traits();

  void set_visual_min_temperature_override(float visual_min_temperature_override);
  void set_visual_max_temperature_override(float visual_max_temperature_override);
  void set_visual_temperature_step_override(float target, float current);
  void set_visual_min_humidity_override(float visual_min_humidity_override);
  void set_visual_max_humidity_override(float visual_max_humidity_override);

 protected:
  friend WaterheaterCall;

  /// Set fan mode. Reset custom fan mode. Return true if fan mode has been changed.
  bool set_fan_mode_(WaterheaterFanMode mode);

  /// Set custom fan mode. Reset primary fan mode. Return true if fan mode has been changed.
  bool set_custom_fan_mode_(const std::string &mode);

  /// Set preset. Reset custom preset. Return true if preset has been changed.
  bool set_preset_(WaterheaterPreset preset);

  /// Set custom preset. Reset primary preset. Return true if preset has been changed.
  bool set_custom_preset_(const std::string &preset);

  /** Get the default traits of this waterheater device.
   *
   * Traits are static data that encode the capabilities and static data for a waterheater device such as supported
   * modes, temperature range etc. Each integration must implement this method and the return value must
   * be constant during all of execution time.
   */
  virtual WaterheaterTraits traits() = 0;

  /** Control the waterheater device, this is a virtual method that each waterheater integration must implement.
   *
   * See more info in WaterheaterCall. The integration should check all of its values in this method and
   * set them accordingly. At the end of the call, the integration must call `publish_state()` to
   * notify the frontend of a changed state.
   *
   * @param call The WaterheaterCall instance encoding all attribute changes.
   */
  virtual void control(const WaterheaterCall &call) = 0;
  /// Restore the state of the waterheater device, call this from your setup() method.
  optional<WaterheaterDeviceRestoreState> restore_state_();
  /** Internal method to save the state of the waterheater device to recover memory. This is automatically
   * called from publish_state()
   */
  void save_state_();

  void dump_traits_(const char *tag);

  CallbackManager<void(Waterheater &)> state_callback_{};
  CallbackManager<void(WaterheaterCall &)> control_callback_{};
  ESPPreferenceObject rtc_;
  optional<float> visual_min_temperature_override_{};
  optional<float> visual_max_temperature_override_{};
  optional<float> visual_target_temperature_step_override_{};
  optional<float> visual_current_temperature_step_override_{};
  optional<float> visual_min_humidity_override_{};
  optional<float> visual_max_humidity_override_{};
};

}  // namespace waterheater
}  // namespace esphome
