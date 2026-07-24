# DC_Motor_Board

---

<img width="2016" height="1512" alt="IMG_1265" src="https://github.com/user-attachments/assets/c9682a4a-c9f7-4dca-b592-62784203ba1d" />

---

This directory contains the hardware design files and documentation for the `DC_Motor_Board`.

The board provides the electrical interface between the Pico-based Controller Board, an external motor-power path, an AE-TB6612 motor-driver module, and a connected DC motor.

---

## Purpose

The `DC_Motor_Board` provides a reusable interface for controlling one DC motor in the Modular Factory System.

It separates three paths:

```text
control path
Controller Board
    ↓ PWM / direction / standby
AE-TB6612 motor driver

motor-power path
Power board
    ↓ motor supply
AE-TB6612 motor driver

motor-output path
AE-TB6612 motor driver
    ↓ H-bridge output
DC motor
```

The board itself does not define motion sequences, timing, target completion, or lifecycle behavior. Those functions are implemented by the motor-node firmware and host-side control system.

---

## Role in the System

```text
Raspberry Pi 5
      ↓ I²C commands
Controller Board
      ↓ GPIO control signals
DC_Motor_Board
      ↓ AE-TB6612 H-bridge output
DC motor / conveyor / actuator
```

The host sends standardized register commands over I²C.

The Controller Board interprets those commands through firmware and drives the motor-control inputs exposed by this board.

---

## Responsibility Boundary

### DC Motor Board hardware

- receives local control signals from the Controller Board
- routes PWM, direction, and standby signals to the motor-driver module
- receives external motor power
- forwards motor power to the driver module
- receives H-bridge output from the driver module
- forwards motor output to the connected DC motor
- provides local decoupling near the driver and supply interfaces

### Motor-node firmware

- determines PWM duty
- selects forward, reverse, brake, or stop behavior
- controls standby
- implements timed operation
- applies target and lifecycle rules
- stops outputs during E-STOP
- reports state and feedback through the common register interface

### AE-TB6612 motor-driver module

- performs H-bridge switching
- drives the connected motor from the supplied motor-power rail
- translates logic-level control inputs into motor output

---

## Controller Board Interface

`J1` is the common 2×10 interface to the Controller Board.

Only four digital control positions are used by this board.

| J1 pin pair | Controller Board signal | Pico GPIO | Motor function |
|---|---|---:|---|
| 3–4 | `GPIO_BUS_0` | GPIO2 | `PWMA` |
| 5–6 | `GPIO_BUS_1` | GPIO3 | `AIN2` |
| 7–8 | `GPIO_BUS_2` | GPIO4 | `AIN1` |
| 9–10 | `GPIO_BUS_3` | GPIO5 | `STBY` |

The odd-numbered pin in each pair is ground.

The remaining digital GPIO positions and `ADC_IN` are not used by this board.

---

## Motor-Control Signal Mapping

The board routes four control signals to the A-channel inputs of the AE-TB6612 motor-driver module.

| Board signal | Driver input | Purpose |
|---|---|---|
| `PWMA_SIG` | `PWMA` | PWM / duty control |
| `AIN1_SIG` | `AIN1` | Direction and brake control |
| `AIN2_SIG` | `AIN2` | Direction and brake control |
| `STBY_SIG` | `STBY` | Driver standby control |

These signal assignments match the current motor-node firmware mapping:

```text
GPIO2 → PWMA
GPIO3 → AIN2
GPIO4 → AIN1
GPIO5 → STBY
```

Forward, reverse, short-brake, stop, and duty behavior depend on the firmware output combinations.

---

## Motor-Driver Module

The current board is designed around the following external driver module:

- [AE-TB6612 motor driver kit](https://akizukidenshi.com/catalog/g/g111219/)

The board uses one motor channel of the module.

The current design acts as an interface and carrier around the external module rather than integrating the TB6612FNG IC directly.

A future revision may integrate the TB6612FNG directly onto the PCB.

---

## Motor-Power Path

Motor power is supplied from an upstream power board.

```text
external power board
        ↓
board power-input connector
        ↓
driver power-feed connector
        ↓
AE-TB6612 motor driver
```

The DC Motor Board does not generate or regulate the motor supply. It receives and routes the external supply to the driver module.

---

## Motor-Output Path

The driver output is returned to the board and then forwarded to the motor connector.

```text
AE-TB6612 motor driver
        ↓
driver-output return connector
        ↓
motor-output connector
        ↓
DC motor
```

This keeps the driver module, board wiring, and external motor connection physically separable.

---

## Decoupling and Noise Control

DC motors can generate substantial electrical noise through brush commutation, switching current, wiring inductance, back-EMF, and rapid load changes.

### Driver VCC decoupling

Near the driver connector:

- 0.1 µF capacitor
- 47 µF capacitor

These provide local high-frequency and bulk decoupling for the motor-driver VCC rail.

### Board-level decoupling

Additional supply decoupling includes:

- VCC: 10 µF and 0.1 µF
- +5 V: 220 µF and 0.1 µF

These capacitors provide a minimum level of local supply stabilization and noise protection.

They do not replace motor-terminal suppression, controlled wiring, system-level filtering, grounding review, or validation under the actual motor load.

---

## Current Status

Implemented in hardware:

- Controller Board 2×10 interface
- GPIO2–GPIO5 motor-control mapping
- `PWMA`, `AIN1`, `AIN2`, and `STBY` routing
- AE-TB6612 module connector
- external motor-power input
- driver power-feed connector
- driver-output return connector
- motor-output connector
- driver-side local decoupling
- board-level VCC and +5 V decoupling

Implemented jointly with firmware:

- start and stop behavior
- PWM duty control
- direction control
- short-brake or coast behavior
- standby control
- timed operation
- E-STOP output shutdown
- register-based motor-node behavior

Not implemented on this board:

- current sensing
- encoder feedback
- speed measurement
- closed-loop motor control
- hardware motion limits
- integrated motor-driver IC
- independent hardware safety shutdown

---

## Current Constraints

- designed for tabletop proof-of-concept mechanisms
- uses an external AE-TB6612 driver module
- only one motor channel is used
- remaining Controller Board GPIO and ADC resources are unused here
- motor behavior depends on firmware
- no onboard current or speed feedback
- no hardware-enforced actuator shutdown independent of firmware
- no galvanic isolation
- no integrated motor-terminal suppression
- actual noise performance depends on motor, wiring, load, and power architecture
- not qualified as an industrial motor-control or safety device

---

## Extension Points

- direct TB6612FNG integration
- onboard current sensing
- encoder input
- closed-loop speed control
- motor-terminal suppression components
- stronger power-input protection
- replaceable fuse or resettable protection
- hardware safe-state gating
- dedicated fault output
- connector keying
- improved test points
- standardized harnesses
- use of the second TB6612 channel
- migration to a different driver for higher voltage or current

---

## BOM

- DigiKey MyList: [DC_Motor_Board](https://www.digikey.jp/ja/mylists/list/UO08XWYEAF)
- External motor-driver module: [AE-TB6612](https://akizukidenshi.com/catalog/g/g111219/)

The AE-TB6612 module is not sourced from DigiKey.

Before ordering, verify connector orientation, pin assignment, capacitor polarity, motor voltage and current, driver-module compatibility, package, footprint, availability, and lifecycle status.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB production data | Gerber files in this directory |
| Controller Board pin mapping | Current schematic and Controller Board README |
| Motor-control GPIO behavior | Motor-node firmware |
| Driver-module pin usage | Current schematic and AE-TB6612 documentation |
| Motor-power routing | Current schematic |
| Motor-output routing | Current schematic |
| Part selection | DigiKey MyList, schematic, and linked driver module |
| Timing and lifecycle behavior | Firmware common core and motor-node implementation |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- Power Monitor Board → [`../Power_Monitor_Board/`](../Power_Monitor_Board/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Motor-node firmware → [`../../Firmware/motor_node/`](../../Firmware/motor_node/)
- Common firmware → [`../../Firmware/common/`](../../Firmware/common/)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
- PORT_X / MAIN_LINE power-monitor SQL → [`../../Software/Grafana/sql/PORT_X%20MAIN_LINE/`](../../Software/Grafana/sql/PORT_X%20MAIN_LINE/)

