from __future__ import annotations

from enum import Enum
from typing import List, cast

from constants import (
    BIT_READY,
    BIT_BUSY,
    BIT_ERROR,
    BIT_WARN,
    BIT_ESTOP,
    STATUS_FLAG_ORDER,
    CMD_NAME_MAP,
    STATE_COLOR_VALUES,
)

class DisplayState(str, Enum):
    SESSION_ENDED = "Session Ended"
    NO_DEVICE = "No Device"
    SIGNAL_LOST = "Signal Lost"
    ESTOP = "ESTOP"
    ERROR = "ERROR"
    WARN = "WARN"
    BUSY = "BUSY"
    READY = "READY"
    UNKNOWN = "UNKNOWN"


STATE_COLORS: dict[DisplayState, dict[str, str]] = {
    cast(DisplayState, DisplayState[name]): value
    for name, value in STATE_COLOR_VALUES.items()
}


def decode_status_flags(status_raw: int) -> List[str]:
    flags: List[str] = []
    for bit, name in STATUS_FLAG_ORDER:
        if status_raw & (1 << bit):
            flags.append(name)
    return flags


def decode_cmd_name(cmd_raw: int) -> str:
    return CMD_NAME_MAP.get(cmd_raw, f"0x{cmd_raw:02X}")


def determine_protocol_state(status_raw: int) -> DisplayState:
    if status_raw & (1 << BIT_ESTOP):
        return DisplayState.ESTOP
    if status_raw & (1 << BIT_ERROR):
        return DisplayState.ERROR
    if status_raw & (1 << BIT_WARN):
        return DisplayState.WARN
    if status_raw & (1 << BIT_BUSY):
        return DisplayState.BUSY
    if status_raw & (1 << BIT_READY):
        return DisplayState.READY
    return DisplayState.UNKNOWN


def merge_display_state(protocol_state: DisplayState, current_warn: bool) -> DisplayState:
    if protocol_state == DisplayState.ESTOP:
        return DisplayState.ESTOP
    if protocol_state == DisplayState.ERROR:
        return DisplayState.ERROR
    if protocol_state == DisplayState.WARN:
        return DisplayState.WARN
    if current_warn and protocol_state in (DisplayState.BUSY, DisplayState.READY):
        return DisplayState.WARN
    return protocol_state