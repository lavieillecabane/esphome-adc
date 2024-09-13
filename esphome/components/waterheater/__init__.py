from esphome import automation
import esphome.codegen as cg
from esphome.components import mqtt, web_server
import esphome.config_validation as cv
from esphome.const import (
    CONF_ACTION_STATE_TOPIC,
    CONF_AWAY,
    CONF_AWAY_COMMAND_TOPIC,
    CONF_AWAY_STATE_TOPIC,
    CONF_CURRENT_HUMIDITY_STATE_TOPIC,
    CONF_CURRENT_TEMPERATURE_STATE_TOPIC,
    CONF_CUSTOM_FAN_MODE,
    CONF_CUSTOM_PRESET,
    CONF_FAN_MODE,
    CONF_FAN_MODE_COMMAND_TOPIC,
    CONF_FAN_MODE_STATE_TOPIC,
    CONF_ID,
    CONF_MAX_TEMPERATURE,
    CONF_MIN_TEMPERATURE,
    CONF_MODE,
    CONF_MODE_COMMAND_TOPIC,
    CONF_MODE_STATE_TOPIC,
    CONF_MQTT_ID,
    CONF_ON_CONTROL,
    CONF_ON_STATE,
    CONF_PRESET,
    CONF_PRESET_COMMAND_TOPIC,
    CONF_PRESET_STATE_TOPIC,
    CONF_SWING_MODE,
    CONF_SWING_MODE_COMMAND_TOPIC,
    CONF_SWING_MODE_STATE_TOPIC,
    CONF_TARGET_HUMIDITY_COMMAND_TOPIC,
    CONF_TARGET_HUMIDITY_STATE_TOPIC,
    CONF_TARGET_TEMPERATURE,
    CONF_TARGET_TEMPERATURE_COMMAND_TOPIC,
    CONF_TARGET_TEMPERATURE_HIGH,
    CONF_TARGET_TEMPERATURE_HIGH_COMMAND_TOPIC,
    CONF_TARGET_TEMPERATURE_HIGH_STATE_TOPIC,
    CONF_TARGET_TEMPERATURE_LOW,
    CONF_TARGET_TEMPERATURE_LOW_COMMAND_TOPIC,
    CONF_TARGET_TEMPERATURE_LOW_STATE_TOPIC,
    CONF_TARGET_TEMPERATURE_STATE_TOPIC,
    CONF_TEMPERATURE_STEP,
    CONF_TRIGGER_ID,
    CONF_VISUAL,
    CONF_WEB_SERVER_ID,
)
from esphome.core import CORE, coroutine_with_priority
from esphome.cpp_helpers import setup_entity

IS_PLATFORM_COMPONENT = True

CODEOWNERS = ["@esphome/core"]
waterheater_ns = cg.esphome_ns.namespace("waterheater")

Waterheater = waterheater_ns.class_("Waterheater", cg.EntityBase)
WaterheaterCall = waterheater_ns.class_("WaterheaterCall")
WaterheaterTraits = waterheater_ns.class_("WaterheaterTraits")

WaterheaterMode = waterheater_ns.enum("WaterheaterMode")
WATERHEATER_MODES = {
    "OFF": WaterheaterMode.WATERHEATER_MODE_OFF,
    "HEAT_COOL": WaterheaterMode.WATERHEATER_MODE_HEAT_COOL,
    "COOL": WaterheaterMode.WATERHEATER_MODE_COOL,
    "HEAT": WaterheaterMode.WATERHEATER_MODE_HEAT,
    "DRY": WaterheaterMode.WATERHEATER_MODE_DRY,
    "FAN_ONLY": WaterheaterMode.WATERHEATER_MODE_FAN_ONLY,
    "AUTO": WaterheaterMode.WATERHEATER_MODE_AUTO,
}
validate_waterheater_mode = cv.enum(WATERHEATER_MODES, upper=True)

WaterheaterFanMode = waterheater_ns.enum("WaterheaterFanMode")
WATERHEATER_FAN_MODES = {
    "ON": WaterheaterFanMode.WATERHEATER_FAN_ON,
    "OFF": WaterheaterFanMode.WATERHEATER_FAN_OFF,
    "AUTO": WaterheaterFanMode.WATERHEATER_FAN_AUTO,
    "LOW": WaterheaterFanMode.WATERHEATER_FAN_LOW,
    "MEDIUM": WaterheaterFanMode.WATERHEATER_FAN_MEDIUM,
    "HIGH": WaterheaterFanMode.WATERHEATER_FAN_HIGH,
    "MIDDLE": WaterheaterFanMode.WATERHEATER_FAN_MIDDLE,
    "FOCUS": WaterheaterFanMode.WATERHEATER_FAN_FOCUS,
    "DIFFUSE": WaterheaterFanMode.WATERHEATER_FAN_DIFFUSE,
    "QUIET": WaterheaterFanMode.WATERHEATER_FAN_QUIET,
}

validate_waterheater_fan_mode = cv.enum(WATERHEATER_FAN_MODES, upper=True)

WaterheaterPreset = waterheater_ns.enum("WaterheaterPreset")
WATERHEATER_PRESETS = {
    "NONE": WaterheaterPreset.WATERHEATER_PRESET_NONE,
    "ECO": WaterheaterPreset.WATERHEATER_PRESET_ECO,
    "AWAY": WaterheaterPreset.WATERHEATER_PRESET_AWAY,
    "BOOST": WaterheaterPreset.WATERHEATER_PRESET_BOOST,
    "COMFORT": WaterheaterPreset.WATERHEATER_PRESET_COMFORT,
    "HOME": WaterheaterPreset.WATERHEATER_PRESET_HOME,
    "SLEEP": WaterheaterPreset.WATERHEATER_PRESET_SLEEP,
    "ACTIVITY": WaterheaterPreset.WATERHEATER_PRESET_ACTIVITY,
}

validate_waterheater_preset = cv.enum(WATERHEATER_PRESETS, upper=True)

WaterheaterSwingMode = waterheater_ns.enum("WaterheaterSwingMode")
WATERHEATER_SWING_MODES = {
    "OFF": WaterheaterSwingMode.WATERHEATER_SWING_OFF,
    "BOTH": WaterheaterSwingMode.WATERHEATER_SWING_BOTH,
    "VERTICAL": WaterheaterSwingMode.WATERHEATER_SWING_VERTICAL,
    "HORIZONTAL": WaterheaterSwingMode.WATERHEATER_SWING_HORIZONTAL,
}

validate_waterheater_swing_mode = cv.enum(WATERHEATER_SWING_MODES, upper=True)

CONF_CURRENT_TEMPERATURE = "current_temperature"
CONF_MIN_HUMIDITY = "min_humidity"
CONF_MAX_HUMIDITY = "max_humidity"
CONF_TARGET_HUMIDITY = "target_humidity"

visual_temperature = cv.float_with_unit(
    "visual_temperature", "(°C|° C|°|C|° K|° K|K|°F|° F|F)?"
)


def single_visual_temperature(value):
    if isinstance(value, dict):
        return value

    value = visual_temperature(value)
    return VISUAL_TEMPERATURE_STEP_SCHEMA(
        {
            CONF_TARGET_TEMPERATURE: value,
            CONF_CURRENT_TEMPERATURE: value,
        }
    )


# Actions
ControlAction = waterheater_ns.class_("ControlAction", automation.Action)
StateTrigger = waterheater_ns.class_(
    "StateTrigger", automation.Trigger.template(Waterheater.operator("ref"))
)
ControlTrigger = waterheater_ns.class_(
    "ControlTrigger", automation.Trigger.template(WaterheaterCall.operator("ref"))
)

VISUAL_TEMPERATURE_STEP_SCHEMA = cv.Any(
    single_visual_temperature,
    cv.Schema(
        {
            cv.Required(CONF_TARGET_TEMPERATURE): visual_temperature,
            cv.Required(CONF_CURRENT_TEMPERATURE): visual_temperature,
        }
    ),
)

WATERHEATER_SCHEMA = (
    cv.ENTITY_BASE_SCHEMA.extend(web_server.WEBSERVER_SORTING_SCHEMA)
    .extend(cv.MQTT_COMMAND_COMPONENT_SCHEMA)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(Waterheater),
            cv.OnlyWith(CONF_MQTT_ID, "mqtt"): cv.declare_id(mqtt.MQTTWaterheaterComponent),
            cv.Optional(CONF_VISUAL, default={}): cv.Schema(
                {
                    cv.Optional(CONF_MIN_TEMPERATURE): cv.temperature,
                    cv.Optional(CONF_MAX_TEMPERATURE): cv.temperature,
                    cv.Optional(CONF_TEMPERATURE_STEP): VISUAL_TEMPERATURE_STEP_SCHEMA,
                    cv.Optional(CONF_MIN_HUMIDITY): cv.percentage_int,
                    cv.Optional(CONF_MAX_HUMIDITY): cv.percentage_int,
                }
            ),
            cv.Optional(CONF_ACTION_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_AWAY_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_AWAY_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_CURRENT_TEMPERATURE_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_CURRENT_HUMIDITY_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_FAN_MODE_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_FAN_MODE_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_MODE_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_MODE_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_PRESET_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_PRESET_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_SWING_MODE_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_SWING_MODE_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_HIGH_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_HIGH_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_LOW_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_TEMPERATURE_LOW_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_HUMIDITY_COMMAND_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_TARGET_HUMIDITY_STATE_TOPIC): cv.All(
                cv.requires_component("mqtt"), cv.publish_topic
            ),
            cv.Optional(CONF_ON_CONTROL): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ControlTrigger),
                }
            ),
            cv.Optional(CONF_ON_STATE): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(StateTrigger),
                }
            ),
        }
    )
)


async def setup_waterheater_core_(var, config):
    await setup_entity(var, config)

    visual = config[CONF_VISUAL]
    if (min_temp := visual.get(CONF_MIN_TEMPERATURE)) is not None:
        cg.add(var.set_visual_min_temperature_override(min_temp))
    if (max_temp := visual.get(CONF_MAX_TEMPERATURE)) is not None:
        cg.add(var.set_visual_max_temperature_override(max_temp))
    if (temp_step := visual.get(CONF_TEMPERATURE_STEP)) is not None:
        cg.add(
            var.set_visual_temperature_step_override(
                temp_step[CONF_TARGET_TEMPERATURE],
                temp_step[CONF_CURRENT_TEMPERATURE],
            )
        )
    if (min_humidity := visual.get(CONF_MIN_HUMIDITY)) is not None:
        cg.add(var.set_visual_min_humidity_override(min_humidity))
    if (max_humidity := visual.get(CONF_MAX_HUMIDITY)) is not None:
        cg.add(var.set_visual_max_humidity_override(max_humidity))

    if (mqtt_id := config.get(CONF_MQTT_ID)) is not None:
        mqtt_ = cg.new_Pvariable(mqtt_id, var)
        await mqtt.register_mqtt_component(mqtt_, config)

        if (action_state_topic := config.get(CONF_ACTION_STATE_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_action_state_topic(action_state_topic))
        if (away_command_topic := config.get(CONF_AWAY_COMMAND_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_away_command_topic(away_command_topic))
        if (away_state_topic := config.get(CONF_AWAY_STATE_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_away_state_topic(away_state_topic))
        if (
            current_temperature_state_topic := config.get(
                CONF_CURRENT_TEMPERATURE_STATE_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_current_temperature_state_topic(
                    current_temperature_state_topic
                )
            )
        if (
            current_humidity_state_topic := config.get(
                CONF_CURRENT_HUMIDITY_STATE_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_current_humidity_state_topic(
                    current_humidity_state_topic
                )
            )
        if (
            fan_mode_command_topic := config.get(CONF_FAN_MODE_COMMAND_TOPIC)
        ) is not None:
            cg.add(mqtt_.set_custom_fan_mode_command_topic(fan_mode_command_topic))
        if (fan_mode_state_topic := config.get(CONF_FAN_MODE_STATE_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_fan_mode_state_topic(fan_mode_state_topic))
        if (mode_command_topic := config.get(CONF_MODE_COMMAND_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_mode_command_topic(mode_command_topic))
        if (mode_state_topic := config.get(CONF_MODE_STATE_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_mode_state_topic(mode_state_topic))
        if (preset_command_topic := config.get(CONF_PRESET_COMMAND_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_preset_command_topic(preset_command_topic))
        if (preset_state_topic := config.get(CONF_PRESET_STATE_TOPIC)) is not None:
            cg.add(mqtt_.set_custom_preset_state_topic(preset_state_topic))
        if (
            swing_mode_command_topic := config.get(CONF_SWING_MODE_COMMAND_TOPIC)
        ) is not None:
            cg.add(mqtt_.set_custom_swing_mode_command_topic(swing_mode_command_topic))
        if (
            swing_mode_state_topic := config.get(CONF_SWING_MODE_STATE_TOPIC)
        ) is not None:
            cg.add(mqtt_.set_custom_swing_mode_state_topic(swing_mode_state_topic))
        if (
            target_temperature_command_topic := config.get(
                CONF_TARGET_TEMPERATURE_COMMAND_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_command_topic(
                    target_temperature_command_topic
                )
            )
        if (
            target_temperature_state_topic := config.get(
                CONF_TARGET_TEMPERATURE_STATE_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_state_topic(
                    target_temperature_state_topic
                )
            )
        if (
            target_temperature_high_command_topic := config.get(
                CONF_TARGET_TEMPERATURE_HIGH_COMMAND_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_high_command_topic(
                    target_temperature_high_command_topic
                )
            )
        if (
            target_temperature_high_state_topic := config.get(
                CONF_TARGET_TEMPERATURE_HIGH_STATE_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_high_state_topic(
                    target_temperature_high_state_topic
                )
            )
        if (
            target_temperature_low_command_topic := config.get(
                CONF_TARGET_TEMPERATURE_LOW_COMMAND_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_low_command_topic(
                    target_temperature_low_command_topic
                )
            )
        if (
            target_temperature_low_state_topic := config.get(
                CONF_TARGET_TEMPERATURE_LOW_STATE_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_temperature_state_topic(
                    target_temperature_low_state_topic
                )
            )
        if (
            target_humidity_command_topic := config.get(
                CONF_TARGET_HUMIDITY_COMMAND_TOPIC
            )
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_humidity_command_topic(
                    target_humidity_command_topic
                )
            )
        if (
            target_humidity_state_topic := config.get(CONF_TARGET_HUMIDITY_STATE_TOPIC)
        ) is not None:
            cg.add(
                mqtt_.set_custom_target_humidity_state_topic(
                    target_humidity_state_topic
                )
            )

    for conf in config.get(CONF_ON_STATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger, [(Waterheater.operator("ref"), "x")], conf
        )

    for conf in config.get(CONF_ON_CONTROL, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger, [(WaterheaterCall.operator("ref"), "x")], conf
        )

    if (webserver_id := config.get(CONF_WEB_SERVER_ID)) is not None:
        web_server_ = await cg.get_variable(webserver_id)
        web_server.add_entity_to_sorting_list(web_server_, var, config)


async def register_waterheater(var, config):
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    cg.add(cg.App.register_waterheater(var))
    await setup_waterheater_core_(var, config)


WATERHEATER_CONTROL_ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(Waterheater),
        cv.Optional(CONF_MODE): cv.templatable(validate_waterheater_mode),
        cv.Optional(CONF_TARGET_TEMPERATURE): cv.templatable(cv.temperature),
        cv.Optional(CONF_TARGET_TEMPERATURE_LOW): cv.templatable(cv.temperature),
        cv.Optional(CONF_TARGET_TEMPERATURE_HIGH): cv.templatable(cv.temperature),
        cv.Optional(CONF_TARGET_HUMIDITY): cv.templatable(cv.percentage_int),
        cv.Optional(CONF_AWAY): cv.invalid("Use preset instead"),
        cv.Exclusive(CONF_FAN_MODE, "fan_mode"): cv.templatable(
            validate_waterheater_fan_mode
        ),
        cv.Exclusive(CONF_CUSTOM_FAN_MODE, "fan_mode"): cv.templatable(
            cv.string_strict
        ),
        cv.Exclusive(CONF_PRESET, "preset"): cv.templatable(validate_waterheater_preset),
        cv.Exclusive(CONF_CUSTOM_PRESET, "preset"): cv.templatable(cv.string_strict),
        cv.Optional(CONF_SWING_MODE): cv.templatable(validate_waterheater_swing_mode),
    }
)


@automation.register_action(
    "waterheater.control", ControlAction, WATERHEATER_CONTROL_ACTION_SCHEMA
)
async def waterheater_control_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if (mode := config.get(CONF_MODE)) is not None:
        template_ = await cg.templatable(mode, args, WaterheaterMode)
        cg.add(var.set_mode(template_))
    if (target_temp := config.get(CONF_TARGET_TEMPERATURE)) is not None:
        template_ = await cg.templatable(target_temp, args, float)
        cg.add(var.set_target_temperature(template_))
    if (target_temp_low := config.get(CONF_TARGET_TEMPERATURE_LOW)) is not None:
        template_ = await cg.templatable(target_temp_low, args, float)
        cg.add(var.set_target_temperature_low(template_))
    if (target_temp_high := config.get(CONF_TARGET_TEMPERATURE_HIGH)) is not None:
        template_ = await cg.templatable(target_temp_high, args, float)
        cg.add(var.set_target_temperature_high(template_))
    if (target_humidity := config.get(CONF_TARGET_HUMIDITY)) is not None:
        template_ = await cg.templatable(target_humidity, args, float)
        cg.add(var.set_target_humidity(template_))
    if (fan_mode := config.get(CONF_FAN_MODE)) is not None:
        template_ = await cg.templatable(fan_mode, args, WaterheaterFanMode)
        cg.add(var.set_fan_mode(template_))
    if (custom_fan_mode := config.get(CONF_CUSTOM_FAN_MODE)) is not None:
        template_ = await cg.templatable(custom_fan_mode, args, cg.std_string)
        cg.add(var.set_custom_fan_mode(template_))
    if (preset := config.get(CONF_PRESET)) is not None:
        template_ = await cg.templatable(preset, args, WaterheaterPreset)
        cg.add(var.set_preset(template_))
    if (custom_preset := config.get(CONF_CUSTOM_PRESET)) is not None:
        template_ = await cg.templatable(custom_preset, args, cg.std_string)
        cg.add(var.set_custom_preset(template_))
    if (swing_mode := config.get(CONF_SWING_MODE)) is not None:
        template_ = await cg.templatable(swing_mode, args, WaterheaterSwingMode)
        cg.add(var.set_swing_mode(template_))
    return var


@coroutine_with_priority(100.0)
async def to_code(config):
    cg.add_define("USE_WATERHEATER")
    cg.add_global(waterheater_ns.using)
