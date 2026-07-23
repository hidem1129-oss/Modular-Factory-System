# Controller_Board

---

<img width="2016" height="1512" alt="IMG_1266" src="https://github.com/user-attachments/assets/4e713d58-47d6-476f-9424-5ad72323e8e6" />

---

This directory contains the hardware design files and documentation for the `Controller_Board`.

The board is a common Raspberry Pi Pico WH-based local controller used between the Raspberry Pi 5 host and device-specific actuator or sensor boards.

It provides:

- daisy-chained I²C, power, and E-STOP routing
- a local active-low E-STOP input
- an active-low acknowledgement input
- a 4-bit hardware address selector
- eight digital GPIO lines
- one analog input
- assembly-selectable 5 V or 3.3 V peripheral power
- local RGB indication hardware

The board provides the electrical interfaces required for distributed control. Register interpretation, node behavior, E-STOP state handling, and recovery logic are implemented jointly with firmware.

---

## Purpose

The `Controller_Board` acts as the reusable local-controller layer of the Modular Factory System.

Its role depends on the combination of:

```text
Controller_Board hardware
        +
loaded firmware profile
        +
connected peripheral board
```

The same controller hardware can therefore be reused with motor, servo, or sensor firmware.

---

## Role in the System

```text
Raspberry Pi 5
      ↓ I²C commands
Controller_Board
      ↓ GPIO / PWM / ADC / selected power
DC Motor Board / Servo Board / Sensor Board
      ↓
physical actuator or sensor
```

---

## Responsibility Boundary

### Hardware

- Pico WH mounting and power routing
- shared-bus connectors
- GPIO and ADC breakout
- peripheral power selection
- address DIP switch
- E-STOP and acknowledgement switches
- shared E-STOP routing
- RGB LED and support circuitry
- connector-side transient protection

### Firmware

- I²C slave initialization
- register interpretation
- GPIO, PWM, and ADC behavior
- E-STOP detection and latching
- shared E-STOP assertion and observation
- acknowledgement handling
- output shutdown and recovery
- feedback and status reporting
- RGB LED behavior, if used

### Peripheral boards

- motor-driver circuitry
- servo interface circuitry
- photo-reflector interface circuitry
- device-specific connectors and loads

---

## Shared Bus Interface

`J1` is the bus input and `J2` is the bus output. Both use the same 6-pin assignment.

| Pin | Signal | Purpose |
|---:|---|---|
| 1 | `+5V` | 5 V distribution |
| 2 | `GND_A` | Ground |
| 3 | `SDA` | I²C data |
| 4 | `GND_B` | Ground |
| 5 | `SCL` | I²C clock |
| 6 | `E_STOP_BUS_N` | Shared active-low E-STOP signal |

```text
previous node
      ↓ J1
Controller_Board
      ↓ J2
next node
```

The daisy-chain interface distributes power, I²C, and the shared E-STOP signal through multiple controller modules.

---

## I²C Signal Protection

SDA and SCL include:

- 33 Ω series resistors
- RClamp04022S two-line TVS protection near the bus connectors

The TVS devices are used mainly for ESD and transient-surge protection on the 3.3 V I²C signal paths.

The board does not provide level shifting, galvanic isolation, active buffering, or differential signaling.

---

## Shared E-STOP Bus

`E_STOP_BUS_N` is an active-low signal routed through J1 and J2.

Intended behavior:

```text
one node detects or asserts E-STOP
        ↓
E_STOP_BUS_N becomes low
        ↓
other nodes observe the shared stop state
        ↓
each node firmware stops its local output
```

The hardware provides the shared electrical path and Pico connections. Firmware implements the actual stop, latch, reporting, and recovery behavior.

---

## Local E-STOP Input

| Signal | Pico GPIO | Default | Active |
|---|---:|---|---|
| `LOCAL_ESTOP_N` | GPIO16 | High | Low |

Pressing the onboard switch pulls `LOCAL_ESTOP_N` to ground.

Firmware detects this input and may then assert the shared `E_STOP_BUS_N` signal.

---

## E-STOP Acknowledgement Input

| Signal | Pico GPIO | Pull-up | Active |
|---|---:|---:|---|
| `ACK_N` | GPIO15 | 22 kΩ to 3.3 V | Low |

Pressing `SW3` pulls `ACK_N` low.

This provides the hardware trigger for requesting recovery from a latched E-STOP condition.

Firmware decides whether:

- local E-STOP has been released
- shared E-STOP is inactive
- the latch may be cleared
- lifecycle state may return to normal
- outputs may restart

Pressing `SW3` does not by itself guarantee immediate recovery.

---

## Shared E-STOP Pull-Up Population

The shared E-STOP bus supports one of two alternative pull-up resistor populations:

- 22 kΩ
- 560 kΩ

Only one is intended to be populated on a board.

This allows the total effective pull-up resistance to be tuned across multiple connected boards, where all populated pull-ups appear in parallel.

The final choice should consider node count, cable length, bus capacitance, sink current, and measured behavior.

---

## Hardware I²C Address Selection

A four-position DIP switch provides the lower four bits of the node address.

| DIP signal | Pico GPIO | Weight |
|---|---:|---:|
| `DIP_0` | GPIO18 | bit 0 / `0x1` |
| `DIP_1` | GPIO19 | bit 1 / `0x2` |
| `DIP_2` | GPIO20 | bit 2 / `0x4` |
| `DIP_3` | GPIO21 | bit 3 / `0x8` |

Each input has a 47 kΩ pull-down.

- switch open → 0
- switch closed → 1

The hardware produces `0x0` to `0xF`.

Firmware combines this value with the `0x10` base address, producing effective addresses from `0x10` to `0x1F`.

```c
i2c_address = 0x10 | dip_value;
```

Two nodes with the same DIP setting will cause an address conflict.

---

## Peripheral Interface Connector

`J5` is the common 2×10 connector for actuator and sensor boards.

| Pin pair | Assignment |
|---|---|
| 1–2 | GND / selected peripheral power |
| 3–4 | GND / `GPIO_BUS_0` |
| 5–6 | GND / `GPIO_BUS_1` |
| 7–8 | GND / `GPIO_BUS_2` |
| 9–10 | GND / `GPIO_BUS_3` |
| 11–12 | GND / `GPIO_BUS_4` |
| 13–14 | GND / `GPIO_BUS_5` |
| 15–16 | GND / `GPIO_BUS_6` |
| 17–18 | GND / `GPIO_BUS_7` |
| 19–20 | GND / `ADC_IN` |

Every power, digital, and analog line is paired with a ground return.

---

## Peripheral GPIO Mapping

| Peripheral signal | Pico GPIO |
|---|---:|
| `GPIO_BUS_0` | GPIO2 |
| `GPIO_BUS_1` | GPIO3 |
| `GPIO_BUS_2` | GPIO4 |
| `GPIO_BUS_3` | GPIO5 |
| `GPIO_BUS_4` | GPIO6 |
| `GPIO_BUS_5` | GPIO7 |
| `GPIO_BUS_6` | GPIO8 |
| `GPIO_BUS_7` | GPIO9 |

The meaning of these signals depends on the connected peripheral board and firmware.

---

## Analog Input

`ADC_IN` is routed to Pico GPIO26 / ADC0.

The input includes:

- 1 kΩ series resistor
- 1 nF capacitor to ground

This provides light analog conditioning and high-frequency noise reduction.

Precision amplification, voltage translation, and physical-unit conversion are not implemented on this board.

---

## Peripheral Power Selection

The peripheral-board supply is selected at assembly time by populating one of two alternative 0 Ω links.

Available options:

- +5 V
- +3.3 V

Only one link is intended to be populated. Both must not be installed simultaneously.

This supports 5 V driver circuits or 3.3 V sensor and logic circuits using the same Controller Board design.

---

## Local RGB Indicator

The board includes a three-color LED intended for local status indication.

In practice, the multi-node assembly exposed usability limits:

- LEDs could be obscured by adjacent boards and wiring
- identifying the relevant node was not always immediate
- individual LEDs were less useful than host-side monitoring
- firmware support was required for meaningful indication

The RGB LED is therefore better treated as a development or troubleshooting interface than as the primary system-status display.

---

## Practical Usability Findings

### Local E-STOP accessibility

Although the onboard E-STOP switch is electrically valid, it could be difficult to identify and reach the correct controller in a dense multi-board assembly.

It should therefore be treated as a local development and test control rather than a complete system-level emergency-stop interface.

A dedicated and clearly accessible emergency-stop board is a possible future improvement.

### Harness reliability

A hand-built I²C harness experienced a wire break during use.

Possible future improvements include:

- commercially available ribbon cable
- matching IDC or equivalent connectors
- standardized replaceable harnesses
- dedicated cable test procedures

These broader system decisions are better documented in a separate ADR.

---

## Firmware Relationships

| Firmware | Typical peripheral |
|---|---|
| [`motor_node`](../../Firmware/motor_node/) | DC Motor Board |
| [`servo_node`](../../Firmware/servo_node/) | Servo Board |
| [`sensor_node`](../../Firmware/sensor_node/) | Sensor Board |

The common firmware defines register behavior, lifecycle state, address initialization, and E-STOP handling. Device-specific firmware defines how GPIO and ADC resources are used.

---

## Current Status

Implemented in hardware:

- Pico WH controller platform
- I²C daisy-chain connectors
- +5 V and ground routing
- SDA and SCL routing
- I²C series resistors and TVS protection
- shared active-low E-STOP route
- local E-STOP switch
- ACK switch
- 4-bit address DIP switch
- eight peripheral GPIO lines
- one ADC input
- assembly-selectable 5 V or 3.3 V peripheral power
- RGB LED hardware
- local decoupling

Implemented jointly with firmware:

- I²C slave address formation
- register-based node behavior
- E-STOP state handling
- acknowledgement-based recovery
- motor, servo, and sensor behavior

Not fully utilized or not prioritized:

- RGB LED status indication
- operator-facing use of the onboard E-STOP switch

---

## Current Constraints

- tabletop proof-of-concept design
- shared I²C bus is not isolated or actively buffered
- daisy-chain faults can affect multiple nodes
- duplicate DIP settings cause address conflicts
- peripheral power is fixed at assembly time
- ADC input is limited to the Pico ADC domain
- E-STOP behavior depends on firmware
- onboard E-STOP accessibility is limited
- RGB LED visibility is limited
- hand-built harness reliability may vary
- not an industrial safety controller
- no redundant communication or safety channel
- no hardware-enforced actuator shutdown independent of firmware

---

## Extension Points

- standardized ribbon-cable harnesses
- keyed IDC or equivalent connectors
- dedicated system-level emergency-stop board
- centralized emergency-stop control
- stack light or centralized status indicator
- optional removal of local RGB LED hardware
- retention of RGB LED for debugging only
- active I²C buffering
- differential communication
- hardware-enforced safe-state outputs
- branch isolation
- improved test points
- automatic board identification
- dedicated harness test fixture

---

## BOM

- DigiKey MyList: [Controller_Board](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78)

Before ordering, verify package, footprint, ratings, alternative-population options, connector orientation, availability, and lifecycle status.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB production data | Gerber files in this directory |
| J1/J2 bus pinout | Current schematic |
| J5 peripheral pinout | Current schematic |
| Address-selection hardware | Current schematic |
| Effective address behavior | Firmware common platform implementation |
| E-STOP and ACK behavior | Firmware common core and platform implementation |
| Peripheral GPIO behavior | Device-specific firmware |
| Part selection | DigiKey MyList and schematic |
| Practical findings | This README and future ADRs |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Pi5 Wiring Auxiliary → [`../Pi5_Wiring_Auxiliary/`](../Pi5_Wiring_Auxiliary/)
- DC Motor Board → [`../DC_Motor_Board/`](../DC_Motor_Board/)
- Servo Board → [`../Servo_Board/`](../Servo_Board/)
- Sensor Board → [`../Sensor_Board/`](../Sensor_Board/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Common firmware → [`../../Firmware/common/`](../../Firmware/common/)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
