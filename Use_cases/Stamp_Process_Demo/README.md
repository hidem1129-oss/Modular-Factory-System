# Stamp Process Demo

This use case demonstrates a tabletop press-style process that holds roll paper, lowers a stamp through a motorized rack-and-pinion mechanism, releases the stamp, and advances the paper to the next position.

The demonstration combines two DC motor nodes, two servo nodes, host-side orchestration, live monitoring, SQLite logging, and Grafana visualization.

It is implemented as one process-specific configuration of the Modular Factory System rather than as an independent production press machine.

---

## Overview

The Stamp Process Demo represents a simplified repetitive processing line.

Roll paper is positioned beneath a stamp mechanism.

Two servo-controlled mechanisms hold the paper during the stamping operation. A DC motor drives a rack-and-pinion mechanism to move the stamp downward and upward. After the paper is released, a second DC motor advances the paper to the next processing position.

```text
Paper initially positioned
      ↓
Clamp paper
      ↓
Lower stamp
      ↓
Hold stamp position
      ↓
Raise stamp
      ↓
Release paper
      ↓
Advance paper
      ↓
Repeat next cycle
```

The current orchestration script performs ten repeated stamp cycles.

The I2C Debugger can observe the participating control nodes and electrical measurements while Grafana can visualize the persisted records.

---

## Process Goal

The process goal is to repeatedly mark roll paper by coordinating:

* material holding
* downward press motion
* temporary press-position hold
* upward return motion
* material release
* controlled paper advancement

The demonstration is intended to validate the integration of:

* sequential multi-actuator operation
* distributed I²C control
* bidirectional DC motor motion
* coordinated dual-servo positioning
* time-based process sequencing
* repeated process cycles
* live monitoring
* persistent logging
* historical visualization

The objective is not to reproduce a production press, printing machine, or certified stamping process.

The objective is to demonstrate how a different physical process can reuse the same control, monitoring, persistence, and visualization architecture used by other Modular Factory System demonstrations.

---

## Architecture Role

This use case sits above the common hardware, firmware, and monitoring layers.

```text
Use-case-specific layer
├─ roll-paper path
├─ clamp mechanism
├─ rack-and-pinion press mechanism
├─ stamp geometry
├─ actuator positions
├─ motor directions
├─ process timing
├─ cycle count
└─ orchestration sequence

Reusable platform layer
├─ distributed I²C control nodes
├─ register-based command interface
├─ motor-node firmware
├─ servo-node firmware
├─ I2C Debugger
├─ SQLite logging
└─ Grafana visualization
```

The physical mechanism and sequence parameters can change without redesigning the complete monitoring and control platform.

---

## System Boundary

The use case coordinates multiple nodes but does not move all device-level control into the host script.

### Orchestration Responsibilities

The host-side orchestration script is responsible for:

* selecting the operation sequence
* commanding both clamp servos
* commanding downward stamp motion
* waiting for the configured hold period
* commanding upward stamp motion
* releasing the clamps
* commanding paper take-up
* repeating the configured number of cycles
* stopping participating nodes during cleanup

### Firmware Responsibilities

The motor and servo nodes remain responsible for:

* receiving register-based commands
* validating local command parameters
* executing time-mode motion
* controlling the connected actuator
* managing device-local lifecycle behavior
* exposing status and feedback through registers

### Monitoring Responsibilities

The I2C Debugger is responsible for:

* observing node states
* interpreting register values
* detecting state transitions
* displaying current system conditions
* recording events and snapshots in SQLite

Grafana reads the persisted observations for historical visualization and analysis.

---

## Initial Conditions

Before the script starts, the demonstrated mechanism assumes that:

* roll paper is installed in the mechanism
* the paper is routed through the clamp and take-up path
* the first stamping position is aligned beneath the stamp
* the stamp mechanism is in a position where the configured press motion can begin
* both clamp mechanisms can move through their configured range
* participating I²C nodes are connected and responding
* the I²C bus is available as bus `1`

The current script does not perform automatic homing or initial-position detection.

Initial mechanical positioning is therefore part of the setup procedure.

---

## Reused Platform Components

### Control Hardware

| Component                             | Role in this use case                               |
| ------------------------------------- | --------------------------------------------------- |
| Raspberry Pi 5                        | Runs orchestration, monitoring, and data services   |
| Raspberry Pi Pico-based control nodes | Provide distributed local control through I²C       |
| Controller Board                      | Hosts or supports local control-node implementation |
| DC Motor Boards                       | Drive the stamp and paper take-up motors            |
| Servo Boards                          | Drive the two paper-clamp servos                    |
| Power Monitor Board                   | Acquires voltage and current for monitored channels |
| Pi 5 Wiring Auxiliary Board           | Supports host-side I²C and wiring distribution      |

### Firmware Roles

| Firmware role | Responsibility                           |
| ------------- | ---------------------------------------- |
| Motor node    | Controls press and paper take-up motions |
| Servo node    | Controls one paper-clamp mechanism       |

The current orchestration script uses two motor nodes and two servo nodes.

### Software Components

| Software                                                | Role                                                                         |
| ------------------------------------------------------- | ---------------------------------------------------------------------------- |
| [`stamp_press_demo.py`](./stamp_press_demo.py)          | Coordinates clamping, pressing, release, paper take-up, and cycle repetition |
| [`Software/I2C_Debugger`](../../Software/I2C_Debugger/) | Provides live observation and SQLite persistence                             |
| [`Software/Grafana`](../../Software/Grafana/)           | Visualizes recorded state and electrical history                             |
| Python `smbus2`                                         | Provides direct I²C register access from the orchestration script            |

---

## Use-Case-Specific Components

The following elements are specific to this process:

* roll-paper path
* paper take-up mechanism
* two-sided or dual-point clamp arrangement
* rack-and-pinion press mechanism
* physical stamp
* stamp mounting
* stamp travel distance
* paper advance amount
* clamp angles
* press and release directions
* press hold time
* cycle interval
* total cycle count

These elements can be changed without replacing the common monitoring and persistence path.

---

## Mechanical Construction

The stamp mechanism is a functional tabletop prototype assembled from available mechanical parts and temporary fixtures.

It does not require a specific industrial frame or standardized structural system.

The mechanism may use:

* commercially available construction-kit parts
* general-purpose gears and shafts
* rack-and-pinion components
* wooden or plastic structural plates
* servo horns
* brackets
* screws, nuts, washers, and spacers
* tape, cable ties, or other temporary attachment methods
* custom parts where necessary

The current physical implementation is one possible arrangement.

The control architecture does not require the same products, dimensions, or attachment methods.

---

## Actual Mechanical Parts Used

This section records the main commercially sourced mechanical parts used in the demonstrated configuration.

| Source part or product                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          | Original intended role                               | Role in this demo                                                       | Reason selected                                                                                                            |
| ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------- | ----------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| [PILOT Frixion Stamp](https://webcatalog.pilot.co.jp/products/DispDetail.do?itemID=t000100003553&volumeName=00004&category=%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B7%E3%83%AA%E3%83%BC%E3%82%BA%2F%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B7%E3%83%AA%E3%83%BC%E3%82%BA%28%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B9%E3%82%BF%E3%83%B3%E3%83%97%29%2F%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B9%E3%82%BF%E3%83%B3%E3%83%97&searchTypeParam=categorySearch) | Manually operated self-inking stationery stamp       | Physical marking tool                                                   | Available off-the-shelf tool that can produce repeated visible marks without a separate ink pad                            |
| [ラック&ピニオン ギヤセット](https://www.tamiya.com/japan/products/70235/index.html)                                                                                                                                                                                                                                                                                                                                                                                                                                                                        | Linear-motion mechanism for educational construction | Moves the stamp toward the paper and separates it after stamping        | Converts motor rotation into reversible linear motion                                                                      |
| [ユニバーサルプレート（2枚セット）](https://www.tamiya.com/japan/products/70157/index.html)                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | General-purpose construction plates                  | Supports and mounts the rack-and-pinion mechanism and its drive gearbox | Provides a regular mounting-hole pattern and a compact elevated structure                                                  |
| [ミニモーター標準ギヤボックス（8速）](https://www.tamiya.com/japan/products/70188/index.html)                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | General-purpose geared motor drive                   | Drives the roll-paper take-up mechanism                                 | Provides multiple gear-ratio options and sufficient space is available around the paper take-up section                    |
| [ミニモーター低速ギヤボックス（4速）](https://www.tamiya.com/japan/products/70189/index.html)                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Compact low-speed geared motor drive                 | Drives the rack-and-pinion stamp mechanism                              | Its smaller size allows it to be mounted on the upper universal plates without significantly obstructing the mechanism    |
| [ユニバーサルプレートL（210 × 160 mm）](https://www.tamiya.com/japan/products/70172/index.html)                                                                                                                                                                                                                                                                                                                                                                                                                                                             | General-purpose construction base                    | Main structural base for the stamp-process mechanism                    | Provides a large mounting area and regular hole pattern for arranging the paper path, actuators, and supporting structures |

The low-speed four-speed gearbox is mounted on the universal plates together with the rack-and-pinion mechanism.

Its compact size is important in the demonstrated configuration because the stamp-drive mechanism is positioned above the main base structure. A larger gearbox could interfere with the moving mechanism, the paper path, or nearby components.

This record describes the demonstrated implementation.

It does not imply that the same products are required to reproduce the system architecture.


---

## Process Material

The demonstrated configuration uses a commercially available roll of paper tape purchased from a local store.

The paper tape was not selected according to a predefined material specification.

It was selected because its roll shape and approximate size appeared suitable for installation in a tabletop mechanism and for continuous feeding, clamping, stamping, and take-up.

The holding, guiding, and take-up mechanisms were then arranged around the available paper roll.

```text
Available paper roll
      ↓
Check whether it can be installed and fed
      ↓
Arrange the holding and paper path
      ↓
Build the clamp and take-up mechanisms around it
      ↓
Adjust the process timing for the resulting configuration
```

This represents a material-first prototype approach rather than selection against a fixed production-material specification.

### Demonstrated Material Properties

| Property            | Demonstrated configuration                                                       |
| ------------------- | -------------------------------------------------------------------------------- |
| Material type       | Commercially available paper tape                                                |
| Source              | Purchased from a local store                                                     |
| Form                | Continuous paper wound into a roll                                               |
| Approximate width   | `17 mm`, measured with a ruler                                                   |
| Thickness           | Not measured                                                                     |
| Paper specification | Not identified                                                                   |
| Roll dimensions     | Selected to fit within the tabletop mechanism                                    |
| Feed direction      | Toward the take-up mechanism                                                     |
| Processed feature   | Repeated stamped marks                                                           |
| Selection basis     | Availability, roll form, and apparent compatibility with the prototype mechanism |

The paper roll is similar in general form to a small roll of kitchen paper, although it is used here as paper tape for the stamping demonstration.

The exact product is not required to reproduce the system architecture.

A replacement roll material may be used if:

* it can be installed in the available mechanism
* it can be pulled through the paper path
* it can be held by the clamp mechanisms
* it accepts a visible stamp mark
* it can be wound by the take-up mechanism

Changing the roll width, stiffness, surface, core size, or diameter may require modifications to the mechanical arrangement and process parameters.

---

## Stamp Tool

The demonstrated configuration uses a commercially available PILOT Frixion Stamp as the physical marking tool.

Product reference:

* [PILOT Frixion Stamp official product page](https://webcatalog.pilot.co.jp/products/DispDetail.do?itemID=t000100003553&volumeName=00004&category=%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B7%E3%83%AA%E3%83%BC%E3%82%BA%2F%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B7%E3%83%AA%E3%83%BC%E3%82%BA%28%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B9%E3%82%BF%E3%83%B3%E3%83%97%29%2F%E3%83%95%E3%83%AA%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3%E3%82%B9%E3%82%BF%E3%83%B3%E3%83%97&searchTypeParam=categorySearch)

The Frixion Stamp is an off-the-shelf self-inking stationery stamp intended for manual use.

In this demonstration, it is reused as the marking tool of a motorized tabletop process rather than being operated directly by hand.

### Mounting Method

The stamp is attached to a bar-shaped moving part of the rack-and-pinion mechanism.

The rack-and-pinion mechanism converts DC motor rotation into horizontal linear movement.

The stamp body is secured to the moving part using rubber bands.

```text
DC motor rotation
      ↓
Pinion rotation
      ↓
Rack movement
      ↓
Horizontal movement of the bar-shaped member
      ↓
Movement of the attached stamp
      ↓
Contact between the stamp and roll paper
```

The rubber-band attachment is a simple prototype fixture that allows:

* rapid attachment and removal
* easy adjustment of the stamp position
* reuse of the commercially available stamp without modifying its body
* validation without manufacturing a dedicated stamp holder

The attachment does not provide precisely controlled rigidity, position, orientation, or contact force.

### Demonstrated Configuration

| Property                  | Demonstrated configuration                      |
| ------------------------- | ----------------------------------------------- |
| Product type              | PILOT Frixion Stamp                             |
| Original intended use     | Manually operated stationery stamp              |
| Role in this demo         | Physical marking tool                           |
| Ink type                  | Frixion ink                                     |
| Stamp mechanism           | Self-inking stamp                               |
| Drive mechanism           | DC motor-driven rack-and-pinion mechanism       |
| Moving direction          | Horizontal linear motion                        |
| Attachment method         | Secured to the moving member using rubber bands |
| Position feedback         | None                                            |
| Contact-force measurement | None                                            |
| Automatic mark inspection | None                                            |

The exact stamp design or printed symbol should be recorded separately if it is relevant to evaluating the resulting mark.

The stamp and its rubber-band mounting are specific to the demonstrated implementation.

They could be replaced by a different commercial stamp, a rigid tool holder, a 3D-printed adapter, or another contact-processing tool without replacing the common distributed-control and monitoring architecture.


---

## I²C Node Assignment

The current orchestration script assigns the following addresses:

| Function                    | I²C address |
| --------------------------- | ----------- |
| Stamp press DC motor node   | `0x11`      |
| Paper take-up DC motor node | `0x18`      |
| Clamp servo node A          | `0x13`      |
| Clamp servo node B          | `0x17`      |

These addresses belong to the demonstrated configuration.

They are not architectural requirements and may be changed if the node-address assignment is updated consistently.

---

## Register Interface

The current script uses the common register-based interface.

| Register              | Address | Purpose in this script                              |
| --------------------- | ------: | --------------------------------------------------- |
| Operation mode        |  `0x1D` | Selects time-based operation                        |
| Command               |  `0x30` | Applies, starts, stops, and selects motor direction |
| Setpoint high byte    |  `0x31` | Upper byte of speed or servo-angle command          |
| Setpoint low byte     |  `0x32` | Lower byte of speed or servo-angle command          |
| Target time high byte |  `0x35` | Upper byte of time-mode target                      |
| Target time low byte  |  `0x36` | Lower byte of time-mode target                      |

The script writes parameters, applies them through the latch command, clears the command, and then issues the run command.

---

## Stamp Press Motion

The press and release operations use the same DC motor node with opposite direction commands.

### Press Motion

| Parameter       | Current value |
| --------------- | ------------: |
| Node address    |        `0x11` |
| Speed command   |         `700` |
| Time-mode tick  |          `35` |
| Direction value |           `0` |
| Script wait     |       `1.0 s` |

### Release Motion

| Parameter       | Current value |
| --------------- | ------------: |
| Node address    |        `0x11` |
| Speed command   |         `700` |
| Time-mode tick  |          `35` |
| Direction value |           `1` |
| Script wait     |       `1.0 s` |

The physical press travel depends on:

* motor characteristics
* gearbox ratio
* rack-and-pinion geometry
* command interpretation
* supply condition
* friction
* mechanical alignment
* load from the stamp and paper

The current implementation does not use a press-position sensor, encoder, or limit switch.

---

## Stamp Hold

After the downward press command, the script waits before commanding the return motion.

Current hold time:

```text
0.5 seconds
```

This provides a short period for the stamp to remain at or near the pressed position.

The script does not confirm actual contact force, travel, or stamp position during this hold period.

---

## Clamp Motion

The current mechanism uses two servo nodes.

Their target angles are not identical because the two physical servo installations use different orientations or linkage geometry.

### Clamp Position

| Servo node | Command angle |
| ---------- | ------------: |
| `0x13`     |         `50°` |
| `0x17`     |         `95°` |

### Unclamp Position

| Servo node | Command angle |
| ---------- | ------------: |
| `0x13`     |         `15°` |
| `0x17`     |        `130°` |

Common servo target-time command:

```text
300
```

Script wait after clamp or unclamp command:

```text
0.40 seconds
```

The displayed angles are software command values.

The actual clamp travel and force depend on:

* servo installation angle
* horn position
* linkage geometry
* clamp structure
* mechanical limits
* paper thickness
* power-supply condition

The current implementation does not use clamp-position or clamp-force feedback.

---

## Paper Take-Up Motion

The paper is advanced after the clamp has been released.

Current take-up parameters:

| Parameter       | Current value |
| --------------- | ------------: |
| Node address    |        `0x18` |
| Speed command   |         `800` |
| Time-mode tick  |          `20` |
| Direction value |           `1` |
| Script wait     |      `0.60 s` |

The paper-advance distance is time-based.

It is not measured using:

* an encoder
* registration marks
* a position sensor
* image-based paper tracking
* roll-diameter compensation
* closed-loop tension control

As the take-up roll diameter changes, the paper distance moved by one motor rotation may also change.

The current demonstration does not compensate for this effect.

---

## Operation Sequence

```text
Verify initial mechanical setup
      ↓
Start cycle
      ↓
Command both clamp servos
      ↓
Wait for clamp motion
      ↓
Command stamp motor downward
      ↓
Wait for press motion
      ↓
Hold stamp position
      ↓
Command stamp motor upward
      ↓
Wait for return motion
      ↓
Command both clamp servos to release
      ↓
Wait for unclamp motion
      ↓
Command paper take-up motor
      ↓
Wait for paper advance
      ↓
Wait for cycle interval
      ↓
More cycles?
      ├─ Yes → repeat
      └─ No → stop all participating nodes
```

---

## Cycle Configuration

The current script repeats the process:

```text
10 cycles
```

Cycle counter:

```text
1 to 10
```

Interval after paper take-up:

```text
0.50 seconds
```

The cycle count and timing values are fixed constants in the current script.

---

## Timing Parameters

| Parameter          | Current value | Purpose                                               |
| ------------------ | ------------: | ----------------------------------------------------- |
| Command latch wait |      `0.05 s` | Separates parameter application from the next command |
| Command-clear wait |      `0.05 s` | Separates stop or clear operations                    |
| Clamp wait         |      `0.40 s` | Allows clamp motion                                   |
| Press wait         |       `1.0 s` | Allows downward press motion                          |
| Stamp hold         |      `0.50 s` | Holds the stamp near the processing position          |
| Release wait       |       `1.0 s` | Allows upward return motion                           |
| Unclamp wait       |      `0.40 s` | Allows clamp release                                  |
| Paper take-up wait |      `0.60 s` | Allows paper advancement                              |
| Cycle interval     |      `0.50 s` | Separates repeated cycles                             |

These values are tuned for the current tabletop mechanism and are not general system constants.

---

## Monitoring and Logging

The I2C Debugger can monitor the distributed nodes while the orchestration script operates the process.

Available observations may include:

* press-motor node lifecycle state
* paper-motor node lifecycle state
* clamp-servo node states
* command and feedback registers
* communication availability
* detected state transitions
* periodic node snapshots
* voltage and current measurements
* calculated-power values
* monitoring-session context

The monitoring layer observes exposed node state.

It does not directly measure:

* stamp force
* stamp contact
* paper position
* clamp force
* transferred-mark quality
* actual mechanical travel

A motor or servo command being accepted therefore does not independently prove that the physical process completed correctly.

---

## Grafana Visualization

Grafana reads persisted SQLite records generated by the I2C Debugger.

A stamp-process run can be investigated using:

* node-state timelines
* recent state-transition events
* ERROR and ESTOP history
* monitoring-session selection
* voltage, current, and calculated-power time series
* average and maximum recorded calculated-power comparison
* recorded maximum, minimum, and maximum-to-minimum ranges

This supports questions such as:

* When did each node change state?
* Were the two clamp-servo operations observed in the expected period?
* Did the electrical load change during the press or take-up motion?
* Did repeated cycles show similar recorded behavior?
* Was any participating node missing or in an error state?

Recorded timing can be correlated with the expected process sequence, but it does not by itself prove the physical cause or result of an event.

---

## Validation Approach

The demonstration should be evaluated at multiple levels.

### Process Validation

Confirm that:

* both clamp mechanisms hold the paper
* the stamp moves downward
* the stamp contacts or marks the paper
* the stamp returns upward
* both clamp mechanisms release the paper
* the paper advances
* the process can repeat for the configured cycle count

### Interface Validation

Confirm that:

* all four I²C nodes respond
* expected register commands are accepted
* motor directions match the intended mechanism
* servo addresses map to the intended clamps
* cleanup commands stop all participating nodes

### Output Validation

Confirm that:

* a visible mark is transferred
* marks appear at successive positions
* the paper is not excessively wrinkled or torn
* stamp alignment remains acceptable
* feed spacing remains acceptable for the intended demonstration

### Historical Validation

Confirm that:

* a monitoring session is recorded
* node-state changes are persisted
* electrical snapshots are available
* Grafana can visualize the intended analysis period

---

## Demonstrated Result

The use case demonstrates that one process sequence can coordinate:

- one bidirectional DC motor for lowering and raising the stamp
- one DC motor for paper take-up
- two independently positioned clamp servos
- a timed stamp hold
- material release
- paper advancement
- repeated cycles
- live system monitoring
- persisted state and measurement history

It also demonstrates that the Modular Factory System can be reconfigured from a sensing-and-sorting process into a sequential press-style process while retaining the common control and observation architecture.

---

## Current Limitations

The current demonstration has the following limitations:

* initial paper and stamp positions are set manually
* the process has no automatic homing
* stamp travel is time-based
* paper advance is time-based
* the press mechanism has no encoder
* the press mechanism has no limit switch
* clamp position and force are not measured
* stamp contact force is not measured
* transferred-mark quality is not evaluated automatically
* paper position is not detected
* take-up roll diameter is not compensated
* paper tension is not controlled
* cycle parameters are fixed in the script
* process-level timeout and retry behavior are limited
* the mechanism is not designed for industrial speed, accuracy, durability, or safety
* the demonstration does not implement production-grade guarding or certified safety control

These constraints are acceptable for the current tabletop architecture-validation scope.

---

## Implementation Note

The orchestration script is a demonstration implementation rather than a production process controller.

It currently contains:

* fixed I²C addresses
* fixed register addresses
* fixed motion parameters
* fixed servo angles
* fixed waits
* a fixed cycle count
* direct `smbus2` register access

Future refactoring could separate:

* register-access functions
* node configuration
* motion profiles
* process sequencing
* calibration values
* cycle configuration
* error handling
* result logging

into independently testable components.

The script does include cleanup behavior that attempts to clear commands on all participating nodes even if execution exits through an exception.

---

## Possible Future Improvements

Possible improvements include:

* automatic homing
* upper and lower press limit switches
* paper-position detection
* encoder-based press travel
* encoder-based paper feed
* registration-mark detection
* adjustable cycle count
* external configuration for motion parameters
* configurable timeouts and retry behavior
* clamp-position feedback
* clamp-force sensing
* press-force sensing
* current-based abnormal-load detection
* paper-tension control
* roll-diameter compensation
* automatic mark-quality inspection
* structured process-result logging
* replacement of temporary fixtures with refined mounts where required
* 3D-printed guides, brackets, or stamp holders after geometry validation

---

## Reconfiguration Lessons

This demonstration shows which elements belong to the reusable platform and which remain process-specific.

```text
Reusable
├─ distributed I²C-node architecture
├─ motor and servo firmware roles
├─ register-based interface
├─ monitoring
├─ SQLite persistence
└─ Grafana visualization

Process-specific
├─ paper path
├─ clamp geometry
├─ rack-and-pinion mechanism
├─ stamp tool
├─ actuator directions
├─ servo angles
├─ timing values
├─ paper-advance amount
└─ process sequence
```

A different press-style or feed-and-process application could replace the stamp, material, or mechanism without replacing the complete control and monitoring stack.

---

## Related Documents

* Use cases overview → [`../README.md`](../README.md)
* Hardware overview → [`../../Hardware/`](../../Hardware/)
* Firmware overview → [`../../Firmware/`](../../Firmware/)
* Software overview → [`../../Software/`](../../Software/)
* Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
* System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
* I2C Debugger → [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/)
* Grafana → [`../../Software/Grafana/`](../../Software/Grafana/)
* DC Motor Board → [`../../Hardware/DC_Motor_Board/`](../../Hardware/DC_Motor_Board/)
* Servo Board → [`../../Hardware/Servo_Board/`](../../Hardware/Servo_Board/)
* Power Monitor Board → [`../../Hardware/Power_Monitor_Board/`](../../Hardware/Power_Monitor_Board/)
