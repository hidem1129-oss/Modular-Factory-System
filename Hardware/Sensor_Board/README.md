# Sensor_Board

---

<img width="2016" height="1512" alt="IMG_1267" src="https://github.com/user-attachments/assets/2aef0bc9-d3f5-4896-b9c2-23c34375496c" />

---

This directory contains the hardware design files and documentation for the `Sensor_Board`.

The board provides a single-channel photo-reflector interface between a reflective optical sensor and the Pico-based Controller Board.

It continuously drives the photo-reflector LED and routes the phototransistor output to Pico GPIO26 / ADC0.

---

## Purpose

The `Sensor_Board` is used for simple reflective detection in tabletop factory mechanisms.

Typical uses include:

- workpiece detection
- timing-point detection
- presence / absence sensing
- simple reflective surface detection
- sensor-based triggering in orchestration sequences

The board itself does not define detection thresholds, branching, logging, or visualization. Those behaviors are implemented by firmware and host-side software.

---

## Role in the System

```text
object / reflective surface
        ↓ reflected light
photo-reflector
        ↓ analog voltage
Sensor_Board
        ↓ ADC_IN
Controller Board / Pico ADC0
        ↓ I²C feedback
Raspberry Pi 5
```

The sensor-node firmware reads the ADC value and exposes it through the common register interface.

The host can then use the value for detection, branching, logging, visualization, and orchestration logic.

---

## Responsibility Boundary

### Sensor Board hardware

- supplies 3.3 V to the photo-reflector LED
- limits LED current with a series resistor
- converts phototransistor current into an ADC voltage
- provides a pull-up on the analog output
- optionally provides a simple RC low-pass filter
- provides local 3.3 V decoupling
- routes the analog signal to the Controller Board

### Sensor-node firmware

- reads Pico GPIO26 / ADC0
- publishes the raw ADC value
- applies any scaling, inversion, or normalization
- evaluates thresholds, if configured
- exposes detection state through registers
- handles lifecycle and E-STOP-related behavior

### Host-side software

- uses the reported value for branching
- logs sensor values
- displays sensor state
- coordinates sensor-driven sequences

---

## Controller Board Interface

`J1` is the common 2×10 connector used to connect the Sensor Board to the Controller Board.

This board uses:

- 3.3 V
- ground
- `ADC_IN`

The analog output is routed through the Controller Board to:

```text
Pico GPIO26 / ADC0
```

The remaining digital GPIO positions are not required by this hardware revision.

---

## Photo-Reflector Circuit

The current circuit uses a continuously illuminated photo-reflector.

### Emitter side

```text
3.3 V
  ↓
R1 330 Ω
  ↓
photo-reflector LED
  ↓
GND
```

`R1` limits current through the internal LED.

The emitter is not switched by firmware in the current hardware configuration.

### Receiver side

```text
3.3 V
  ↓
R2 4.7 kΩ
  ↓
ADC_IN ─── Pico GPIO26 / ADC0
  ↓
photo-transistor
  ↓
GND
```

`R2` pulls the analog output toward 3.3 V when the phototransistor is weakly conducting.

As reflected light increases, the phototransistor pulls `ADC_IN` toward ground.

Therefore:

```text
weaker reflection → higher ADC value
stronger reflection → lower ADC value
```

Object detection is therefore associated with a lower raw ADC value, not a higher one.

---

## Optional Analog Filter

`C1` is an optional 0.1 µF capacitor from `ADC_IN` to ground.

When populated, it forms a simple RC low-pass filter with the 4.7 kΩ pull-up resistor.

This may help reduce high-frequency noise, short spikes, and small ADC fluctuations. The trade-off is a slower analog response.

`C1` should be treated as an optional population choice rather than a mandatory part of the current configuration.

---

## Power Decoupling

The 3.3 V rail includes:

- 0.1 µF ceramic capacitor
- 47 µF electrolytic capacitor

The ceramic capacitor provides high-frequency bypassing.

The electrolytic capacitor provides local bulk decoupling and helps stabilize the sensor supply.

These components do not replace validation of ambient-light effects, cable noise, grounding, ADC reference stability, installation geometry, or actual sensor behavior in the mechanism.

---

## Validated Photo-Reflector

The currently validated photo-reflector is:

- [Photo-reflector used in the current prototype](https://akizukidenshi.com/catalog/g/g104500/)

The present circuit, footprint, pin assignment, resistor values, and practical measurements are based on this device.

The linked sensor is not sourced from DigiKey.

Parts listed in the DigiKey BOM may have the same pin assignment and similar electrical characteristics, but their operation has not yet been confirmed in this board.

Alternative sensors should be verified for:

- pin assignment
- LED forward-voltage and current requirements
- phototransistor polarity
- collector current
- mechanical dimensions
- optical geometry
- response under the intended target and ambient light

---

## Observed Sensor Behavior

Practical measurements with the validated photo-reflector showed the following approximate raw ADC values.

| Test condition | Observed ADC value |
|---|---:|
| No nearby object | close to 4095 |
| Object approximately 20 mm from the receiver | approximately 2800–3200 |
| Mirror placed close to the sensor | close to 0 |

These observations are consistent with the circuit polarity:

- little reflected light leaves the output near the 3.3 V pull-up level
- stronger reflected light causes the phototransistor to pull the output toward ground

The measured values are examples rather than guaranteed thresholds.

Actual readings depend on object color, surface reflectivity, distance, alignment, ambient light, sensor variation, supply voltage, and mechanical installation.

Any detection threshold should therefore be calibrated for the actual target and installation.

---

## Practical Tip

In practical tests, adding a simple black vinyl-tape shroud around the photo-reflector increased the usable detection distance.

This is not a characterized optical design, but it may be useful when ambient or off-axis light reduces detection contrast.

The likely effect is that the shroud narrows the light entering the receiver and reduces unrelated light from the surrounding environment.

Performance still depends on the target, alignment, ambient light, and shroud geometry.

---

## Current Status

Implemented in hardware:

- Controller Board 2×10 interface
- 3.3 V photo-reflector supply
- 330 Ω LED current-limiting resistor
- phototransistor analog output
- 4.7 kΩ analog pull-up
- routing to GPIO26 / ADC0
- optional 0.1 µF analog filter footprint
- 0.1 µF supply bypass capacitor
- 47 µF bulk capacitor

Implemented jointly with firmware:

- ADC sampling
- raw-value reporting
- register-based sensor feedback
- threshold-based detection, when configured
- lifecycle behavior
- sensor use in host-side branching and logging

Not implemented on this board:

- automatic calibration
- ambient-light cancellation
- differential optical measurement
- digital comparator output
- adjustable hardware threshold
- emitter modulation
- multiple sensor channels
- electrical isolation

---

## Current Constraints

- designed for one reflective photo-reflector
- validated primarily with the linked Akizuki sensor
- stronger reflection produces a lower ADC value
- detection distance depends strongly on target reflectivity and geometry
- ambient and off-axis light can affect readings
- no automatic threshold calibration
- no emitter modulation
- no hardware comparator
- optional analog filtering slows response
- DigiKey-listed alternatives are not yet validated
- not qualified as an industrial presence or safety sensor

---

## Extension Points

Possible future improvements include:

- emitter on / off control
- synchronous or modulated optical measurement
- ambient-light subtraction
- adjustable threshold
- comparator output
- automatic calibration
- multiple sensor channels
- improved optical shroud
- dedicated mechanical sensor housing
- connector keying
- additional test points
- better analog filtering
- current-source LED drive
- validated DigiKey-compatible replacement sensor
- digital sensor interface for longer cable runs

---

## BOM

- DigiKey MyList: [Sensor_Board](https://www.digikey.jp/ja/mylists/list/EW3FT9N8K2)
- Validated photo-reflector: [Akizuki photo-reflector](https://akizukidenshi.com/catalog/g/g104500/)

Before ordering, verify sensor pin assignment, footprint, LED polarity, phototransistor polarity, resistor values, capacitor population, package, availability, and lifecycle status.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB production data | Gerber files in this directory |
| Controller Board ADC mapping | Current schematic and Controller Board README |
| ADC sampling behavior | Sensor-node firmware |
| Sensor polarity | Current schematic and observed measurements |
| Observed ADC values | This README |
| Validated sensor | Linked Akizuki product and current hardware |
| Alternative parts | DigiKey MyList and future validation results |
| Threshold and lifecycle behavior | Firmware common core and sensor-node implementation |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Sensor-node firmware → [`../../Firmware/sensor_node/`](../../Firmware/sensor_node/)
- Common firmware → [`../../Firmware/common/`](../../Firmware/common/)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
