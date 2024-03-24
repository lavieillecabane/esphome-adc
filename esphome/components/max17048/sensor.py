import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_ID,
)
from . import max17048_ns, MAX17048Component

DEPENDENCIES = ["max17048"]

MAX17048Gain = max17048_ns.enum("MAX17048Gain")
GAIN = {
    "6.144": MAX17048Gain.MAX17048_GAIN_6P144,
    "4.096": MAX17048Gain.MAX17048_GAIN_4P096,
    "2.048": MAX17048Gain.MAX17048_GAIN_2P048,
    "1.024": MAX17048Gain.MAX17048_GAIN_1P024,
    "0.512": MAX17048Gain.MAX17048_GAIN_0P512,
    "0.256": MAX17048Gain.MAX17048_GAIN_0P256,
}

def validate_gain(value):
    if isinstance(value, float):
        value = f"{value:0.03f}"
    elif not isinstance(value, str):
        raise cv.Invalid(f'invalid gain "{value}"')

    return cv.enum(GAIN)(value)

MAX17048Sensor = max17048_ns.class_(
    "MAX17048Sensor", sensor.Sensor, cg.PollingComponent, voltage_sampler.VoltageSampler
)

CONF_MAX17048_ID = "max17048_id"
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        MAX17048Sensor,
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(CONF_MAX17048_ID): cv.use_id(MAX17048Component),
            cv.Required(CONF_GAIN): validate_gain,
        }
    )
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_MAX17048_ID])
    var = cg.new_Pvariable(config[CONF_ID], paren)
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)

    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(paren.register_sensor(var))
