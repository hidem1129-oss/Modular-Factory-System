# Servo_Board

---

<img width="2016" height="1512" alt="IMG_1264" src="https://github.com/user-attachments/assets/d3c5bc24-323a-4e86-a070-3e9131d77a80" />

---

This directory contains hardware design files for the Servo_Board.

The Servo_Board provides the hardware interface for servo motor-based actuator mechanisms in the Modular Factory System.

---

## Purpose

The `Servo_Board` provides the wiring and power interface for servo motors used in small position-controlled mechanisms.

It is designed for mechanisms such as sorting gates, diverter arms, paper clamp mechanisms, and other small actuator modules that require controlled angular movement.

---

## Role in the system

This board receives local control signals from a Pico-based control node and drives connected servo motors.

```text
Raspberry Pi 5
      ↓ I²C commands
Controller / servo control node
      ↓ local servo control signals
Servo_Board
      ↓ PWM output
Servo motor / gate / clamp mechanism
```

The host-side software sends standardized commands through the I²C control architecture.

The local controller interprets those commands and generates PWM signals for the servo motors through this board.

---

## Main Functions
- drives servo motors used in physical demo mechanisms
- supports position or angle-based actuator control
- supports timed servo movement depending on firmware configuration
- provides a reusable servo output interface for different tabletop mechanisms
- helps separate host-side orchestration from low-level servo signal generation
- supports mechanisms such as sorting gates, diverters, and clamps

---

## Main Components

| Component | Purpose |
|---|---|
| Servo connector | Output connection to the servo motor |
| Control signal connector | Receives local control signals from the controller node |
| Power connector | Supplies servo-side power |
| Damping resistor | Suppresses ringing and helps stabilize the signal |
| Passive components | Decoupling, filtering, and electrical support |

---

## BOM

- DigiKey MyList: [Servo_Board](https://www.digikey.jp/ja/mylists/list/1G5XREGH36)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Firmware servo node → [`../../Firmware/servo_node/`](../../Firmware/servo_node/)
