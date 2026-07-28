# Manufacturing

This directory serves as a cross-board manufacturing and procurement index for the hardware used in the Modular Factory System.

It provides a common entry point for:

- board documentation
- PCB Gerber data
- DigiKey MyList / BOM references
- prototype manufacturing notes
- procurement checks
- substitution guidance

Detailed electrical design, board-specific limitations, and implementation status remain in each board directory and README.

---

## Scope

This directory is an index and review checklist.

It is not:

- a complete production specification
- a certified assembly procedure
- a regulatory-compliance document
- a guarantee that every provided Gerber revision is production-ready
- a substitute for reviewing schematics, board READMEs, and component datasheets

Before ordering or assembling any board, confirm the current board revision, intended use, known limitations, and actual assembled status.

---

## Board Manufacturing Index

| Board | Documentation | Gerber data | DigiKey MyList / BOM | Main role |
|---|---|---|---|---|
| Controller_Board | [`README`](../Controller_Board/) | [`gerber/`](../Controller_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/P0VEJG3F78) | Raspberry Pi Pico-based local I²C control node |
| DC_Motor_Board | [`README`](../DC_Motor_Board/) | [`gerber/`](../DC_Motor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/UO08XWYEAF) | DC motor driver interface |
| Servo_Board | [`README`](../Servo_Board/) | [`gerber/`](../Servo_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/1G5XREGH36) | Servo actuator interface |
| Sensor_Board | [`README`](../Sensor_Board/) | [`gerber/`](../Sensor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/EW3FT9N8K2) | Photo-reflector sensor interface |
| Power_Monitor_Board | [`README`](../Power_Monitor_Board/) | [`gerber/`](../Power_Monitor_Board/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/RITVVGPV8U) | External power entry, branch distribution, and main/branch monitoring |
| Pi5_Wiring_Auxiliary | [`README`](../Pi5_Wiring_Auxiliary/) | [`gerber/`](../Pi5_Wiring_Auxiliary/gerber/) | [DigiKey MyList](https://www.digikey.jp/ja/mylists/list/QO09LGCG39) | Raspberry Pi 5-side wiring and bus support |

---

## PCB Manufacturing Notes

Gerber files are stored in the corresponding board directories.

They can be uploaded to prototype PCB manufacturing services such as Seeed Studio Fusion or an equivalent vendor.

Some manufacturing parameters may be detected automatically, but the generated preview and order settings must still be reviewed manually.

### Before ordering, verify

- board name and revision
- board dimensions
- board outline
- layer count
- drill-file detection
- board thickness
- copper weight
- minimum trace and clearance compatibility
- solder-mask and silkscreen visibility
- connector footprint orientation
- polarized-component orientation
- mounting-hole positions
- quantity
- manufacturing preview
- panelization assumptions, if any
- whether the selected Gerber set matches the intended schematic revision

> These Gerber files are prototype manufacturing data.  
> They may not represent a universally production-ready revision.

---

## Board-Specific Review

Before ordering a PCB, review the corresponding board README for:

- intended system role
- connector mapping
- required connection order
- implemented and unimplemented functions
- current and voltage limits
- protection assumptions
- assembly notes
- known constraints
- extension points
- related firmware or software dependencies

The schematic remains the source of truth for electrical connectivity.

The Gerber files remain the source of truth for PCB fabrication data.

---

## Component Procurement Notes

DigiKey MyLists are provided as procurement references for PCB-mounted components.

They are not a substitute for checking the current schematic, footprint, and component datasheet.

### Before ordering components, verify

- manufacturer part number
- package size
- footprint compatibility
- pinout
- polarity
- rated voltage
- rated current
- power rating
- tolerance
- temperature range
- availability
- lifecycle status
- minimum order quantity
- lead time
- acceptable replacement parts

---

## Substitution Guidance

Substitution may be possible, but equivalent package size alone is not sufficient.

### Passive components

For resistors and capacitors, check:

- resistance or capacitance value
- tolerance
- power rating
- voltage rating
- dielectric type
- ESR or frequency behavior where relevant
- temperature coefficient
- package and land pattern

For example, a YAGEO resistor may be replaced by an equivalent resistor from another vendor such as KOA when the electrical and mechanical characteristics match.

### Shunt resistors

Check:

- resistance value
- tolerance
- power rating
- temperature coefficient
- package
- current-sense suitability

### Ferrite beads

Check:

- impedance characteristic
- rated current
- DC resistance
- package
- frequency range

### Resettable fuses

Check:

- hold current
- trip current
- trip-time behavior
- ambient-temperature derating
- package
- voltage rating

### Semiconductors and ICs

Do not assume compatibility based only on package or function name.

Check:

- pinout
- logic levels
- electrical limits
- timing
- control behavior
- register compatibility
- thermal behavior
- startup state

### Connectors and switches

Check:

- pitch
- keying
- orientation
- mating part
- current rating
- contact arrangement
- mechanical height
- locking behavior

Semiconductors, connectors, switches, protection devices, and sensing components should not be treated as drop-in replacements unless electrical, mechanical, and functional compatibility has been confirmed.

---

## Assembly Guidance

The boards are intended for prototype and tabletop proof-of-concept use.

Assembly should include:

- visual inspection before soldering
- footprint and polarity confirmation
- staged assembly where practical
- continuity checks before first power-on
- current-limited first power-on
- connector-orientation review
- verification against the board schematic
- functional checks before connecting actuators

For detailed board behavior, refer to the corresponding board README.

---

## First Power-On Guidance

A cautious first power-on sequence is recommended:

1. inspect the assembled board
2. confirm polarity and connector orientation
3. check for shorts between power and ground
4. use a current-limited source
5. begin with reduced current where practical
6. verify local power rails
7. verify communication before connecting actuators
8. connect loads one branch at a time
9. confirm expected current and temperature behavior

This is general prototype guidance, not a certified safety procedure.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Board role and limitations | Corresponding board README |
| Electrical connectivity | Corresponding board schematic |
| PCB fabrication data | Corresponding Gerber directory |
| Part selection | Board BOM documentation and linked DigiKey MyList |
| Component limits | Manufacturer datasheets |
| Firmware behavior | [`../../Firmware/`](../../Firmware/) |
| Register behavior | [`../../Docs/Register_Map/`](../../Docs/Register_Map/) |
| Hardware architecture | [`../README.md`](../README.md) |

---

## Related Documents

- Hardware overview → [`../README.md`](../README.md)
- Controller Board → [`../Controller_Board/`](../Controller_Board/)
- DC Motor Board → [`../DC_Motor_Board/`](../DC_Motor_Board/)
- Servo Board → [`../Servo_Board/`](../Servo_Board/)
- Sensor Board → [`../Sensor_Board/`](../Sensor_Board/)
- Power Monitor Board → [`../Power_Monitor_Board/`](../Power_Monitor_Board/)
- Pi5 Wiring Auxiliary → [`../Pi5_Wiring_Auxiliary/`](../Pi5_Wiring_Auxiliary/)
