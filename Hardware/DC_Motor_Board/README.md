# DC_Motor_Board

---

<img width="2016" height="1512" alt="IMG_1265" src="https://github.com/user-attachments/assets/c9682a4a-c9f7-4dca-b592-62784203ba1d" />

---

This directory contains hardware design files for the DC_Motor_Board.

The DC_Motor_Board provides the motor driver interface for DC motor-based motion mechanisms in the Modular Factory System.

---

## Purpose

The `DC_Motor_Board` drives DC motors used in tabletop factory mechanisms.

It is designed for mechanisms such as conveyor belts, take-up rollers, and rack-and-pinion actuators that require simple rotational motion control.

---

## Role in the system

This board receives local control signals from a Pico-based control node and drives a connected DC motor.

```text
Raspberry Pi 5
      ↓ I²C commands
Controller / motor control node
      ↓ local motor control signals
DC_Motor_Board
      ↓ motor output
DC motor / conveyor / actuator mechanism
```

The host-side software sends standardized commands through the I²C control architecture.

The local controller interprets those commands and drives the DC motor through this board.

---

## Main Functions
- drives DC motors used in physical demo mechanisms
- supports start / stop motor control
- supports speed or duty-style control depending on firmware configuration
- supports timed motor operation
- supports direction control if used with compatible firmware and wiring
- provides a reusable motor output interface for different tabletop mechanisms

---

## Main Components

| Component	| Purpose |
|---|---|
| Motor driver IC / driver circuit | Drives the connected DC motor |
|Motor connector | Output connection to the DC motor |
|Control signal connector | Receives local control signals from the controller node|
|Power connector | Supplies motor-side power |
|Passive components |	Decoupling, filtering, and electrical support |
|Protection / support components | Helps stabilize motor operation and wiring, if mounted |

---

## BOM
- DigiKey MyList: [DC_Motor_Board](https://www.digikey.jp/ja/mylists/list/UO08XWYEAF)

---

## Related Documents

- Hardware overview → [../README.md](../README.md) 
- Manufacturing index → [../Manufacturing/](../Manufacturing/)
- Firmware motor node → [../../Firmware/motor_node/](../../Firmware/motor_node/)
