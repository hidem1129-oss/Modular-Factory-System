# System Architecture

This directory contains system architecture diagrams for the Modular Factory System.

---

## Overall Architecture

![System Architecture](./system_architecture.png)

This diagram shows the overall relationship between the host controller, distributed control nodes, actuator / sensor nodes, power monitoring, logging, and visualization.

---

## Main Components

| Component | Role |
|---|---|
| Raspberry Pi 5 | Host controller, orchestration, monitoring, and logging |
| Control Node | Pico-based I²C node that receives commands and controls local hardware |
| Actuator or Sensor Node | Physical device layer such as motors, servos, and sensors |
| Power Monitor Board | Distributes power and monitors voltage / current |
| SQLite DB | Stores monitoring logs and snapshots |
| Grafana | Visualizes logs and power monitor data |

---

## Data and Control Flow

The Raspberry Pi 5 sends commands to distributed control nodes through the I²C-based control architecture.

Control nodes operate local actuator or sensor hardware.

The Power Monitor Board distributes power to the hardware modules and provides measurement data for monitoring.

The host-side software stores monitoring results in SQLite, and Grafana visualizes the recorded data.

---

## Related Documents

- Project overview → [`../../README.md`](../../README.md)
- Hardware overview → [`../../Hardware/`](../../Hardware/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Software overview → [`../../Software/`](../../Software/)
- Use cases → [`../../Use_cases/`](../../Use_cases/)
