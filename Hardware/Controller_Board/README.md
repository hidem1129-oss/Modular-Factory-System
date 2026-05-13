# Controller_Board

---

<img width="2016" height="1512" alt="IMG_1266" src="https://github.com/user-attachments/assets/4e713d58-47d6-476f-9424-5ad72323e8e6" />

---

This directory contains manufacturing and schematic data for the Controller_Board.

## Purpose

The `Controller_Board` is a Pico-based I²C control node used in the Modular Factory System.

It receives register-based commands from the Raspberry Pi 5 via I²C and translates them into local control signals for connected actuators or sensors.

This board is not only a wiring interface, but also a local control layer between the host controller and the physical hardware.

---

## Role in the system

This board acts as a local I²C-controlled hardware node.

```text
Raspberry Pi 5
      ↓ I²C commands
Controller_Board
      ↓ local control signals
Actuators / sensors / module circuitry
```

The Raspberry Pi 5 sends standardized commands over I²C.

The Raspberry Pi Pico mounted on this board interprets those commands and generates the actual control signals required by the connected hardware.

This makes the board a bridge between host-side orchestration and physical device control.

---

## Manufacturing Data

Gerber data for this board is stored in:

- [`gerber/`](./gerber/)

The schematic is stored in:

- [`schematic/`](./schematic/)

---

## BOM

- DigiKey MyList: [Controller_Board](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
