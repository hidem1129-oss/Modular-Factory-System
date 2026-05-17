#!/usr/bin/env python3
from __future__ import annotations

import time
from dataclasses import dataclass

try:
    from smbus2 import SMBus
except ImportError as e:
    raise SystemExit("smbus2 is not installed. Run: pip install smbus2") from e


BUS_NO = 1

# ---- I2C addresses ----
STAMP_ADDR = 0x11
PAPER_ADDR = 0x18

CLAMP_SERVO_ADDRS = [0x13, 0x17]

LOOP = 10

# ---- Registers ----
REG_OP_MODE = 0x1D
REG_CMD = 0x30

REG_SETPOINT0_H = 0x31
REG_SETPOINT0_L = 0x32
REG_TARGET_TIME_H = 0x35
REG_TARGET_TIME_L = 0x36

OP_MODE_TIME = 0x02

CMD_STOP = 0x00
CMD_RUN = 0x01
CMD_DIR = 0x02
CMD_LATCH_APPLY = 0x10

LATCH_WAIT = 0.05
CMD_CLEAR_WAIT = 0.05


@dataclass(frozen=True)
class DCMotion:
    label: str
    addr: int
    speed: int
    tick: int
    wait: float
    direction: int  # 0 or 1


@dataclass(frozen=True)
class ServoMotion:
    label: str
    degrees: dict[int, int]
    target_time: int
    wait: float


# ---- Stamp rack-pinion motion ----
PRESS = DCMotion(
    label="PRESS",
    addr=STAMP_ADDR,
    speed=700,
    tick=35,
    wait=1.0,
    direction=0,
)

RELEASE = DCMotion(
    label="RELEASE",
    addr=STAMP_ADDR,
    speed=700,
    tick=35,
    wait=1.0,
    direction=1,
)

# ---- Paper take-up motor ----
# 方向確認結果に合わせて direction=1
TAKEUP = DCMotion(
    label="PAPER TAKE-UP",
    addr=PAPER_ADDR,
    speed=800,
    tick=20,
    wait=0.60,
    direction=1,
)

# ---- Paper clamp servos ----
# まずは安全寄り。押さえが弱ければ CLAMP_DEGREE を増やす。
# 逆向きなら CLAMP_DEGREE と RELEASE_DEGREE を入れ替える。
CLAMP = ServoMotion(
    label="CLAMP PAPER",
    degrees={
        0x13: 50,
        0x17: 95,
    },
    target_time=300,
    wait=0.40,
)

UNCLAMP = ServoMotion(
    label="UNCLAMP PAPER",
    degrees={
        0x13: 15,
        0x17: 130,
    },
    target_time=300,
    wait=0.40,
)

STAMP_HOLD = 0.5
STAMP_INTERVAL = 0.50


def set_u16(bus: SMBus, addr: int, reg_h: int, reg_l: int, value: int) -> None:
    hi = (value >> 8) & 0xFF
    lo = value & 0xFF
    bus.write_byte_data(addr, reg_h, hi)
    bus.write_byte_data(addr, reg_l, lo)


def clear_cmd(bus: SMBus, addr: int) -> None:
    bus.write_byte_data(addr, REG_CMD, CMD_STOP)
    time.sleep(CMD_CLEAR_WAIT)


def setup_time_mode(bus: SMBus, addr: int, setpoint0: int, target_time: int) -> None:
    bus.write_byte_data(addr, REG_OP_MODE, OP_MODE_TIME)
    set_u16(bus, addr, REG_SETPOINT0_H, REG_SETPOINT0_L, setpoint0)
    set_u16(bus, addr, REG_TARGET_TIME_H, REG_TARGET_TIME_L, target_time)


def latch_dc_motion(bus: SMBus, motion: DCMotion) -> None:
    if motion.direction == 0:
        bus.write_byte_data(motion.addr, REG_CMD, CMD_LATCH_APPLY)
        time.sleep(LATCH_WAIT)
        bus.write_byte_data(motion.addr, REG_CMD, CMD_STOP)
    else:
        bus.write_byte_data(motion.addr, REG_CMD, CMD_LATCH_APPLY | CMD_DIR)
        time.sleep(LATCH_WAIT)
        bus.write_byte_data(motion.addr, REG_CMD, CMD_DIR)

    time.sleep(LATCH_WAIT)


def run_dc_motion_cmd(bus: SMBus, motion: DCMotion) -> None:
    if motion.direction == 0:
        bus.write_byte_data(motion.addr, REG_CMD, CMD_RUN)
    else:
        bus.write_byte_data(motion.addr, REG_CMD, CMD_RUN | CMD_DIR)


def run_dc_time(bus: SMBus, motion: DCMotion) -> None:
    print(
        f"{motion.label}: "
        f"addr=0x{motion.addr:02X} "
        f"speed={motion.speed} "
        f"tick={motion.tick} "
        f"dir={motion.direction}"
    )

    clear_cmd(bus, motion.addr)
    setup_time_mode(bus, motion.addr, motion.speed, motion.tick)
    latch_dc_motion(bus, motion)
    run_dc_motion_cmd(bus, motion)

    time.sleep(motion.wait)

    clear_cmd(bus, motion.addr)


def run_single_servo(bus: SMBus, addr: int, degree: int, target_time: int) -> None:
    clear_cmd(bus, addr)
    setup_time_mode(bus, addr, degree, target_time)

    # Servo does not use DIR. Apply latch, clear, then RUN.
    bus.write_byte_data(addr, REG_CMD, CMD_LATCH_APPLY)
    time.sleep(LATCH_WAIT)
    bus.write_byte_data(addr, REG_CMD, CMD_STOP)
    time.sleep(LATCH_WAIT)

    bus.write_byte_data(addr, REG_CMD, CMD_RUN)


def run_servo_motion(bus: SMBus, motion: ServoMotion) -> None:
    degrees_text = ", ".join(
        f"0x{addr:02X}:{degree}deg"
        for addr, degree in motion.degrees.items()
    )

    print(
        f"{motion.label}: "
        f"degrees=[{degrees_text}] "
        f"target_time={motion.target_time}"
    )

    for addr, degree in motion.degrees.items():
        run_single_servo(bus, addr, degree, motion.target_time)

    time.sleep(motion.wait)

    for addr in motion.degrees.keys():
        clear_cmd(bus, addr)


def cleanup_all(bus: SMBus) -> None:
    for addr in {STAMP_ADDR, PAPER_ADDR, *CLAMP_SERVO_ADDRS}:
        try:
            clear_cmd(bus, addr)
        except Exception as e:
            print(f"cleanup failed: addr=0x{addr:02X}: {e}")


def do_stamp_cycle(bus: SMBus, cycle: int) -> None:
    print(f"=== Stamp cycle {cycle} / {LOOP} ===")

    run_servo_motion(bus, CLAMP)

    run_dc_time(bus, PRESS)

    print(f"HOLD stamp: {STAMP_HOLD}s")
    time.sleep(STAMP_HOLD)

    run_dc_time(bus, RELEASE)

    run_servo_motion(bus, UNCLAMP)

    run_dc_time(bus, TAKEUP)

    print(f"Interval: {STAMP_INTERVAL}s")
    time.sleep(STAMP_INTERVAL)


def main() -> None:
    print("Start stamp pressing demo")
    print(f"BUS={BUS_NO}")
    print(f"STAMP_ADDR=0x{STAMP_ADDR:02X} PAPER_ADDR=0x{PAPER_ADDR:02X} LOOP={LOOP}")
    print(f"CLAMP_SERVO_ADDRS={[f'0x{addr:02X}' for addr in CLAMP_SERVO_ADDRS]}")
    print(f"PRESS   speed={PRESS.speed} tick={PRESS.tick} wait={PRESS.wait} dir={PRESS.direction}")
    print(f"HOLD    wait={STAMP_HOLD}")
    print(f"RELEASE speed={RELEASE.speed} tick={RELEASE.tick} wait={RELEASE.wait} dir={RELEASE.direction}")
    print(f"CLAMP   degrees={CLAMP.degrees} target_time={CLAMP.target_time} wait={CLAMP.wait}")
    print(f"UNCLAMP degrees={UNCLAMP.degrees} target_time={UNCLAMP.target_time} wait={UNCLAMP.wait}")
    print(f"TAKEUP  speed={TAKEUP.speed} tick={TAKEUP.tick} wait={TAKEUP.wait} dir={TAKEUP.direction}")
    print(f"INTERVAL={STAMP_INTERVAL}")

    with SMBus(BUS_NO) as bus:
        try:
            for i in range(1, LOOP + 1):
                do_stamp_cycle(bus, i)
        finally:
            cleanup_all(bus)

    print("Done")


if __name__ == "__main__":
    main()
