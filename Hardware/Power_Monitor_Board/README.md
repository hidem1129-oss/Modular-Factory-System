# Power_Monitor_Board

---
<img width="2016" height="1512" alt="IMG_1260" src="https://github.com/user-attachments/assets/d4146cf6-dabb-4de8-b1d1-86981b8779a9" />

---

This directory contains hardware design files for the Power Monitor Board.

The Power Monitor Board distributes 5 V power to multiple branches and monitors voltage / current for each power line.

---

## Purpose

The `Power_Monitor_Board` provides power distribution and power monitoring for the Modular Factory System.

It is designed to support:

- centralized 5 V power distribution
- branch-level current and voltage monitoring
- I²C-based measurement access
- logging and visualization of power usage
- safer tabletop experiments with multiple actuator modules

---

## Role in the system

This board sits between the main 5 V power source and each hardware module.

```text
5 V Power Source
      ↓
Power_Monitor_Board
      ↓
Motor / Servo / Sensor / Controller modules
      ↓
Software logging and Grafana visualization
```
The board allows the system to observe how much current each branch consumes during operation.

This makes it useful not only as a power distribution board, but also as a diagnostic and visualization component.

---

## Main Functions

- distributes 5 V power to multiple output branches
- monitors branch voltage and current
- provides I²C access to measurement ICs
- supports per-branch power logging
- helps identify abnormal current draw or wiring issues
- provides measurement data for dashboards and analysis

---

## Main Components

| Component | Purpose |
|---|---|
| INA219 | Current / voltage monitoring |
| TCA9548A | I²C multiplexer for multiple monitoring channels |
| Shunt resistors | Current measurement |
| Terminal blocks / connectors | Power input and branch output |
| Switches / indicators | Power branch operation and visibility, if mounted |
| Passive components | Pull-ups, filtering, and support circuitry |

---

## Directory Structure

- [`gerber/`](./gerber/) : Gerber data used for PCB manufacturing
- [`schematic/`](./schematic/) : circuit schematic files
- README.md : overview of the Power Monitor Board

The exact file names may depend on the PCB CAD export settings.

---

## Manufacturing Data

Gerber data for this board is stored in:
- [`gerber/`](./gerber/)

The schematic is stored in:
- [`schematic/`](./schematic/)

The Gerber files can be used as prototype PCB manufacturing data.


When uploading the Gerber files to a PCB manufacturer such as [Seeed Studio Fusion](https://www.fusionpcb.jp/), please check the generated preview and manufacturing settings before ordering.

Check especially:

- board dimensions
- layer count
- board outline
- drill file detection
- board thickness
- copper weight
- quantity

---

## BOM

The component list is managed through DigiKey MyList.

- DigiKey MyList: [Power Monitor Board BOM](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U)

Passive components may be substituted if they match the required package size, rating, tolerance, and electrical characteristics.

IC substitutions are not guaranteed to work because behavior, electrical characteristics, and register compatibility may differ.

---

## Relationship to Firmware and Software

The Power Monitor Board is not a motor or servo control node.

Instead, it provides measurement data that can be collected by the host-side software.

```text
Power_Monitor_Board
      ↓
I²C measurement access
      ↓
SQLite logging
      ↓
Grafana dashboard
```

In the demo system, power data can be logged together with node status and actuator behavior.

This helps visualize the relationship between physical motion and electrical load.

---

## Example use in the demo

The board can be used to monitor:

- conveyor motor current
- servo operation current
- module-level power consumption
- idle vs active current difference
- abnormal current draw during wiring or mechanical issues

---

## Related Documents

- Hardware overview → [README.md](../README.md)
- Manufacturing index → [Manufacturing](../Manufacturing/)
- Software overview → [Software](../../Software/)
- Grafana / logging related documents → [Software](../../Software/)
