# constants.py

# ----------------------
# App / UI
# ----------------------
DEFAULT_POLLING_MS = 100
POLLING_OPTIONS_MS = [50, 100, 200]

MAX_UI_EVENT_LINES = 100

# ----------------------
# Address Range
# ----------------------
ADDRESS_START = 0x10
ADDRESS_END = 0x19

# ----------------------
# Debugger Logic
# ----------------------
MAX_CONSECUTIVE_ERRORS = 3

# ----------------------
# Power UI
# ----------------------
UI_POWER_PORT_COUNT = 8
MAIN_CURRENT_LIMIT_MA = 3000.0
MAIN_POWER_LIMIT_W = 15.0

# ----------------------
# Reader
# ----------------------
DEFAULT_I2C_BUS = 1

# ----------------------
# Session
# ----------------------
SESSION_PREFIX = "dbg"

# ----------------------
# Colors
# ----------------------
COLOR_GRAY = "#555555"
COLOR_GREEN = "#00AA00"
COLOR_YELLOW = "#C9A000"
COLOR_RED = "#CC0000"

# ----------------------
# Module Types
# ----------------------
MODULE_TYPE_NAMES: dict[int, str] = {
    0x00: "Generic",
    0x02: "DC Motor",
    0x03: "Sensor",
    0x04: "Servo",
}

# ----------------------
# Node Status Bits
# ----------------------
BIT_READY = 0
BIT_BUSY = 1
BIT_ERROR = 2
BIT_WARN = 3
BIT_ESTOP = 4
BIT_DATA_READY = 5

STATUS_FLAG_ORDER = [
    (BIT_DATA_READY, "DATA_READY"),
    (BIT_ESTOP, "ESTOP"),
    (BIT_WARN, "WARN"),
    (BIT_ERROR, "ERROR"),
    (BIT_BUSY, "BUSY"),
    (BIT_READY, "READY"),
]

CMD_NAME_MAP = {
    0x00: "STOP / NOP",
    0x01: "RUN",
    0x08: "ESTOP_RESET",
    0x10: "LATCH_APPLY",
}

STATE_COLOR_VALUES: dict[str, dict[str, str]] = {
    "SESSION_ENDED": {"bg": "#7A6EA8", "fg": "white"},
    "NO_DEVICE": {"bg": "#666666", "fg": "white"},
    "SIGNAL_LOST": {"bg": "#C6C6C6", "fg": "black"},
    "ESTOP": {"bg": "#F58A8A", "fg": "white"},
    "ERROR": {"bg": "#FF1717", "fg": "white"},
    "WARN": {"bg": "#E8EB7B", "fg": "black"},
    "BUSY": {"bg": "#B8E5BE", "fg": "black"},
    "READY": {"bg": "#AFC6E6", "fg": "black"},
    "UNKNOWN": {"bg": "#FFFFFF", "fg": "black"},
}