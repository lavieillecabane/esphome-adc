from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PINS
from esphome.core import CORE
from esphome.cpp_generator import Pvariable, add
from esphome.cpp_helpers import setup_component
from esphome.cpp_types import App, Component

DEPENDENCIES = ["sensor"]

CONF_ATTENUATION = "attenuation"

ADCMultiReader = sensor.sensor_ns.class_("ADCMultiReader", Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ADCMultiReader),
    cv.Required(CONF_PINS): cv.ensure_list(cv.positive_int),
    cv.Optional(CONF_ATTENUATION, default=3.3): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    rhs = App.register_component(ADCMultiReader(config[CONF_PINS], config[CONF_ATTENUATION]))
    reader = Pvariable(config[CONF_ID], rhs)
    setup_component(reader, config)
