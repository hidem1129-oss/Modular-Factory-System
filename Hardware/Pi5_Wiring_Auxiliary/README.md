# Pi5_Wiring_Auxiliary

---

<img width="2016" height="1512" alt="IMG_1263" src="https://github.com/user-attachments/assets/40dbf589-2225-41d5-a115-78f62e907d39" />

---

This directory contains the hardware design files and documentation for the `Pi5_Wiring_Auxiliary` board.

The board acts as the host-side wiring boundary between the Raspberry Pi 5 and the distributed hardware modules in the Modular Factory System.

It distributes +5 V, +3.3 V, I²C SDA/SCL, and three dedicated TCA9548 reset-signal paths through a fixed module connector. It also provides adjustable I²C pull-ups, light series damping, and local power decoupling.

---

## Purpose

The `Pi5_Wiring_Auxiliary` board reduces direct wiring complexity around the Raspberry Pi 5.

Its main purposes are:

- providing a repeatable host-side connection point
- organizing I²C, power, and reset wiring
- reducing setup and replacement mistakes
- making the signal path easier to inspect and explain
- allowing I²C pull-up strength to be adjusted without modifying the PCB
- providing dedicated reset-signal paths for the TCA9548 multiplexers used by the power-monitoring boards

The board does not perform protocol conversion or power conversion. It routes and conditions signals already provided by the Raspberry Pi 5.

The hardware includes the electrical paths required for future host-controlled TCA9548 reset and recovery. The corresponding host-side reset-control software is not currently implemented.

---

## Role in the System

```text
Raspberry Pi 5
      ↓ GPIO, I²C, and power
Pi5_Wiring_Auxiliary
      ↓ fixed 10-pin interface
Controller boards and power-monitoring boards
      ↓
distributed factory modules
```

The board establishes a stable host-side wiring boundary so that downstream modules can be changed without repeatedly rewiring the Raspberry Pi 5 header.

---

## Distributed Signals

### Power

- `+5V_PI`
- `+3V3_PI`
- GND

### I²C

- `SCL_OUT`
- `SDA_OUT`

### Multiplexer reset paths

- `MUX_RST_0`
- `MUX_RST_1`
- `MUX_RST_2`

The three reset lines are routed for TCA9548 I²C multiplexers used on the power-monitoring hardware.

At present, these are hardware-prepared signal paths. Host-side software does not yet actively control them.

---

## Module Connector Pinout

The downstream module connector is `J8`, a 2×5 connector.

| Pin | Signal | Purpose |
|---:|---|---|
| 1 | `+5V_PI` | Main 5 V supply, primarily used for Raspberry Pi Pico power |
| 2 | GND | Ground |
| 3 | `MUX_RST_0` | Prepared reset path for TCA9548 multiplexer 0 |
| 4 | `SCL_OUT` | I²C clock |
| 5 | `MUX_RST_1` | Prepared reset path for TCA9548 multiplexer 1 |
| 6 | GND | Ground |
| 7 | `MUX_RST_2` | Prepared reset path for TCA9548 multiplexer 2 |
| 8 | `SDA_OUT` | I²C data |
| 9 | `+3V3_PI` | 3.3 V supply for INA219, TCA9548, and I²C pull-ups |
| 10 | GND | Ground |

The connector places ground near the I²C lines and distributes both 5 V and 3.3 V through the same interface.

---

## Power Distribution

### +5 V rail

`+5V_PI` is mainly used to supply Raspberry Pi Pico-based controller boards.

### +3.3 V rail

`+3V3_PI` is mainly used for:

- INA219 current and voltage monitor ICs
- TCA9548 I²C multiplexers
- I²C pull-up resistors

### Local decoupling

| Rail | Local capacitance |
|---|---|
| +5 V | 0.1 µF + 47 µF |
| +3.3 V | 0.1 µF + 47 µF |

The board does not generate either voltage rail.

---

## TCA9548 Reset-Signal Routing

`MUX_RST_0`, `MUX_RST_1`, and `MUX_RST_2` provide dedicated electrical paths from the Raspberry Pi 5 side to the reset inputs of TCA9548 multiplexers on downstream power-monitoring boards.

These paths are intended to support future host-controlled reset and recovery if:

- the multiplexer stops responding
- the I²C path becomes stuck
- software recovery is insufficient
- a known reset sequence is required during initialization or troubleshooting

Each reset line has a 10 kΩ pull-up to 3.3 V.

This keeps the reset input inactive during normal operation unless the Raspberry Pi 5 actively asserts reset.

---

## I²C Signal Conditioning

The board includes:

- 33 Ω series resistors on SDA and SCL
- one fixed 10 kΩ pull-up on each I²C line
- four jumper-selectable 10 kΩ pull-up branches on each line
- independent adjustment for SDA and SCL
- dedicated jumper-parking headers

The 33 Ω resistors provide light series damping near the Raspberry Pi 5 side. They are not level shifters and do not isolate the bus.

---

## Adjustable Pull-Up Network

SDA and SCL each use the same pull-up structure.

```text
+3.3 V
  │
  ├─ fixed 10 kΩ pull-up
  ├─ jumper-selectable 10 kΩ branch
  ├─ jumper-selectable 10 kΩ branch
  ├─ jumper-selectable 10 kΩ branch
  └─ jumper-selectable 10 kΩ branch
  │
SDA_OUT or SCL_OUT
```

With all adjustment jumpers open, only the fixed 10 kΩ resistor is active.

Installing jumpers connects additional 10 kΩ resistors in parallel.

| Active 10 kΩ paths | Effective resistance |
|---:|---:|
| 1 | 10 kΩ |
| 2 | 5 kΩ |
| 3 | approximately 3.33 kΩ |
| 4 | 2.5 kΩ |
| 5 | 2 kΩ |

This allows the pull-up strength to be adjusted from 10 kΩ to 2 kΩ.

### Adjustment headers

- `J3`: SCL pull-up selection
- `J4`: SDA pull-up selection
- `J5`: unused SCL jumper parking
- `J6`: unused SDA jumper parking

---

## Pull-Up Selection Guidance

A stronger pull-up may help when:

- cable length increases
- more modules are connected
- bus capacitance increases
- SDA or SCL rise time becomes too slow
- communication becomes unstable

The strongest setting is not automatically the best setting because lower resistance increases the sink current required from devices on the bus.

A practical adjustment approach is:

1. begin with the default 10 kΩ setting
2. check communication stability
3. inspect SDA and SCL with an oscilloscope when necessary
4. add pull-up branches gradually
5. stop when rise time and communication are acceptable

SDA and SCL can be adjusted independently.

---

## What the Board Does Not Provide

The current board does not provide:

- I²C level shifting
- galvanic isolation
- active bus buffering
- differential I²C signaling
- power regulation
- reverse-polarity protection
- branch-level current limiting
- automatic bus recovery
- automatic pull-up selection
- implemented host-side TCA9548 reset control

It is a passive host-side breakout, distribution, and signal-conditioning board.

---

## Main Components

| Component | Purpose |
|---|---|
| Raspberry Pi 5 GPIO header connection | Connects the board to the Raspberry Pi 5 |
| J8 2×5 module connector | Distributes power, I²C, ground, and prepared reset signals |
| 33 Ω series resistors | Light damping on SDA and SCL |
| Fixed 10 kΩ pull-ups | Default I²C pull-up path |
| Selectable 10 kΩ pull-up branches | Adjustable pull-up strength |
| Pull-up selection headers | Enable additional resistor branches |
| Jumper-parking headers | Store unused jumper shunts |
| 10 kΩ reset pull-ups | Hold TCA9548 reset lines inactive |
| 0.1 µF capacitors | High-frequency decoupling |
| 47 µF capacitors | Local bulk capacitance |

---

## Design Intent

The board was introduced to avoid concentrating temporary jumper wiring directly around the Raspberry Pi 5.

Its design priorities are:

- repeatable module wiring
- visible signal routing
- easier maintenance
- independent I²C pull-up adjustment
- hardware preparation for future host-controlled TCA9548 recovery
- practical tabletop assembly
- reproduction from standard components

---

## Example Use

```text
1. Connect the board to the Raspberry Pi 5 GPIO header.
2. Connect J8 to the downstream controller and monitoring harness.
3. Begin with SDA and SCL at the default 10 kΩ setting.
4. Confirm I²C communication with the connected nodes.
5. Add jumper-selected pull-up branches only when bus behavior requires them.
6. When host-side reset control is implemented, use `MUX_RST_0–2` to reset the corresponding TCA9548 devices.
```

Step 6 describes the intended future use of the prepared reset routes; it is not part of the currently implemented host software.

---

## Current Status

Implemented:

- Raspberry Pi 5 breakout and signal routing
- +5 V and +3.3 V distribution
- local supply decoupling
- SDA and SCL series resistors
- adjustable SDA and SCL pull-up networks
- J8 module connector
- three TCA9548 reset-signal routes
- default pull-ups on all three reset lines

Not implemented:

- active host-side control of `MUX_RST_0–2`
- automatic TCA9548 reset or fault recovery
- active I²C buffering
- level translation
- bus isolation
- onboard power conversion

---

## Current Constraints

- designed for the current 3.3 V I²C system
- fixed J8 pin assignment
- fixed 33 Ω series resistors
- pull-up adjustment range is 10 kΩ to 2 kΩ
- no active bus buffering
- no level translation
- no isolation
- no onboard power conversion
- reset paths are prepared for three TCA9548 devices but are not yet software-controlled
- suitability depends on cable length, node count, and bus capacitance
- intended for tabletop proof-of-concept use

---

## Extension Points

Possible future improvements include:

- host-side GPIO control of `MUX_RST_0–2`
- automatic TCA9548 reset and recovery logic
- reset-status logging
- integration with power-monitoring software
- active I²C buffering
- differential bus conversion
- configurable series damping
- protected GPIO outputs
- branch-level connectors
- additional reset channels
- branch fusing
- reverse-current protection
- power-good indication
- test points for SDA, SCL, reset, and power rails
- connector keying improvements
- automatic bus-recovery circuitry

---

## BOM

- DigiKey MyList: [Pi5_Wiring_Auxiliary](https://www.digikey.jp/ja/mylists/list/QO09LGCG39)

Before ordering, verify package, footprint, ratings, availability, and connector orientation.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB manufacturing data | Gerber files in this directory |
| Connector pinout | Current schematic |
| Pull-up options | Current schematic |
| Reset-path implementation | Current schematic and this README |
| Host-side reset behavior | Not currently implemented |
| Part selection | DigiKey MyList and schematic |
| System-level role | This README and [`../README.md`](../README.md) |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- Power Monitor Board → [`../Power_Monitor_Board/`](../Power_Monitor_Board/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
