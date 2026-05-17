# Docs

This directory contains supplementary documentation for the Modular Factory System.

The main project structure is organized into:

- [`Hardware`](../Hardware/) : PCB design, manufacturing data, and BOM information
- [`Firmware`](../Firmware/) : Raspberry Pi Pico firmware for distributed I²C nodes
- [`Software`](../Software/) : monitoring, logging, and Grafana visualization
- [`Use_cases`](../Use_cases/) : example tabletop factory processes

---

## Purpose

The `Docs` directory is used for additional design notes and reference materials that do not belong to a single hardware board, firmware node, or software tool.

It includes:

- system architecture notes
- register map references
- orchestration design notes
- troubleshooting notes
- development history or design decisions
- diagrams and explanatory materials

---

## Suggested Topics

| Topic | Description |
|---|---|
| [`System_Architecture/`](./System_Architecture/) | System architecture diagrams and data/control flow overview |
| [`Register_Map`](./Register_Map/) | Common I²C register definitions and command format |

---

## Related Documents

- Hardware overview → [`../Hardware/`](../Hardware/)
- Firmware overview → [`../Firmware/`](../Firmware/)
- Software overview → [`../Software/`](../Software/)
- Use cases → [`../Use_cases/`](../Use_cases/)
