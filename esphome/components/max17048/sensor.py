import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor, voltage_sampler
from esphome.const import (
    CONF_ID,
    CONF_VOLTAGE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
)

DEPENDENCIES = ["i2c"]
max17048_ns = cg.esphome_ns.namespace("max17048")

#define MAX17048_I2CADDR_DEFAULT 0x36 ///< MAX17048 default i2c address
MAX17048Component = max17048_ns.class_(
    "MAX17048Component", cg.PollingComponent, i2c.I2CDevice
)

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
            #cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
            #cv.Required(CONF_GAIN): validate_gain,
            #cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
            #    RESOLUTION, upper=True, space="_"
            #),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x36))
)
'''
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MAX17048Component),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x36))
)
'''

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_VOLTAGE in config:
        conf = config[CONF_VOLTAGE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_voltage_sensor(sens))
