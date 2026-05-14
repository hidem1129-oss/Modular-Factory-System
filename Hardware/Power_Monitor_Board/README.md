# Power_Monitor_Board

---

<img width="2016" height="1512" alt="IMG_1262" src="https://github.com/user-attachments/assets/d9be0243-1ba5-4366-858a-190fcb7c2a07" />

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

## BOM

The component list is managed through DigiKey MyList.

- DigiKey MyList: [Power Monitor Board](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Software overview → [`../../Software/`](../../Software/)
- Grafana / logging related documents → [`../../Software/`](../../Software/)
