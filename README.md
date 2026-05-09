
# Modular Factory System

> A reconfigurable DX (Digital Transformation) platform built with distributed I²C nodes, real hardware, and data-driven orchestration.

---

## Demo Video

### Contest Version

This video was created for a prototype contest and gives a compact overview of the system.

- YouTube: https://youtu.be/8Od8Ocg0tjE

The video demonstrates:
- modular tabletop factory hardware
- I²C-based motor / servo / sensor control
- camera-based color sorting
- SQLite logging
- Grafana visualization
- custom PCB-based control and monitoring modules

---
## Choose your path

- 🛠 Engineers (run the system) → [Quick Start](./Software/servo_node/quick_start/README.md)
- 🧑‍💼 Recruiters (DX overview) → [Japanese Overview](./日本語版/README.md)

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

## Core System

The core of this project is not a specific actuator, but a **modular node architecture**:

- Distributed I²C node system
- Register-based communication protocol
- State machine-driven execution model
- Hardware-agnostic module design

This allows different modules to be plugged into the same system:

- Servo Node (actuator example)
- Photo Sensor Node (measurement example)
- Motor Node (drive example)

This architecture abstracts physical devices into standardized nodes,
similar to microservices in software systems.

---

## Quick Start

Start with a simple actuator example: 
[Servo Node Quick Start](./Software/servo_node/quick_start/README.md)

Example:

```bash
i2cset -y 1 0x14 0x30 0x10
```

## Architecture
```text
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
- Hardware modules behave as interchangeable nodes
- Scalable system design
👉 Detailed design: ./Software/servo_node/design/README.md


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

## Hardware and BOM ~About manufacturing~

Custom PCBs are used for control and monitoring modules.  
Each board has a DigiKey MyList-based BOM.

- [Power Monitor Board BOM](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U)
- [DC Motor Control Board BOM](https://www.digikey.jp/ja/mylists/list/UO08XWYEAF)
- [Servo Control Board BOM](https://www.digikey.jp/ja/mylists/list/1G5XREGH36)
- [Sensor Board BOM](https://www.digikey.jp/ja/mylists/list/EW3FT9N8K2)
- [Controller Board BOM](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78)
- [Pi 5 Board BOM](https://www.digikey.jp/ja/mylists/list/QO09LGCG39)
- [Other Components BOM](https://www.digikey.jp/ja/mylists/list/5G1M6HVIDC)

See: [Hardware BOM](./Hardware/README.md)

## Current Status

- Multi-node I²C control: implemented
- Camera-based sorting demo: implemented
- SQLite logging: implemented
- Grafana dashboard: implemented
- Custom PCB modules: implemented
- BOM management via DigiKey MyLists: prepared

## Future Work

- Flow-editor style orchestration UI
- More reusable module slots and standardized harnesses
- Additional use cases using the same control modules
- Recruiter-oriented 5-minute technical walkthrough video

## Disclaimer
This project is intended for educational and PoC purposes.
