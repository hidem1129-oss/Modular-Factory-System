# Stamp Process Demo

This use case demonstrates a tabletop press-style process that feeds paper and presses a stamp using motorized mechanisms.

It is inspired by a simple press-processing line, where a material is fed into position, clamped, pressed, and then moved forward.

---

## Overview

The Stamp Process Demo is a small tabletop process line built with the Modular Factory System.

A paper roll is fed through the mechanism by a motor-driven take-up roller.  
A clamp mechanism holds the paper in place, and a rack-and-pinion mechanism presses a stamp onto the paper.

Although this demo uses a stamp instead of an industrial press tool, the operation flow is similar to a simplified press-processing line:
```text
Feed material
      ↓
Clamp / hold position
      ↓
Press
      ↓
Release
      ↓
Feed next position
```

---

## Purpose

This use case demonstrates:

- reuse of the same modular hardware for a different process
- DC motor-based roll paper feed
- motorized press motion using a rack-and-pinion mechanism
- servo-based clamping or holding mechanism
- I²C-based distributed control
- host-side orchestration of multiple actuators
- monitoring and logging through the I2C Debugger
- visualization through Grafana

---

## Hardware Used

| Hardware | Role |
|---|---|
| Controller_Board | Local I²C control node |
| DC_Motor_Board | Motor control for feed and press mechanisms |
| Servo_Board | Roll paper clamp or hold-down mechanism |
| Power_Monitor_Board | Voltage / current monitoring |
| Pi5_Wiring_Auxiliary | Raspberry Pi 5 wiring support |
| Raspberry Pi 5 | Host controller and monitoring system |
| Rack-and-pinion mechanism | Converts motor rotation into linear press motion |
| Stamp | Physical marking tool |
| Roll paper | Work material |

---

## Firmware Used

| Firmware | Role |
|---|---|
| `Firmware/motor_node` | Controls feed and press motors |
| `Firmware/servo_node` | Controls the clamp mechanism |

---

## Software Used

| Software | Role |
|---|---|
| `Software/I2C_Debugger` | Monitors node states and power values |
| `Software/Grafana` | Visualizes logs and power monitor data |
| [`Orchestration script`](./stamp_press_demo.py) | Coordinates feed, clamp, press, release, and repeat operations |

---

## Operation Flow

```text
Start paper feed
      ↓
Move paper to target position
      ↓
Clamp paper with servo mechanism
      ↓
Drive rack-and-pinion press mechanism
      ↓
Press stamp onto paper
      ↓
Return press mechanism
      ↓
Release clamp
      ↓
Feed paper to next position
      ↓
Log node states and power values
      ↓
Visualize results in Grafana
```

---

## Monitoring and Visualization

During the demo, the system can be monitored using the I2C Debugger.

The monitor records:

- motor node state transitions
- servo node state transitions
- actuator timing
- power monitor values
- monitoring sessions

Grafana can visualize the recorded SQLite logs as timelines, tables, and power graphs.

This makes it possible to compare actuator operation with electrical load during the press-style process.

---

## Result

This use case confirms that the Modular Factory System can be reconfigured from a sorting process into a press-style process.

It demonstrates that the same control, monitoring, logging, and visualization layers can be reused even when the physical mechanism is changed.

---

## Notes and Future Improvements

The following are potential areas for future improvement:

- Improved accuracy of paper feed position
- Improved stamp alignment accuracy
- Reproducibility testing across multiple cycles
- Addition of paper position detection sensors
- Addition of force or current-based abnormal load detection methods
- Improved stamp accuracy through limit switch control

---

## Related Documents

- Use cases overview → [`../README.md`](../README.md)
- Hardware overview → [`../../Hardware/`](../../Hardware/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Software overview → [`../../Software/`](../../Software/)
- DC Motor Board → [`../../Hardware/DC_Motor_Board/`](../../Hardware/DC_Motor_Board/)
- Servo Board → [`../../Hardware/Servo_Board/`](../../Hardware/Servo_Board/)
- Power Monitor Board → [`../../Hardware/Power_Monitor_Board/`](../../Hardware/Power_Monitor_Board/)
- I2C Debugger → [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/)
- Grafana → [`../../Software/Grafana/`](../../Software/Grafana/)
