# Hardware

This directory contains hardware design files and documentation for the Modular Factory System.

The hardware is organized as a set of modular PCBs that connect physical actuators, sensors, power lines, and the central controller through a shared I²C-based architecture.

---

## Purpose

The hardware layer provides the physical foundation of the Modular Factory System.

It is designed to support:

- distributed I²C-connected control modules
- swappable actuator and sensor boards
- centralized power distribution
- current / voltage monitoring
- repeatable tabletop factory demonstrations
- easier wiring and maintenance through standardized connectors

---

## Board Overview

Each PCB has a dedicated role in the system.

| Board | Role | Main Function |
|---|---|---|
| [Controller_Board](Controller_Board/) | System interface | Connects Raspberry Pi 5 to I²C modules |
| [DC_Motor_Board](DC_Motor_Board/) | Actuator control | Drives DC motors such as conveyors and linear mechanisms |
| [Servo_Board](Servo_Board/) | Actuator control | Drives servo motors for sorting gates and clamp mechanisms |
| [Sensor_Board](Sensor_Board/) | Sensor input | Connects photo-reflectors and other detection sensors |
| [Power_Monitor_Board](Power_Monitor_Board/) | Power distribution / monitoring | Distributes 5 V power and monitors voltage/current |
| [Pi5_Wiring_Auxiliary](Pi5_Wiring_Auxiliary/) | Wiring support | Helps organize harnessing and module connections |

---

## Common Design Requirements

The PCBs are designed around the following common requirements.

### I²C-based modular connection

Each control module is connected through the I²C bus.

Common design points:

- Raspberry Pi 5 acts as the central controller
- Raspberry Pi Pico-based nodes act as distributed I²C devices
- modules expose control / feedback through register-based communication
- board-to-board wiring is designed to be replaceable and maintainable

### Connector and cable policy

The system uses standardized connectors and cables where possible.

Common goals:

- reduce wiring mistakes
- make module replacement easier
- keep I²C and power lines physically organized
- support repeated tabletop demonstrations

Typical connection elements include:

- ribbon cables for module wiring
- board-mounted connectors for I²C and control lines
- screw terminals or equivalent connectors for power input/output
- keyed or pitch-consistent connectors where possible

### Power distribution

The hardware layer includes a dedicated power distribution and monitoring concept.

Common goals:

- distribute 5 V power to multiple branches
- monitor voltage and current per branch
- separate logic/control wiring from higher-current paths where practical
- expose power data to the software layer for logging and visualization

### Mechanical and demo constraints

The hardware is designed for a tabletop demonstration environment.

Design considerations include:

- easy access to connectors
- visible wiring for explanation
- enough robustness for repeated setup and teardown
- compatibility with LEGO / Tamiya / small actuator-based mechanisms
- clear relation between PCB function and physical demo behavior

---

## Directory Structure

Each board directory contains design data and documentation for one PCB or hardware module in the following format:

```text
Hardware/
├─ README.md
├─ XXXXX_board/
│  ├─ README.md
│  ├─ schematic/
│  └─ gerber/

```

---

## Relationship to Firmware and Software

The hardware layer works together with the firmware and software layers.

```text
Hardware PCB
    ↓
Firmware node
    ↓
I²C register interface
    ↓
Host-side orchestration / logging
    ↓
Grafana visualization
```

For example:
- a DC motor board is driven by [`Firmware/motor_node`](../Firmware/motor_node)
- a servo board is driven by [`Firmware/servo_node`](../Firmware/servo_node)
- a sensor board is read by [`Firmware/sensor_node`](../Firmware/sensor_node)
- power monitor data is logged and visualized by the software layer

---

## Related Documents

- Firmware overview → [`Firmware`](../Firmware/)
- Software overview → [`Software`](../Software/)
- Use cases → [`Use_cases`](../Use_cases/)
- Japanese overview → [`日本語版`](../日本語版/)
- Manufacturing → [`Manufacturing`](./Manufacturing/)
