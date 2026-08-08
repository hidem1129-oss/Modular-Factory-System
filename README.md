# Modular Factory System

> A reconfigurable tabletop factory platform built with distributed I²C nodes, custom PCBs, real actuators, logging, and Grafana visualization.

<img width="1920" height="1080" alt="overall view" src="https://github.com/user-attachments/assets/3304786b-fce8-4f30-938c-a0456ee5e704" />



*Overall view of the modular tabletop factory system, including conveyor, camera, and distributed control hardware.*



---

## Demo Video

<img width="248" height="140" alt="Adobe Express - gif" src="https://github.com/user-attachments/assets/b950c0a5-6086-4731-b46b-9f26fcd5e588" />

*Animated preview of the conveyor-based demo running on the modular factory platform.*


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

## What is this?

This project is a modular tabletop factory system designed to prototype small factory processes and DX-style monitoring workflows.

It combines:

- Raspberry Pi 5 as the host platform
- Raspberry Pi Pico-based distributed I²C nodes
- custom PCBs for control, sensing, wiring, and power monitoring
- real actuators and sensors
- SQLite logging
- Grafana visualization

The goal is to show how physical mechanisms, embedded control, host-side software, and data visualization can be integrated into one reusable system.

---

## Repository Map

| Directory | Description |
|---|---|
| [`Hardware/`](./Hardware/) | PCB design files, Gerber data, schematics, manufacturing notes, and BOM links |
| [`Firmware/`](./Firmware/) | Raspberry Pi Pico firmware for motor, servo, and sensor I²C nodes |
| [`Software/`](./Software/) | I²C monitoring software, SQLite logging, and Grafana visualization notes |
| [`Use_cases/`](./Use_cases/) | Example tabletop factory processes built with the system |
| [`Docs/`](./Docs/) | Supplementary architecture notes and design references |
| [`日本語版/`](./日本語版/) | Japanese overview for recruiters and readers in Japan |
| [`ADL/`](./ADL/) | Architecture decisions, design constraints, principles, alternatives, and implementation consequences |

---

## System Architecture

```text
                      Raspberry Pi 5
                            │
             ┌──────────────┴──────────────┐
             │                             │
      Host orchestration              Monitoring
             │                             │
             ↓                             ↓
     I²C register interface          SQLite logging
             │                             │
             ↓                             ↓
       Pico device nodes                 Grafana
             │
             ↓
      Physical mechanisms
```

The system is designed around a distributed I²C node architecture.

Each physical module exposes status, command, and feedback values through a register-based interface.

Host-side orchestration coordinates the physical process through the common register interface.

Separate monitoring software observes exposed node states and feedback, persists selected records to SQLite, and Grafana visualizes the stored historical data.

See also:
- [`Docs/System_Architecture/`](./Docs/System_Architecture/)

---

## Use Cases

| Use Case | Description |
|---|---|
| [`Amazon-style Sorting Demo`](./Use_cases/Amazon-style_Sorting_Demo/) | Detects a workpiece, identifies its color, and sorts it with a servo gate |
| [`Stamp Process Demo`](./Use_cases/Stamp_Process_Demo/) | Feeds paper and presses a stamp using motorized mechanisms |

These use cases show that the same hardware, firmware, and software layers can be reused for different tabletop factory processes.

---

## Technical Highlights

- Distributed I²C node architecture
- Register-based control and feedback interface
- Raspberry Pi Pico-based local control nodes
- PyQt5-based I²C debugger and power monitor
- SQLite-based event and snapshot logging
- Grafana dashboards for state timelines and power monitoring
- Custom PCBs with Gerber data and DigiKey MyList-based BOMs
- Reconfigurable tabletop mechanisms using DC motors, servos, sensors, and camera detection
- Architecture Decision Log covering requirements, constraints, alternatives, responsibility boundaries, reproducibility, and deferred scope

Protocol reference:

- [`Docs/Register_Map/`](./Docs/Register_Map/)

---

## Hardware and Manufacturing

Custom PCBs are used for control, sensing, wiring, and power monitoring.

Hardware documentation includes:

- board role descriptions
- photos
- schematics
- Gerber files
- DigiKey MyList BOM links
- manufacturing notes

See:

- [`Hardware/`](./Hardware/)
- [`Hardware/Manufacturing/`](./Hardware/Manufacturing/)

---

## Software and Visualization

The software layer includes:

- I²C node monitoring
- power monitoring
- state transition logging
- SQLite database generation
- Grafana visualization notes and SQL examples

See:

- [`Software/I2C_Debugger/`](./Software/I2C_Debugger/)
- [`Software/Grafana/`](./Software/Grafana/)

---

## Current Status

- Multi-node I²C control: implemented
- Camera-based sorting demo: implemented
- Stamp process demo: implemented
- SQLite logging: implemented
- Grafana dashboard: implemented
- Power monitoring dashboard: implemented
- Custom PCB modules: implemented
- Gerber data and schematic files: published
- BOM management via DigiKey MyLists: prepared

---

## Future Work

- Flow-editor style orchestration UI
- Re-evaluation of the control-module physical interconnection and harness strategy  
  - See [`Control Module Interconnection and Operational Feedback`](./Docs/Design_Reviews/Control_Module_Interconnection_and_Operational_Feedback.md)
- Additional use cases using the same control modules
- Recruiter-oriented 5-minute technical walkthrough video
- More detailed troubleshooting and setup documentation

---

## Disclaimer

This project is intended for educational, prototyping, and proof-of-concept purposes.
