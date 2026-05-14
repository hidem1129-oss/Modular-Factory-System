# power_calc.py

INA219_SHUNT_LSB_MV = 0.01
INA219_BUS_LSB_V = 0.004

def shunt_raw_to_current_mA(shunt_raw: int, shunt_resistor_ohm: float) -> float:
    shunt_mV = shunt_raw * INA219_SHUNT_LSB_MV
    return shunt_mV / shunt_resistor_ohm

def bus_raw_to_voltage_V(bus_raw: int) -> float:
    bus_value = bus_raw >> 3
    return bus_value * INA219_BUS_LSB_V