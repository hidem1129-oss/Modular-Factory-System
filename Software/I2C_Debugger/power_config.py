# power_config.py
from dataclasses import dataclass

@dataclass(frozen=True)
class PowerPortConfig:
    port_id: str
    label: str
    tca_addr: int
    tca_channel: int
    ina_addr: int
    shunt_ohms: float
    is_main: bool = False

POWER_PORTS = [
    PowerPortConfig("PORT_1", "U5",  0x70, 0, 0x40, 0.10),
    PowerPortConfig("PORT_2", "U6",  0x70, 1, 0x44, 0.10),
    PowerPortConfig("PORT_3", "U7",  0x70, 2, 0x41, 0.10),
    PowerPortConfig("PORT_4", "U8",  0x70, 3, 0x45, 0.10),
    PowerPortConfig("PORT_5", "U9",  0x71, 0, 0x40, 0.10),
    PowerPortConfig("PORT_6", "U10", 0x71, 1, 0x44, 0.10),
    PowerPortConfig("PORT_7", "U11", 0x71, 2, 0x41, 0.10),
    PowerPortConfig("PORT_8", "U12", 0x71, 3, 0x45, 0.10),
    PowerPortConfig("MAIN_LINE", "U13", 0x72, 0, 0x40, 0.05, True),
]

BRANCH_POWER_PORTS = [p for p in POWER_PORTS if not p.is_main]
MAIN_POWER_PORT = next(p for p in POWER_PORTS if p.is_main)
