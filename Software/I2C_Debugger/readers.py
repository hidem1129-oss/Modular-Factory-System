from __future__ import annotations

import random
from dataclasses import dataclass
from typing import Optional, Protocol

try:
    import smbus2  # type: ignore
except Exception:
    smbus2 = None

REG_MODULE_TYPE = 0x02
REG_STATUS = 0x10
REG_CMD = 0x30
REG_SETPOINT0_H = 0x31
REG_SETPOINT0_L = 0x32
REG_TARGET_TIME_H = 0x35
REG_TARGET_TIME_L = 0x36
REG_FEEDBACK0_H = 0x20
REG_FEEDBACK0_L = 0x21

@dataclass
class RawNodeData:
    address: int
    module_type: int
    status_raw: int
    cmd_raw: int
    setpoint0: Optional[int] = None
    target_time_ms: Optional[int] = None
    feedback0: Optional[int] = None

class RegisterReader(Protocol):
    def read_node(self, address: int) -> RawNodeData:
        ...


class SMBusRegisterReader:
    def __init__(self, bus_no: int = 1) -> None:
        if smbus2 is None:
            raise RuntimeError("smbus2 is not available. Install it or use mock mode.")
        self.bus_no = bus_no
        self.bus = smbus2.SMBus(bus_no)

    def read_node(self, address: int) -> RawNodeData:
        module_type = self.bus.read_byte_data(address, REG_MODULE_TYPE)
        status_raw = self.bus.read_byte_data(address, REG_STATUS)
        cmd_raw = self.bus.read_byte_data(address, REG_CMD)

        try:
            high = self.bus.read_byte_data(address, REG_SETPOINT0_H)
            low = self.bus.read_byte_data(address, REG_SETPOINT0_L)

            setpoint0 = (high << 8) | low

        except Exception:
            setpoint0 = None

        try:
            tt_h = self.bus.read_byte_data(address, REG_TARGET_TIME_H)
            tt_l = self.bus.read_byte_data(address, REG_TARGET_TIME_L)
            target_time_ms = (tt_h << 8) | tt_l
        except Exception:
            target_time_ms = None

        try:
            fb_h = self.bus.read_byte_data(address, REG_FEEDBACK0_H)
            fb_l = self.bus.read_byte_data(address, REG_FEEDBACK0_L)
            feedback0 = (fb_h << 8) | fb_l
        except Exception:
            feedback0 = None

        return RawNodeData(
            address=address,
            module_type=module_type,
            status_raw=status_raw,
            cmd_raw=cmd_raw,
            setpoint0=setpoint0,
            target_time_ms=target_time_ms,
            feedback0=feedback0,
        )

    def _close_all_tcas(self) -> None:
        for tca_addr in (0x70, 0x71, 0x72):
            try:
                self.bus.write_byte(tca_addr, 0x00)
            except Exception:
                pass

    def read_ina219_shunt_raw(self, tca_addr: int, tca_channel: int, ina_addr: int) -> int:
        REG_INA219_SHUNT = 0x01

        self._open_tca_channel(tca_addr, tca_channel)
        raw = self.bus.read_word_data(ina_addr, REG_INA219_SHUNT)
        raw = ((raw & 0xFF) << 8) | (raw >> 8)

        if raw & 0x8000:
            raw -= 0x10000

        return raw

    def read_ina219_bus_raw(self, tca_addr: int, tca_channel: int, ina_addr: int) -> int:
        REG_INA219_BUS = 0x02

        self._open_tca_channel(tca_addr, tca_channel)
        raw = self.bus.read_word_data(ina_addr, REG_INA219_BUS)
        raw = ((raw & 0xFF) << 8) | (raw >> 8)
        return raw

    def _open_tca_channel(self, tca_addr: int, tca_channel: int) -> None:
        self._close_all_tcas()
        self.bus.write_byte(tca_addr, 1 << tca_channel)

    def close(self) -> None:
        try:
            self.bus.close()
        except Exception:
            pass


class MockRegisterReader:
    """Mock reader for UI / logic testing without hardware."""

    def __init__(self) -> None:
        self.present = {
            0x10: 0x02,
            0x11: 0x02,
            0x12: 0x03,
            0x13: 0x02,
            0x14: 0x04,
            0x15: 0x02,
        }
        self.tick = 0
        self.drop_after = 80  # simulate signal loss after some time

    def read_ina219_shunt_raw(self, tca_addr: int, tca_channel: int, ina_addr: int) -> int:
        mock_currents_mA = {
            (0x72, 0): 5.0,  # PORT_0  gray
            (0x70, 0): 50.0,  # PORT_1  green
            (0x70, 1): 250.0,  # PORT_2  green
            (0x70, 2): 350.0,  # PORT_3  yellow
            (0x70, 3): 500.0,  # PORT_4  yellow
            (0x71, 0): 750.0,  # PORT_5  red
            (0x71, 1): 800.0,  # PORT_6  red
            (0x71, 2): 100,  # PORT_7  no data
            (0x71, 3): 120.0,  # PORT_8  予備
        }

        value = mock_currents_mA.get((tca_addr, tca_channel), 20.0)

        if value is None:
            raise OSError("Mock no device")

        # current_mA = shunt_raw * 0.01 / 0.1 = shunt_raw * 0.1
        # よって shunt_raw = current_mA / 0.1
        return int(value / 0.1)

    def read_ina219_bus_raw(self, tca_addr: int, tca_channel: int, ina_addr: int) -> int:
        base = 1200 + (tca_channel % 3)  # 1200 * 4mV = 4.8V
        return base << 3

    def read_node(self, address: int) -> RawNodeData:
        self.tick += 1

        if address not in self.present:
            raise OSError(f"No device at {hex(address)}")

        # Simulate one device disappearing after it had once been visible.
        if address == 0x15 and self.tick > self.drop_after:
            raise OSError(f"Signal lost at {hex(address)}")

        # Occasional transient error for UNKNOWN testing.
        if address == 0x13 and random.random() < 0.05:
            raise OSError(f"Transient read error at {hex(address)}")

        module_type = self.present[address]

        if address == 0x10:
            status = 0x21  # DATA_READY + READY
            cmd = 0x00
        elif address == 0x11:
            status = 0x22  # DATA_READY + BUSY
            cmd = 0x01
        elif address == 0x12:
            status = 0x29  # DATA_READY + WARN + READY
            cmd = 0x10
        elif address == 0x13:
            status = 0x24  # DATA_READY + ERROR
            cmd = 0x01
        elif address == 0x14:
            status = 0x30  # DATA_READY + ESTOP
            cmd = 0x08
        else:
            status = 0x22  # busy
            cmd = 0x01

        return RawNodeData(
            address=address,
            module_type=module_type,
            status_raw=status,
            cmd_raw=cmd,
        )
