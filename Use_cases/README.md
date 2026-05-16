# Use Cases

This directory contains example use cases built with the Modular Factory System.

Each use case demonstrates how the hardware boards, firmware nodes, and software monitoring tools can be combined to create a small tabletop factory process.

---

## Purpose

The use cases show how the system can be reconfigured for different mechanisms and workflows.

They demonstrate:

- modular hardware reuse
- I²C-based distributed control
- firmware node reuse
- host-side orchestration
- monitoring and logging
- Grafana visualization
- tabletop-scale process prototyping

---

## Use Case Overview

| Use Case | Description | Main Technologies |
|---|---|---|
| [`Amazon-style Sorting Demo`](/Amazon-style_Sorting_Demo/) | Detects a workpiece, identifies its color, and sorts it with a servo gate | Camera, sensor, DC motor, servo, I²C, Grafana |
| Stamp Process Demo | Feeds paper and presses a stamp using motorized mechanisms | DC motor, servo clamp, rack-and-pinion, orchestration |

---

## Common System Flow

```text
Physical mechanism
      ↓
Hardware boards
      ↓
Firmware nodes
      ↓
Raspberry Pi 5 orchestration / monitoring
      ↓
SQLite logs
      ↓
Grafana visualization
```

---

## Documentation Format

Each use case should include:

- overview
- hardware used
- firmware used
- software used
- operation flow
- photos / GIFs / video links
- observed results
- notes and future improvements

---

## Related Documents

- Hardware overview → [`../Hardware/`](../Hardware/)
- Firmware overview → [`../Firmware/`](../Firmware/)
- Software overview → [`../Software/`](../Software/)
