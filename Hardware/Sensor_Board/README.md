# Sensor_Board

---

<img width="2016" height="1512" alt="IMG_1267" src="https://github.com/user-attachments/assets/2aef0bc9-d3f5-4896-b9c2-23c34375496c" />

---

This directory contains hardware design files for the Sensor_Board.

The Sensor_Board provides the hardware interface for sensor-based detection mechanisms in the Modular Factory System.

---

## Purpose

The `Sensor_Board` provides the wiring and signal interface for sensors used in the Modular Factory System.

It is designed for sensors such as photo-reflectors that detect workpieces, timing points, or physical changes in tabletop factory mechanisms.

---

## Role in the system

This board receives sensor-side signals and routes them to a Pico-based sensor control node.

```text
Physical sensor
      ↓ sensor signal
Sensor_Board
      ↓ local input signal
Controller / sensor control node
      ↓ I²C feedback values
Raspberry Pi 5
```

The local controller reads the sensor input and exposes the measured value through the I²C register interface.

The host-side software can use this value for detection, branching, logging, or visualization.

---

## Main Functions

- connects physical sensors to the local control node
- routes sensor signals to the Pico-based firmware node
- supports workpiece detection in tabletop demo mechanisms
- provides a reusable sensor input interface for different mechanisms
- helps separate host-side orchestration from low-level sensor reading
- supports sensor-based triggering, branching, and logging

---

## Main Components

| Component | Purpose |
|---|---|
| [Photo-reflector](https://akizukidenshi.com/catalog/g/g104500/) | Detect objects |
| Control signal connector | Routes sensor signals to the controller node |
| Pull-up / support components | Signal stabilization and basic support circuitry |
| Passive components | Decoupling, filtering, and electrical support |

The photo-reflector in the link is not available at Digikey.

The parts in the BOM have the same pin assignment and similar electrical characteristics, but their operation has not been confirmed.

---

## BOM
- DigiKey MyList: [Sensor_Board](https://www.digikey.jp/ja/mylists/list/EW3FT9N8K2)

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Firmware sensor node → [`../../Firmware/sensor_node/`](../../Firmware/sensor_node/)
