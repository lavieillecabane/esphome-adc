import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID


DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@lboue"]
MULTI_CONF = True

CONF_m5stack_bldc_driver_ID = "m5stack_bldc_driver_id"

m5stack_bldc_driver_ns = cg.esphome_ns.namespace("m5stack_bldc_driver")
M5StackBDLCDriver = m5stack_bldc_driver_ns.class_(
    "M5StackBDLCDriver",
    i2c.I2CDevice,
    cg.Component,
)

AnalogBits = m5stack_bldc_driver_ns.enum("AnalogBits")


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(M5StackBDLCDriver),
    }
).extend(i2c.i2c_device_schema(0x65))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
