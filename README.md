
# Modular Factory System

> A reconfigurable DX (Digital Transformation) platform built with distributed I²C nodes, real hardware, and data-driven orchestration.

---

## Demo

See the system in action  
- Demo Video: ./Media/demo.mp4  
- YouTube: https://youtube.com/...

---

## What is this?

This project is a **modular factory system** designed to simulate real-world industrial DX scenarios using:

- Raspberry Pi 5 (orchestration / AI / visualization)
- Raspberry Pi Pico (distributed control nodes)
- I²C-based communication
- Real actuators and sensors

It enables:

- Rapid prototyping of factory systems
- Reconfiguration of physical workflows
- Integration of control, sensing, and data analytics

---

## Quick Start

Start with the actuator node:  
[Servo Node Quick Start](./Software/servo_node/quick_start/README.md)

Example:

```bash
i2cset -y 1 0x14 0x30 0x10
```

## Architecture
```Markdwon
[ Raspberry Pi 5 ]
        │ I²C
 ┌──────┴─────────┐────────────────┐────────────...
 │                │                │
Servo Node   Sensor Node       Motor Node       ...
 │                │                │
Actuator      Measurement       Actuator        ...
```
- Distributed node architecture
- Register-based communication
- Scalable system design
Details: ./Software/servo_node/design/README.md

## Technical Highlights
- Register map (0x00–0x3F)
- DATA_READY / UPDATE_CNT synchronization
- Command validation (accept / reject model)
- E-STOP propagation across nodes
- Modular firmware architecture

## Why this matters

Modern DX requires:

- Integration of hardware and software
- Real-time observability
- Flexible and modular systems

This project demonstrates:
- Plug-and-play hardware architecture
- Data-driven control loops
- Physical system orchestration

## Future Work
- Multi-node orchestration
- Digital twin / debugger interface
- Grafana / Power BI integration
- AI-based sorting system

## Disclaimer
This project is intended for educational and PoC purposes.
