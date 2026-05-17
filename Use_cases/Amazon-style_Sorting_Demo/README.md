# Amazon-style Sorting Demo

This use case demonstrates a tabletop sorting line that detects a workpiece, identifies its color, and sorts it using a servo gate.

---

## Overview

The Amazon-style Sorting Demo is a small tabletop logistics line built with the Modular Factory System.

A workpiece is moved by a DC motor-driven conveyor.  
A sensor detects the workpiece, a camera captures it, and the host-side software determines the sorting destination.  
A servo-driven gate then diverts the workpiece according to the detection result.

---

## Purpose

This use case demonstrates:

- modular reuse of motor, servo, and sensor hardware
- I²C-based distributed control
- sensor-triggered operation
- camera-based detection
- actuator coordination
- monitoring and logging through the I2C Debugger
- visualization through Grafana

---

## Hardware Used

| Hardware | Role |
|---|---|
| Controller_Board | Local I²C control node |
| DC_Motor_Board | Conveyor motor control |
| Servo_Board | Sorting gate control |
| Sensor_Board | Workpiece detection |
| Power_Monitor_Board | Voltage / current monitoring |
| Pi5_Wiring_Auxiliary | Raspberry Pi 5 wiring support |
| Raspberry Pi 5 | Host controller and monitoring system |
| Camera | Workpiece color detection |

---

## Firmware Used

| Firmware | Role |
|---|---|
| `Firmware/motor_node` | Controls the conveyor motor |
| `Firmware/servo_node` | Controls the sorting gate servo |
| `Firmware/sensor_node` | Reads workpiece detection sensor values |

---

## Software Used

| Software | Role |
|---|---|
| `Software/I2C_Debugger` | Monitors node states and power values |
| `Software/Grafana` | Visualizes logs and power monitor data |
| [`Orchestration script`](./warehouse_demo.py) | Coordinates conveyor, detection, and servo operation. Including camera operation too. |

---

## Operation Flow

```text
Start conveyor
      ↓
Detect workpiece with photo-reflector
      ↓
Capture image with camera
      ↓
Classify workpiece color
      ↓
Move servo gate
      ↓
Sort workpiece
      ↓
Log node states and power values
      ↓
Visualize results in Grafana
```

---

## Monitoring and Visualization

During the demo, the system can be monitored using the I2C Debugger.

The monitor records:

- node state transitions
- sensor values
- motor and servo activity
- power monitor values
- monitoring sessions

Grafana can visualize the recorded SQLite logs as timelines, tables, and power graphs.

---

## Result

This use case confirms that multiple hardware modules can be combined into a single tabletop process.

It demonstrates that sensing, image recognition, actuator control, logging, and visualization can be integrated in one modular system.

---

## Related Documents

- Use cases overview → [`../README.md`](../README.md)
- Hardware overview → [`../../Hardware/`](../../Hardware/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Software overview → [`../../Software/`](../../Software/)
- I2C Debugger → [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/)
- Grafana → [`../../Software/Grafana/`](../../Software/Grafana/)
