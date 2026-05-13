# Controller_Board

---

<img width="2016" height="1512" alt="IMG_1266" src="https://github.com/user-attachments/assets/4e713d58-47d6-476f-9424-5ad72323e8e6" />

---

This directory contains manufacturing and schematic data for the Controller_Board.

## Purpose

The `Controller_Board` provides the hardware interface between the Raspberry Pi 5 and the distributed I²C modules.

It organizes the I²C connection and related wiring so that the central controller (Raspberry Pi 5) can communicate with motor, servo, sensor, and monitoring boards.

---

## Role in the system

This board is connected to the Raspberry Pi 5 and acts as the physical interface for the modular I²C bus.

```text
Raspberry Pi 5
      ↓
Controller_Board
      ↓
I²C-connected hardware modules
```

It helps reduce wiring complexity and provides a consistent connection point for the rest of the system.

The Raspberry Pi Pico mounted on this board interprets commands received from the Raspberry Pi 5 via I²C and controls the actuators.

The command format is standardized so that it is common to all actuators and sensors.

The role of this board is to interpret this format and generate the actual control signals.

---

## Directory Structure

- [`gerber/`](./gerber/) : Gerber data used for PCB manufacturing
- [`schematic/`](./schematic/) : circuit schematic files
- `README.md` : overview of this board

---

## Manufacturing Data

Gerber data for this board is stored in:

- [`gerber/`](./gerber/)

The schematic is stored in:

- [`schematic/`](./schematic/)

---

## BOM

- DigiKey MyList: [Board BOM](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
