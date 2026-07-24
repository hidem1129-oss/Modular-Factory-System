# Servo_Board

---

<img width="2016" height="1512" alt="IMG_1264" src="https://github.com/user-attachments/assets/d3c5bc24-323a-4e86-a070-3e9131d77a80" />

---

This directory contains the hardware design files and documentation for the `Servo_Board`.

The board provides a single-channel power and PWM interface between the Pico-based Controller Board, an external +5 V supply, and a connected hobby servo motor.

---

## Purpose

The `Servo_Board` is used for small position-controlled mechanisms in the Modular Factory System.

Typical applications include:

- sorting gates
- diverter arms
- paper clamps
- lightweight linkage mechanisms
- other small angular actuators

The board itself does not calculate target angles or movement timing. Those behaviors are implemented by the servo-node firmware.

---

## Role in the System

```text
Raspberry Pi 5
      ↓ I²C commands
Controller Board
      ↓ GPIO2 PWM signal
Servo_Board
      ↓ PWM + 5 V + GND
Servo motor / gate / clamp
```

The host sends standardized register commands over I²C.

The Controller Board interprets those commands through firmware and generates the servo PWM signal routed by this board.

---

## Responsibility Boundary

### Servo Board hardware

- receives the PWM signal from the Controller Board
- routes the PWM signal through a series damping resistor
- receives an external +5 V servo supply
- distributes +5 V and ground to the servo connector
- provides local and bulk supply decoupling
- provides connector silkscreen guidance for the validated servo

### Servo-node firmware

- converts register values into PWM pulse widths
- defines target positions or angles
- manages movement timing
- applies lifecycle and latch rules
- handles E-STOP behavior
- reports state and feedback through the common register interface

### External power board

- supplies the +5 V servo rail
- provides the upstream power path and any system-level monitoring

---

## Controller Board Interface

`J1` is the common 2×10 connector used to connect the Servo Board to the Controller Board.

This board uses only one digital control position.

| J1 pin pair | Controller Board signal | Pico GPIO | Servo function |
|---|---|---:|---|
| 3–4 | `GPIO_BUS_0` | GPIO2 | PWM signal |

The odd-numbered pin is ground.

The remaining digital GPIO positions and `ADC_IN` are not used by this board.

---

## PWM Signal Path

The PWM signal path is:

```text
Controller Board GPIO2
        ↓
J1
        ↓
R1 47 Ω
        ↓
J2 servo signal pin
```

`R1` is a 47 Ω series damping resistor.

Its purpose is to:

- reduce ringing
- soften fast signal edges
- improve signal integrity over external wiring
- provide limited series isolation between the Pico GPIO and servo signal line

The resistor is not a level shifter or signal buffer.

---

## Servo Connector

`J2` is the three-pin servo connector.

| J2 pin | Function |
|---:|---|
| 1 | PWM signal |
| 2 | +5 V |
| 3 | GND |

The board routes both power and control to the connected servo.

The electrical pin assignment, not wire color alone, is the authoritative connection reference.

---

## Servo Power Input

`J3` is the external servo-power input.

| J3 pin | Function |
|---:|---|
| 1 | +5 V |
| 2 | GND |

The power path is:

```text
external power board
        ↓
J3
        ↓
Servo Board +5 V rail
        ↓
J2
        ↓
servo motor
```

The Servo Board does not generate or regulate the +5 V supply.

---

## Decoupling and Noise Control

Servo motors can create substantial supply noise and short current spikes during startup, rapid position changes, direction reversals, mechanical loading, and stall or near-stall conditions.

### Servo-side local decoupling

Near the servo connector:

- 47 µF electrolytic capacitor
- 0.1 µF capacitor

These components provide local energy storage and high-frequency bypassing close to the servo load.

### Additional bulk decoupling

The board also includes:

- 470 µF electrolytic capacitor
- 0.1 µF capacitor

The relatively large 470 µF capacitor was selected conservatively to absorb larger transient current demand and supply disturbance caused by servo operation.

All capacitors are connected to the same +5 V rail. The distinction is primarily their value and physical placement rather than electrically isolated supply sections.

These capacitors provide a minimum level of local stabilization. They do not replace validation of servo stall current, cable resistance, connector current capacity, upstream power-supply capability, grounding, and actual transient behavior under load.

---

## Validated Servo

The reference and validated servo for this board is:

- [Tower Pro SG90 micro servo](https://akizukidenshi.com/catalog/g/g108761/)

The current board layout, connector labeling, power assumptions, and practical testing are based on this servo.

Reference characteristics listed by the supplier include:

- model: SG90
- control signal: PWM
- nominal voltage: 4.8 V
- listed supply range: 3.3 V to 6 V
- rotation range: 180°
- listed maximum stall torque: 1.8 kgf·cm
- listed maximum speed: 0.1 s / 60°

### Connector silkscreen

The J2 silkscreen reflects the wire colors used by the validated SG90 servo.

| Board function | Silkscreen reference |
|---|---|
| PWM signal | `Y` |
| +5 V | `O` |
| GND | `Bro` |

These labels are intended as assembly guidance for the validated servo.

Other three-wire PWM servos may also be compatible when all of the following are verified:

- supply-voltage range
- control-signal requirements
- connector pin order
- current demand
- mechanical load
- travel range
- pulse-width requirements

Wire colors are not guaranteed to be standardized across manufacturers or product revisions.

Always verify the actual connector pinout before applying power.

---

## Current Status

Implemented in hardware:

- Controller Board 2×10 interface
- GPIO2 PWM routing
- 47 Ω PWM damping resistor
- three-pin servo connector
- external +5 V power input
- 47 µF local bulk capacitor
- 470 µF additional bulk capacitor
- two 0.1 µF bypass capacitors
- connector silkscreen for the validated SG90 servo
- PWM test point

Implemented jointly with firmware:

- target-position control
- PWM pulse-width generation
- timed servo movement
- register-based control
- lifecycle behavior
- E-STOP response

Not implemented on this board:

- servo-position feedback
- current sensing
- stall detection
- overload protection
- independent hardware shutdown
- voltage regulation
- signal level shifting
- support for multiple servo channels

---

## Current Constraints

- designed for a single small hobby servo
- validated primarily with the Tower Pro SG90
- uses an externally supplied +5 V rail
- does not regulate servo power
- no onboard current measurement
- no direct position feedback
- no stall detection
- no fuse or resettable protection
- no hardware-enforced safe-state independent of firmware
- actual voltage stability depends on the upstream supply, wiring, connector, servo load, and mechanical condition
- wire colors must not be treated as a universal pinout standard
- not qualified as an industrial servo-control or safety device

---

## Extension Points

Possible future improvements include:

- onboard current sensing
- resettable fuse or dedicated power protection
- hardware power disconnect
- separate servo-power switch
- configurable signal-level translation
- keyed connector
- connector polarity protection
- additional servo channels
- stall detection
- position feedback
- larger test points
- standardized power harness
- support for higher-current servos
- direct integration with a dedicated servo-power distribution board

---

## BOM

- DigiKey MyList: [Servo_Board](https://www.digikey.jp/ja/mylists/list/1G5XREGH36)
- Validated servo: [Tower Pro SG90](https://akizukidenshi.com/catalog/g/g108761/)

Before ordering, verify connector orientation, capacitor polarity, servo pin order, servo voltage, expected current demand, mechanical load, footprint, availability, and lifecycle status.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB production data | Gerber files in this directory |
| Controller Board pin mapping | Current schematic and Controller Board README |
| Servo PWM behavior | Servo-node firmware |
| Power routing | Current schematic |
| Connector pinout | Current schematic |
| Validated servo characteristics | Linked SG90 supplier page and product documentation |
| Part selection | DigiKey MyList and schematic |
| Timing and lifecycle behavior | Firmware common core and servo-node implementation |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- Power Monitor Board → [`../Power_Monitor_Board/`](../Power_Monitor_Board/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Servo-node firmware → [`../../Firmware/servo_node/`](../../Firmware/servo_node/)
- Common firmware → [`../../Firmware/common/`](../../Firmware/common/)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
