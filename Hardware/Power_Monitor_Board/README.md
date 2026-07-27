# Power_Monitor_Board

---

<img width="2016" height="1512" alt="IMG_1262" src="https://github.com/user-attachments/assets/d9be0243-1ba5-4366-858a-190fcb7c2a07" />

---

This directory contains the hardware design files and documentation for the `Power_Monitor_Board`.

The board is the physical point where external 5 V power enters the Modular Factory System, is distributed to monitored branches, and is forwarded to actuator and controller-side hardware.

It also provides the I²C measurement path for nine INA219 monitors through three TCA9548A multiplexers.

---

## Purpose

The `Power_Monitor_Board` provides:

- centralized 5 V power distribution
- eight monitored output branches
- one monitored main line
- branch enable switches
- branch power indicators
- branch-level noise mitigation
- I²C access to current and voltage measurements
- a physical pass-through path from the Pi5 Wiring Auxiliary to the Controller Board chain

The board is intended primarily for repeatable trend monitoring, logging, comparison, and diagnostics.

It is not a precision load-terminal power meter, voltage regulator, or safety-rated power-distribution unit.

---

## Physical System Topology

The current wiring topology fixes the physical connection order.

### Logic and communication path

```text
Raspberry Pi 5
      ↓
Pi5 Wiring Auxiliary
      ↓ J2
Power Monitor Board
      ↓ J1
Controller Board chain
```

### Actuator power path

```text
external 5 V source
      ↓ J11
Power Monitor Board
      ↓ monitored branch outputs
Actuator boards
```

The current architecture therefore assumes that actuator power passes through the Power Monitor Board.

The board acts as:

- power-entry point
- branch-distribution hub
- measurement hub
- Wiring Auxiliary-to-Controller pass-through

---

## Responsibility Boundary

### Power Monitor Board hardware

- receives external 5 V power
- measures the main line
- distributes power to eight branches
- measures each branch
- provides branch switches
- provides branch power LEDs
- provides resettable-fuse protection
- provides branch ferrite-bead and capacitor filtering
- routes I²C to three TCA9548A multiplexers
- routes Wiring Auxiliary signals toward the Controller Board chain

### Software

- selects TCA9548A channels
- reads INA219 registers
- applies calibration values
- converts raw readings into voltage, current, and power values
- maps channels to branch names
- logs measurements
- evaluates trends or abnormal behavior
- provides Grafana visualization

### Upstream power source

- determines input voltage quality
- determines available current
- provides primary regulation
- provides any upstream current limiting
- provides any short-circuit or thermal protection

The Power Monitor Board does not improve or certify the quality of the upstream power source.

---

## Main Measurement Architecture

The board contains:

- 3 × TCA9548A I²C multiplexers
- 9 × INA219 current / voltage monitors
- 8 monitored branch lines
- 1 monitored main line

```text
Raspberry Pi 5 I²C
        ↓
3 × TCA9548A
        ↓
9 × INA219
        ├─ 8 branch monitors
        └─ 1 main-line monitor
```

---

## TCA9548A Addressing and Grouping

The three TCA9548A devices use different A0 / A1 / A2 strap patterns.

| Reference | A2 | A1 | A0 | Connected INA219 devices |
|---|---:|---:|---:|---|
| U1 | GND | GND | GND | U5–U8 |
| U2 | GND | GND | VCC | U9–U12 |
| U3 | GND | VCC | GND | U13 |

This reference order follows the hardware address pattern.

The TCA9548A devices separate the monitor groups and allow multiple INA219 devices to coexist on the same upstream I²C bus.

---

## INA219 Addressing

Each INA219 uses A0 and A1 connection patterns to select its I²C address.

The current board uses different VCC / GND strap combinations so that multiple INA219 devices can be distinguished within each multiplexer group.

The exact INA219 address mapping should be read from:

- the current schematic
- the software channel map
- the INA219 address table

---

## Measurement Channel Allocation

| Monitor group | INA219 references | Purpose |
|---|---|---|
| Branch monitors | U5–U12 | Eight actuator / branch lines |
| Main-line monitor | U13 | Total incoming 5 V line |

The branch names used in software and Grafana should remain aligned with this hardware mapping.

---

## Shunt Resistors

### Branch lines

Each branch uses:

- 0.1 Ω shunt resistor
- 2 W rated shunt resistor

### Main line

The main line uses:

- 0.05 Ω shunt resistor
- 2 W rated shunt resistor

The main-line shunt uses a lower resistance because the total current can be higher than on an individual branch.

The lower value reduces voltage drop and dissipation in the shared path.

---

## Resettable-Fuse Protection

### Main line

- nominal hold current: 1.5 A
- nominal trip current: 3 A

### Each branch

- nominal hold current: 0.5 A
- nominal trip current: approximately 1.1 A

These are resettable-fuse characteristics, not precise electronic current limits.

Trip behavior depends on:

- current magnitude
- ambient temperature
- board temperature
- elapsed time
- device tolerance
- installation conditions

The resettable fuses are intended as supplementary last-resort protection against sustained overcurrent.

They do not replace:

- a suitable upstream power source
- upstream current limiting
- short-circuit protection
- correct wiring
- load-current verification
- operator supervision

---

## Protection Philosophy

The board is expected to receive power from an external regulated source.

If that source has poor regulation, no short-circuit protection, inadequate current limiting, or other deficiencies, routing it through this board does not correct those problems.

Protection is layered conceptually as:

```text
upstream power source
    ↓ primary regulation and protection

main-line resettable fuse
    ↓ supplementary board-wide protection

branch resettable fuses
    ↓ supplementary branch protection

software monitoring
    ↓ observation, logging, and diagnostics
```

The board is a power-distribution and observation board, not a power-conditioning or safety-certification device.

---

## Branch Power Path

A typical branch follows this path:

```text
+5 V main rail
    ↓
manual switch
    ↓
INA219 / shunt measurement
    ↓
resettable fuse
    ↓
ferrite bead
    ↓
local capacitors
    ↓
branch output connector
```

Each branch includes:

- manual enable switch
- power indicator LED
- INA219 measurement
- resettable fuse
- ferrite bead
- local decoupling capacitors
- output connector

The switch must be enabled before branch power is supplied.

The LED provides a local visual indication that the branch is powered.

---

## Branch Noise Mitigation

Each branch includes basic power-line noise mitigation after the resettable fuse.

Typical components include:

- ferrite bead
- 0.1 µF capacitor
- 10 µF capacitor

These components are intended to reduce:

- high-frequency noise entering the branch
- noise returning from motors or servos
- local supply disturbance
- fast transients near the branch output

This is basic mitigation for tabletop actuator wiring.

It does not provide:

- galvanic isolation
- full EMI filtering
- guaranteed suppression for every load
- precision low-noise power delivery

---

## Measurement Boundary

The INA219 measurement point is upstream of some branch components.

A branch is measured before:

- the resettable fuse
- the ferrite bead
- downstream PCB trace
- the output connector
- the external harness
- the actuator-side input path

Therefore, the reported values represent power entering the monitored branch, not necessarily the exact power delivered at the actuator terminals.

The actual load voltage and power may be slightly lower because of downstream voltage drops.

During transients, measured branch current may also include current used to charge downstream decoupling capacitors.

The monitor is therefore best interpreted as:

- branch telemetry
- trend data
- relative comparison
- diagnostic evidence

It is not intended as precision load-terminal metrology.

---

## Measurement Philosophy

The design prioritizes:

- a common measurement boundary
- uniform channel handling
- repeatable polling
- comparable data across branches
- continuous logging
- trend visibility

Measuring directly on each actuator board could provide values closer to the load, but would require distributed measurement hardware, calibration, communication, and maintenance on every actuator.

For this system, stable and repeatable measurement at a consistent cadence is more useful than irregular measurements with higher nominal precision.

Typical uses include:

- comparing normal and abnormal operation
- detecting changes in startup current
- observing actuator load trends
- comparing branches
- identifying drift
- supporting Grafana dashboards

---

## Wiring Auxiliary and Controller Connections

### J2: Wiring Auxiliary side

`J2` receives signals from the Pi5 Wiring Auxiliary.

These include:

- `+5V_PI`
- `3.3V_PI`
- `SDA_OUT`
- `SCL_OUT`
- `MUX_RST_0`
- `MUX_RST_1`
- `MUX_RST_2`
- ground

The reset lines are routed toward the three TCA9548A devices.

### J1: Controller Board side

`J1` forwards the controller-side shared signals toward the Controller Board chain.

These include:

- `+5V_PI`
- ground
- `SDA_OUT`
- `SCL_OUT`
- `E_STOP_BUS_N`

This makes the Power Monitor Board part of the current fixed physical wiring path.

---

## External Power Input

`J11` is the external 5 V power-input connector.

The current actuator-power path assumes:

```text
external 5 V source
      ↓ J11
main-line monitor
      ↓
branch distribution
      ↓
actuator boards
```

The Power Monitor Board does not generate or regulate this 5 V rail.

---

## Power Source Used During Testing

The prototype was operated using:

- [HANMATEK HM305 adjustable bench power supply](https://hanmatek.com/en-eu/collections/power-supplies/products/hanmatek-hm305-adjustable-power-supply)

This model is recorded for reproducibility and is not a required system dependency.

The basic intended source condition is:

- nominal 5 V output
- operation within the board's approximate 3 A main-line design region
- sufficient current for the connected loads

For reduced-load testing, lower current limits such as 0.5 A or 1 A may be used.

An adjustable supply is convenient because voltage and current limits can be changed during staged testing, but it is not mandatory.

Any suitable source may be used if its voltage, available current, wiring, and behavior are compatible with the connected loads.

Upstream protection and output quality remain properties of the selected source.

---

## Current Status

Implemented in hardware:

- external 5 V input
- main-line distribution
- eight output branches
- one main-line INA219 monitor
- eight branch INA219 monitors
- three TCA9548A multiplexers
- INA219 address strapping
- TCA9548A address strapping
- 0.05 Ω main-line shunt
- 0.1 Ω branch shunts
- 2 W shunt resistors
- main resettable fuse
- branch resettable fuses
- branch switches
- branch LEDs
- branch ferrite beads
- branch local capacitors
- Wiring Auxiliary input connector
- Controller Board pass-through connector
- MUX reset routes

Implemented jointly with software:

- multiplexer selection
- INA219 polling
- voltage / current / power conversion
- branch naming
- logging
- Grafana visualization
- trend analysis
- abnormal-consumption review

Not implemented as a hardware guarantee:

- precision load-terminal power measurement
- electronic current limiting
- instant short-circuit isolation
- voltage regulation
- galvanic isolation
- automatic branch shutdown based on measured current
- safety-rated protection

---

## Current Constraints

- designed for tabletop proof-of-concept use
- nominal 5 V system
- approximate 3 A main-line design region
- branch protection is based on resettable fuses
- fuse trip behavior is time- and temperature-dependent
- reported power is measured upstream of downstream branch losses
- downstream filtering can slightly reduce delivered voltage
- measurement accuracy depends on calibration and implementation
- upstream supply quality is not improved by this board
- fixed current topology assumes the board is in the actuator-power path
- current wiring order assumes Wiring Auxiliary → Power Monitor Board → Controller Board
- not qualified as an industrial power-distribution or safety device

---

## Extension Points

Possible future improvements include:

- direct load-side measurement
- per-actuator measurement boards
- calibrated branch measurement
- automated branch shutdown
- electronic current limiting
- faster short-circuit protection
- relay or load-switch control
- higher-current main-line design
- replaceable fuse options
- improved EMI filtering
- galvanic isolation
- connector keying
- branch identification labels
- software-driven branch control
- synchronized sampling
- automated calibration
- improved thermal monitoring
- dedicated power-source validation procedure

---

## BOM

- DigiKey MyList: [Power_Monitor_Board](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U)

Before ordering, verify:

- shunt resistance
- shunt power rating
- resettable-fuse hold and trip characteristics
- connector current rating
- ferrite-bead current rating
- capacitor voltage rating
- polarity
- footprint
- availability
- lifecycle status

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Electrical connectivity | Schematic files in this directory |
| PCB production data | Gerber files in this directory |
| TCA9548A grouping | Current schematic |
| INA219 references and allocation | Current schematic |
| Shunt values | Current schematic and BOM |
| Resettable-fuse characteristics | Current schematic, BOM, and component datasheets |
| Wiring order | Current schematic and hardware architecture |
| Measurement conversion | Software implementation |
| Branch naming | Software and Grafana configuration |
| Power source used during testing | This README and linked HM305 product page |
| Practical measurement boundary | This README and current circuit topology |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Pi5 Wiring Auxiliary → [`../Pi5_Wiring_Auxiliary/`](../Pi5_Wiring_Auxiliary/)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- DC Motor Board → [`../DC_Motor_Board/`](../DC_Motor_Board/)
- Servo Board → [`../Servo_Board/`](../Servo_Board/)
- Sensor Board → [`../Sensor_Board/`](../Sensor_Board/)
- Manufacturing index → [`../Manufacturing/`](../Manufacturing/)
- Software overview → [`../../Software/`](../../Software/)
- Grafana and logging → [`../../Software/`](../../Software/)
- PORT_X / MAIN_LINE power-monitor SQL → [`../../Software/Grafana/sql/PORT_X%20MAIN_LINE/`](../../Software/Grafana/sql/PORT_X%20MAIN_LINE/)
