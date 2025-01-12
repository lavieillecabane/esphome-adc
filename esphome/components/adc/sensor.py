from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.cpp_generator import Pvariable
from esphome.cpp_types import App

DEPENDENCIES = ["adc_multi_reader"]

ADCMultiReader = sensor.sensor_ns.class_("ADCMultiReader", Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ADCMultiReader),
}).extend(sensor.SENSOR_SCHEMA)

def to_code(config):
    reader = Pvariable(config[CONF_ID], App.get_component(config[CONF_ID]))
