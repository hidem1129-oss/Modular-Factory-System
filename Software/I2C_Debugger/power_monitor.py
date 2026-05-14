from __future__ import annotations

import time
from typing import Callable

from power_calc import shunt_raw_to_current_mA, bus_raw_to_voltage_V
from power_config import POWER_PORTS, PowerPortConfig


class PowerMonitorService:
    def __init__(self, event_logger, session_id: str) -> None:
        self.event_logger = event_logger
        self.session_id = session_id

    def get_reader_functions(self, reader):
        read_shunt_fn = getattr(reader, "read_ina219_shunt_raw", None)
        read_bus_fn = getattr(reader, "read_ina219_bus_raw", None)

        if not callable(read_shunt_fn):
            print("[POWER PORT] read_ina219_shunt_raw not found")
            return None, None

        if not callable(read_bus_fn):
            print("[POWER PORT] read_ina219_bus_raw not found")
            return None, None

        return read_shunt_fn, read_bus_fn

    def read_single_power_port(
        self,
        port: PowerPortConfig,
        read_shunt_fn: Callable,
        read_bus_fn: Callable,
    ) -> dict[str, float | int | None]:
        shunt_raw = None
        current_mA = None
        bus_voltage_V = None
        power_mW = None

        try:
            shunt_raw = int(read_shunt_fn(port.tca_addr, port.tca_channel, port.ina_addr))
            current_mA = round(
                shunt_raw_to_current_mA(shunt_raw, port.shunt_ohms),
                3,
            )
        except Exception as e:
            print(f"[POWER PORT][{port.port_id}] shunt read failed: {e}")

        try:
            bus_raw = int(read_bus_fn(port.tca_addr, port.tca_channel, port.ina_addr))
            bus_voltage_V = round(bus_raw_to_voltage_V(bus_raw), 3)
        except Exception as e:
            print(f"[POWER PORT][{port.port_id}] bus read failed: {e}")

        if bus_voltage_V is not None and current_mA is not None:
            power_mW = round(bus_voltage_V * current_mA, 3)

        return {
            "shunt_raw": shunt_raw,
            "current_mA": current_mA,
            "bus_voltage_V": bus_voltage_V,
            "power_mW": power_mW,
        }

    def collect_snapshot(self, reader) -> dict[str, dict[str, float | None]]:
        read_shunt_fn, read_bus_fn = self.get_reader_functions(reader)
        if read_shunt_fn is None or read_bus_fn is None:
            return {}

        now = time.time()
        power_snapshot: dict[str, dict[str, float | None]] = {}

        for port in POWER_PORTS:
            port_data = self.read_single_power_port(port, read_shunt_fn, read_bus_fn)

            power_snapshot[port.port_id] = {
                "current_mA": port_data["current_mA"],
                "bus_voltage_V": port_data["bus_voltage_V"],
                "power_mW": port_data["power_mW"],
            }

            self.event_logger.log_power_port_snapshot(
                ts_epoch=now,
                port_id=port.port_id,
                tca_addr=port.tca_addr,
                tca_channel=port.tca_channel,
                ina_addr=port.ina_addr,
                bus_voltage_V=port_data["bus_voltage_V"],
                shunt_raw=port_data["shunt_raw"],
                current_mA=port_data["current_mA"],
                power_mW=port_data["power_mW"],
                session_id=self.session_id,
            )

        self.event_logger.commit()
        return power_snapshot