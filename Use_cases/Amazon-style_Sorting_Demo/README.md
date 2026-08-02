# Amazon-style Sorting Demo

This use case demonstrates a tabletop sorting process that detects a workpiece, classifies its color, and diverts it through a corresponding servo-driven gate.

The demonstration combines a conveyor, a photo-reflector, a camera, distributed I²C control nodes, host-side orchestration, live monitoring, SQLite logging, and Grafana visualization.

It is implemented as one process-specific configuration of the Modular Factory System rather than as an independent single-purpose machine.

---

## Overview

The sorting demo represents a simplified tabletop logistics process.

A DC motor-driven conveyor transports a workpiece through the mechanism.

A photo-reflector detects the arrival of the workpiece. After a configured delay, the Raspberry Pi 5 captures an image from the camera and classifies the workpiece as:

* blue
* green
* yellow
* unknown

The orchestration script then selects one of three servo-controlled sorting gates according to the classification result.

```text
Conveyor transport
      ↓
Photo-reflector trigger
      ↓
Capture delay
      ↓
Camera image acquisition
      ↓
HSV-based color classification
      ↓
Select corresponding servo node
      ↓
Activate sorting gate
      ↓
Continue conveyor transport
```

The I2C Debugger can observe the participating nodes and electrical measurements while Grafana can visualize the persisted records after or during the demonstration.

---

## Process Goal

The process goal is to transport a workpiece, detect its arrival, classify it by color, and physically divert it toward the corresponding destination.

The demonstration is intended to validate the integration of:

* continuous material transport
* event-driven sensing
* delayed image acquisition
* image-based classification
* process-level decision-making
* distributed actuator control
* live monitoring
* persistent logging
* historical visualization

The objective is not to reproduce a commercial warehouse sorter or an industrial vision system at production scale.

The objective is to demonstrate the architectural flow from physical detection to software decision and physical actuation.

---

## Architecture Role

This use case sits above the common hardware, firmware, and monitoring layers.

```text
Use-case-specific layer
├─ sorting mechanism
├─ camera position
├─ sensor threshold
├─ color-classification thresholds
├─ capture timing
├─ destination mapping
└─ process orchestration

Reusable platform layer
├─ distributed I²C control nodes
├─ register-based command interface
├─ motor-node firmware
├─ servo-node firmware
├─ sensor-node firmware
├─ I2C Debugger
├─ SQLite logging
└─ Grafana visualization
```

The sorting logic can change without redesigning the complete control and monitoring platform.

Likewise, the current physical mechanism could be replaced while retaining a similar node interface and software boundary.

---

## System Boundary

The use case coordinates multiple components but does not move all control responsibility into the orchestration script.

### Orchestration Responsibilities

The host-side orchestration script is responsible for:

* starting the conveyor
* polling the workpiece sensor
* determining when image capture should occur
* capturing a camera frame
* classifying the detected color
* mapping the classification result to a sorting gate
* issuing commands to the corresponding servo node
* coordinating the overall process sequence

### Firmware Responsibilities

The distributed firmware nodes remain responsible for:

* receiving register-based commands
* validating device-local commands
* controlling the connected motor, servo, or sensor
* managing device-local lifecycle behavior
* exposing status and feedback through the register interface

### Monitoring Responsibilities

The I2C Debugger is responsible for:

* observing node states
* interpreting register values
* detecting state transitions
* displaying the current system condition
* recording events and snapshots in SQLite

Grafana reads the persisted observations for historical visualization and analysis.

---

## Reused Platform Components

### Control Hardware

| Component                             | Role in this use case                                                |
| ------------------------------------- | -------------------------------------------------------------------- |
| Raspberry Pi 5                        | Runs orchestration, camera processing, monitoring, and data services |
| Raspberry Pi Pico-based control nodes | Provide distributed local control through I²C                        |
| Controller Board                      | Hosts or supports the local control-node implementation              |
| DC Motor Board                        | Drives the conveyor motor                                            |
| Servo Boards                          | Drive the three sorting-gate servos                                  |
| Sensor Board                          | Acquires the photo-reflector value                                   |
| Power Monitor Board                   | Acquires voltage and current for monitored electrical channels       |
| Pi 5 Wiring Auxiliary Board           | Supports host-side I²C and wiring distribution                       |

### Firmware Roles

| Firmware role | Responsibility                                 |
| ------------- | ---------------------------------------------- |
| Motor node    | Controls conveyor operation                    |
| Servo node    | Controls one sorting gate                      |
| Sensor node   | Acquires and exposes the photo-reflector value |

The current orchestration script uses one motor node, one sensor node, and three gate-servo nodes.

### Software Components

| Software                                                | Role                                                                     |
| ------------------------------------------------------- | ------------------------------------------------------------------------ |
| [`warehouse_demo.py`](./warehouse_demo.py)              | Coordinates transport, sensing, image classification, and gate selection |
| [`Software/I2C_Debugger`](../../Software/I2C_Debugger/) | Provides live observation and SQLite persistence                         |
| [`Software/Grafana`](../../Software/Grafana/)           | Visualizes recorded state and electrical history                         |
| OpenCV                                                  | Captures and processes camera images                                     |
| NumPy                                                   | Supports image-data filtering and median calculation                     |
| Linux `i2cset` / `i2cget`                               | Performs register access from the orchestration script                   |

---

## Use-Case-Specific Components

The following elements are specific to this sorting process:

* conveyor mechanism
* workpiece-detection position
* camera and camera mounting position
* image region of interest
* color-classification thresholds
* blue sorting gate
* green sorting gate
* yellow sorting gate
* capture delay
* gate timing
* physical destination arrangement

These elements can be changed without replacing the common monitoring and persistence path.

---

## Mechanical Construction

The sorting mechanism is a functional tabletop prototype assembled from available mechanical parts and temporary fixtures.

It does not require one specific construction product, manufacturer, or standardized structural system.

The current mechanism may include:

* commercially available construction-kit parts
* general-purpose gears, shafts, wheels, or plates
* reused components
* available screws, nuts, washers, or spacers
* wooden or plastic structural materials
* tape, cable ties, or other temporary attachment methods
* custom parts where necessary

The original intended use of a commercial part does not determine its role in this demonstration.

A part is selected according to properties such as:

* dimensions
* shape
* stiffness
* available motion
* mounting convenience
* compatibility with the workpiece
* ease of replacement
* availability during prototyping

---

## Actual Mechanical Parts Used

This section should record the parts actually used in the demonstrated configuration.

| Source part or product | Original intended role | Role in this demo                   | Modification or attachment | Reason selected |
| ---------------------- | ---------------------- | ----------------------------------- | -------------------------- | --------------- |
| To be documented       | —                      | Conveyor structure or drive element | —                          | —               |
| To be documented       | —                      | Workpiece guide                     | —                          | —               |
| To be documented       | —                      | Camera support                      | —                          | —               |
| To be documented       | —                      | Photo-reflector support             | —                          | —               |
| To be documented       | —                      | Sorting-gate mechanism              | —                          | —               |

This record describes the demonstrated implementation.

It does not imply that the same commercial products or parts are required to reproduce the system architecture.

---

## Workpiece

The current demonstration classifies workpieces by visible color.

The implemented classification categories are:

| Classification | Destination            |
| -------------- | ---------------------- |
| Blue           | Blue gate              |
| Green          | Green gate             |
| Yellow         | Yellow gate            |
| Unknown        | No mapped sorting gate |

The workpiece geometry, material, dimensions, and surface finish should be documented for the demonstrated configuration because they can affect:

* sensor detection
* conveyor transport
* image appearance
* gate interaction
* repeatability

Current workpiece details:

| Property               | Demonstrated configuration |
| ---------------------- | -------------------------- |
| Shape                  | To be documented           |
| Approximate dimensions | To be documented           |
| Material               | To be documented           |
| Color variants         | Blue, green, and yellow    |
| Surface condition      | To be documented           |

---

## I²C Node Assignment

The current orchestration script assigns the following addresses:

| Function               | I²C address |
| ---------------------- | ----------- |
| Conveyor DC motor node | `0x14`      |
| Workpiece sensor node  | `0x15`      |
| Blue gate servo node   | `0x13`      |
| Green gate servo node  | `0x16`      |
| Yellow gate servo node | `0x17`      |

These addresses belong to the demonstrated configuration.

They are not architectural requirements and may be changed if the node-address assignment is updated consistently.

---

## Conveyor Control

The conveyor is started through the DC motor node.

The orchestration script writes:

* operation parameters
* motor-control values
* lifecycle or mode values
* command transitions

through the register-based I²C interface.

The host script decides when the conveyor should start.

The motor-node firmware remains responsible for executing the local motor-control behavior.

The current script starts the conveyor before entering the continuous sensor-monitoring loop.

---

## Workpiece Detection

The photo-reflector is accessed through the sensor node.

The current script:

1. configures or triggers sensor acquisition
2. waits briefly for the sensor value to update
3. reads the high and low bytes
4. combines them into one 16-bit value
5. compares the value with a configured threshold

Current threshold:

```text
3600
```

The workpiece trigger condition is:

```text
sensor value < threshold
```

The threshold is specific to the demonstrated sensor position, workpiece, surface, lighting, and mechanical arrangement.

It should therefore be calibrated again if those conditions change.

---

## Capture Timing

After a sensor trigger, the script waits before using the camera.

Current capture delay:

```text
1.8 seconds
```

This delay allows the conveyor to move the workpiece from the photo-reflector position into the camera observation area.

The delay is currently time-based.

It is not calculated from:

* measured conveyor speed
* encoder feedback
* workpiece position feedback
* image-based arrival detection
* a second position sensor

As a result, changes in conveyor speed, friction, workpiece geometry, or sensor position may require recalibration.

---

## Camera Configuration

The script opens camera device:

```text
CAMERA_ID = 0
```

Requested capture resolution:

```text
320 × 240 pixels
```

Several initial frames are discarded before normal operation to allow the camera stream to stabilize.

The current script processes one selected frame after the configured capture delay.

---

## Image Region of Interest

Color classification is performed on a rectangular region of interest rather than the complete image.

The current ROI is defined proportionally:

```text
Horizontal range: 20% to 80% of image width
Vertical range:   45% to 75% of image height
```

This concentrates the calculation on the expected workpiece area and reduces the influence of unrelated image regions.

The ROI assumes that the camera and mechanism remain in approximately the calibrated positions.

---

## Color Classification

The captured BGR image is converted into HSV color space.

Pixels are retained when:

```text
Saturation > 100
Value > 80
```

The script calculates the median hue, saturation, and value of the retained pixels.

The median is used to reduce the influence of isolated pixels and image noise.

The current hue ranges are:

| Result  |                                               Hue range |
| ------- | ------------------------------------------------------: |
| Blue    |                                               `100–130` |
| Green   |                                                 `45–85` |
| Yellow  |                                                 `18–40` |
| Unknown | Outside the configured ranges or no valid target pixels |

The classifier is rule-based.

It does not use:

* a trained neural network
* object detection
* instance segmentation
* automatic white-balance correction
* adaptive color calibration
* confidence scoring
* multi-frame voting

The implemented method is intentionally simple and sufficient for the controlled tabletop demonstration.

---

## Gate Selection

Each recognized color is mapped to a separate servo-node address.

```text
blue   → 0x13
green  → 0x16
yellow → 0x17
```

When a color is recognized, the orchestration script selects the corresponding gate.

If another gate was previously activated, that gate is returned to its open position before the newly selected gate is moved.

An `unknown` result does not map to a sorting-gate address.

---

## Gate Motion

The current gate positions are:

| State         | Servo command |
| ------------- | ------------: |
| Open position |         `90°` |
| Push position |        `180°` |

At startup, all three gate servos are moved to the configured open position.

When a classification result is produced, the selected servo is commanded toward the push position.

The displayed degree values are software command values.

The actual mechanical angle and gate travel depend on:

* servo calibration
* horn installation
* linkage geometry
* gate mounting
* mechanical limits
* firmware interpretation
* power-supply condition

The current implementation does not use gate-position feedback.

---

## Operation Sequence

```text
Initialize camera
      ↓
Move all gates to the open position
      ↓
Start conveyor
      ↓
Read workpiece sensor repeatedly
      ↓
Sensor value falls below threshold?
      ├─ No → continue polling
      └─ Yes
           ↓
      Wait for workpiece to reach camera area
           ↓
      Capture camera frame
           ↓
      Extract configured ROI
           ↓
      Convert ROI to HSV
           ↓
      Filter low-saturation and dark pixels
           ↓
      Calculate median HSV values
           ↓
      Classify as blue, green, yellow, or unknown
           ↓
      Select corresponding servo address
           ↓
      Return previously active gate if necessary
           ↓
      Activate selected gate
           ↓
      Apply cooldown
           ↓
      Resume sensor polling
```

---

## Timing Parameters

The current orchestration script uses the following timing parameters:

| Parameter                     |    Current value | Purpose                                            |
| ----------------------------- | ---------------: | -------------------------------------------------- |
| Sensor-update wait            |         `0.05 s` | Allows the sensor acquisition result to update     |
| Capture delay                 |          `1.8 s` | Allows the workpiece to move from sensor to camera |
| Detection cooldown            |          `1.0 s` | Reduces repeated handling of the same trigger      |
| Gate-change wait              |         `0.15 s` | Allows the previous gate to return                 |
| Gate-command wait             |         `0.05 s` | Separates open and push commands                   |
| Startup gate calibration wait | `0.3 s` per gate | Allows initial gate positioning                    |

These values are tuned for the current tabletop configuration and are not general system constants.

---

## Monitoring and Logging

The I2C Debugger can monitor the distributed nodes while the sorting orchestration script operates the process.

Available observations may include:

* motor-node lifecycle state
* servo-node lifecycle state
* sensor-node state
* command and feedback registers
* communication availability
* state-transition events
* periodic node snapshots
* voltage and current measurements
* calculated-power values
* monitoring-session context

The monitoring application observes exposed node state.

It does not infer every physical event directly.

For example, a servo command being accepted does not independently prove that a workpiece reached the intended destination.

---

## Grafana Visualization

Grafana reads persisted SQLite records generated by the I2C Debugger.

The sorting run can be investigated using:

* node-state timelines
* recent state-transition events
* ERROR and ESTOP history
* monitoring-session selection
* voltage, current, and calculated-power time series
* average and maximum recorded calculated-power comparison
* recorded maximum, minimum, and maximum-to-minimum ranges

This supports questions such as:

* When did the motor or servo state change?
* Did electrical load change during gate activation?
* Was a node missing or in an error state?
* Did repeated sorting runs show similar behavior?
* Which monitored channel showed the largest recorded load range?

Correlation between a logged state change and a physical result does not by itself prove causation.

---

## Validation Approach

The demonstration should be evaluated at multiple levels.

### Process Validation

Confirm that:

* the conveyor transports the workpiece
* the sensor detects the workpiece
* the workpiece reaches the camera area
* the intended color is classified
* the corresponding gate activates
* the workpiece is diverted toward the intended destination

### Interface Validation

Confirm that:

* each required I²C node responds
* the expected register commands are accepted
* sensor values can be read
* gate addresses map to the intended physical mechanisms
* monitoring can observe the participating nodes

### Historical Validation

Confirm that:

* a monitoring session is recorded
* node-state changes are persisted
* electrical snapshots are available
* Grafana can visualize the intended analysis period

---

## Demonstrated Result

The use case demonstrates that one process sequence can coordinate:

* continuous motor-driven transport
* sensor-triggered operation
* camera-based software classification
* destination selection
* multiple distributed servo nodes
* live system monitoring
* persisted event and measurement history

It also demonstrates that process-specific behavior can be implemented mainly through mechanism arrangement and host-side orchestration while reusing the common control and observation platform.

---

## Current Limitations

The current demonstration has the following limitations:

* color thresholds are manually configured
* classification assumes controlled camera placement and lighting
* the ROI is fixed relative to the image
* capture timing is based on a fixed delay
* conveyor position is not measured by an encoder
* the gate position is not measured
* the final destination is not independently confirmed
* unknown workpieces are not routed to a dedicated reject path
* repeated-trigger suppression is time-based
* I²C command errors are not recovered through a process-level retry strategy
* the orchestration script directly invokes Linux `i2cset` and `i2cget`
* the current mechanism is not designed for industrial speed, accuracy, or durability
* the demonstration does not implement production-grade guarding or safety control

These constraints are acceptable for the current tabletop architecture-validation scope.

---

## Implementation Note

The orchestration script is a demonstration implementation rather than a production process controller.

It currently contains direct register writes, fixed addresses, fixed timing parameters, and fixed classification thresholds.

Future refactoring could separate:

* register-access functions
* node configuration
* process sequencing
* image classification
* calibration values
* error handling
* result logging

into independently testable components.

The current script also contains an error path that references an undefined `angle` variable when no valid HSV target pixels are found.

This does not affect the architecture described by the use case, but the implementation should be corrected before treating the `unknown` path as fully validated.

---

## Possible Future Improvements

Possible improvements include:

* external configuration for I²C addresses and register values
* adaptive color calibration
* lighting normalization
* multi-frame classification
* explicit classification confidence
* a dedicated reject path for unknown workpieces
* a second sensor at the camera position
* a destination-confirmation sensor
* conveyor encoder feedback
* measured workpiece-position tracking
* gate-position feedback
* configurable timeouts and retry behavior
* structured logging from the orchestration script
* direct integration with the monitoring database
* process-level error and recovery states
* replacement of temporary fixtures with refined mounts where required
* 3D-printed camera, sensor, or gate brackets after geometry validation

---

## Reconfiguration Lessons

This demonstration shows which elements belong to the reusable platform and which elements remain process-specific.

```text
Reusable
├─ distributed I²C-node architecture
├─ motor, servo, and sensor firmware roles
├─ register-based interface
├─ monitoring
├─ SQLite persistence
└─ Grafana visualization

Process-specific
├─ conveyor geometry
├─ sensor position
├─ camera position
├─ color thresholds
├─ capture timing
├─ gate arrangement
├─ destination mapping
└─ orchestration sequence
```

A different sorting process could replace the camera classifier, mechanism, or destination arrangement without replacing the entire control and monitoring stack.

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
* Sensor Board → [`../../Hardware/Sensor_Board/`](../../Hardware/Sensor_Board/)
* Power Monitor Board → [`../../Hardware/Power_Monitor_Board/`](../../Hardware/Power_Monitor_Board/)
