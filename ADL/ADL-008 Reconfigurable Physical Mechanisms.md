# ADL-008: Reconfigurable Physical Mechanisms

## Purpose of This Document

This document explains the decision to use reconfigurable, prototype-oriented physical mechanisms in the Modular Factory System.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)
* [ADL-007: Separation of Responsibilities](./ADL-007%20Separation%20of%20Responsibilities.md)

The earlier decisions establish modular electrical, firmware, communication, and software boundaries.

This document explains how the physical mechanism layer follows the same general principle without requiring a production-grade modular mechanical system.

---

## Decision Summary

The physical mechanisms are constructed as reconfigurable tabletop prototypes rather than as optimized single-purpose machines.

The current approach uses combinations of:

* commercially available construction-kit parts
* general-purpose plates
* regular mounting-hole patterns
* gears and shafts
* compact gearboxes
* small motors and servos
* simple brackets and spacers
* screws, nuts, and washers
* temporary fixtures
* tape, cable ties, or elastic attachments where appropriate
* custom parts only where necessary

The architecture prioritizes:

* physical rearrangement
* component replacement
* adjustment
* visibility
* rapid construction
* compatibility with the electrical module boundaries
* reuse of available parts
* tabletop dimensions

over:

* minimum size
* maximum rigidity
* maximum throughput
* precision positioning
* optimized cycle time
* production-grade durability
* dedicated tooling

The physical mechanism is treated as a replaceable process-specific layer.

---

## Decision Context

The Modular Factory System is intended to demonstrate multiple physical processes using a shared control and monitoring platform.

Current demonstrated processes include:

* color-based workpiece sorting
* repeated paper stamping

These processes require substantially different physical mechanisms.

The sorting process includes:

* a conveyor
* workpiece guides
* a photo-reflector position
* a camera position
* multiple sorting gates
* multiple destinations

The stamping process includes:

* a roll-paper path
* paper clamps
* a rack-and-pinion mechanism
* a physical stamp
* a paper take-up mechanism

A single dedicated machine structure would not naturally support both demonstrations.

The system therefore needed a physical construction strategy that allowed different mechanisms to be built around the same broader electrical and software architecture.

---

## Physical Reconfiguration Goals

The mechanism strategy was selected to support the following goals.

| ID   | Goal                                                                   |
| ---- | ---------------------------------------------------------------------- |
| PM1  | Allow process-specific mechanisms to change independently              |
| PM2  | Reuse general-purpose mechanical parts where practical                 |
| PM3  | Support rearrangement without manufacturing a new machine frame        |
| PM4  | Maintain compatibility with modular electrical control                 |
| PM5  | Keep mechanisms small enough for tabletop operation                    |
| PM6  | Make physical behavior visible and understandable                      |
| PM7  | Allow quick adjustment during proof-of-concept development             |
| PM8  | Prefer replaceable parts over highly optimized custom structures       |
| PM9  | Permit temporary fixtures during early validation                      |
| PM10 | Separate architectural requirements from exact commercial parts        |
| PM11 | Accept process-specific calibration after physical changes             |
| PM12 | Avoid unnecessary mechanical precision before it becomes a requirement |

---

## PM1: Independent Process-Specific Mechanisms

The physical mechanism belongs primarily to the use-case-specific layer.

For example:

```text
Reusable platform
├─ Raspberry Pi 5
├─ distributed Pico nodes
├─ common register interface
├─ motor control
├─ servo control
├─ sensor acquisition
├─ monitoring
└─ persistence

Process-specific physical layer
├─ conveyor and sorting gates
or
└─ clamps, rack-and-pinion, stamp, and paper feed
```

Changing from one process to another should not require redesigning the entire control and monitoring architecture.

The mechanism may change substantially while retaining compatible:

* actuator-node concepts
* sensor-node concepts
* electrical interfaces
* host-side coordination boundaries

---

## PM2: General-Purpose Mechanical Parts

The mechanisms use commercially available parts according to their useful physical properties rather than only their original intended purpose.

Relevant properties include:

* dimensions
* mounting-hole patterns
* stiffness
* motion type
* gear ratio
* available travel
* ease of attachment
* replaceability
* availability

For example, commercially available educational construction components are used as:

* conveyor structures
* machine bases
* actuator supports
* camera supports
* linear-motion mechanisms

The original product category does not determine the architectural role of a component.

The useful physical behavior determines whether it is appropriate for the prototype.

---

## PM3: Rearrangement Without a Dedicated Frame

The system avoids requiring a dedicated welded, machined, or otherwise permanently fabricated machine frame for every use case.

General-purpose plates and arms with repeated mounting-hole patterns allow components to be positioned and repositioned.

This supports changes such as:

* actuator position
* sensor position
* camera position
* support height
* gate position
* paper path
* gearbox position
* mechanism spacing

Not every modification can be performed without fabrication.

The objective is to reduce the number of changes that require a completely new mechanical structure.

---

## PM4: Compatibility with Modular Electrical Control

Mechanical reconfiguration remains useful only if the electrical control architecture can follow it.

The physical mechanism is therefore kept conceptually separate from:

* Controller Boards
* DC Motor Boards
* Servo Boards
* Sensor Boards
* distributed node firmware
* host orchestration

A mechanism can use a motor, servo, or sensor through the corresponding control module without requiring the electrical architecture to reproduce the exact mechanical arrangement.

Conceptually:

```text
Mechanical function
      ↓
Actuator or sensor
      ↓
Electrical interface board
      ↓
Pico node
      ↓
Common host interface
```

The mechanism can change while preserving this general boundary.

---

## PM5: Tabletop Scale

The physical system must remain suitable for tabletop development and demonstration.

This constrains:

* overall dimensions
* actuator size
* structural materials
* travel distance
* workpiece size
* mounting methods
* wiring length
* available access space

The current mechanisms use small motors, servos, gears, plates, and workpieces that fit the tabletop environment.

The objective is not to miniaturize every component as far as possible.

Enough physical space is retained for:

* access
* observation
* adjustment
* wiring
* replacement
* explanation

---

## PM6: Visible Physical Behavior

The project is intended to make process behavior observable.

The mechanism therefore should make it possible to see:

* material movement
* actuator motion
* sensor location
* gate behavior
* clamping
* stamping
* paper advancement

This visibility helps connect:

```text
software command
      ↓
device response
      ↓
physical process
```

The mechanism is not hidden inside an optimized enclosure.

This supports inspection and explanation during development and demonstration.

---

## PM7: Adjustment During Development

Prototype mechanisms often require iterative adjustment.

Examples from the demonstrated processes include:

* sensor position
* sensor threshold
* camera position
* image region of interest
* capture delay
* sorting-gate geometry
* servo angles
* paper path
* clamp positions
* press travel
* press hold time
* paper-advance timing

Some of these values are software parameters.

Others depend directly on physical arrangement.

The physical design therefore allows adjustment rather than assuming that the first geometry will remain final.

---

## PM8: Replaceable Parts

The mechanism favors components that can be replaced without reconstructing the entire process.

Examples include:

* small servos
* commercially available gearboxes
* construction plates
* gears
* shafts
* mounting arms
* workpieces
* process tools

Replacement may still require:

* mechanical adjustment
* parameter changes
* recalibration
* firmware configuration
* host-side configuration

Replaceability does not imply perfect interchangeability.

The goal is to avoid unnecessary dependence on one irreplaceable physical component.

---

## PM9: Temporary Fixtures

Early proof-of-concept validation does not always justify designing a dedicated fixture.

The current mechanisms may therefore use temporary attachment methods such as:

* tape
* cable ties
* elastic bands
* simple spacers
* readily available brackets

For example, the current stamp is attached to the moving rack-and-pinion member using rubber bands.

This allows:

* rapid installation
* removal
* position adjustment
* reuse of the commercial stamp without modifying it
* validation before designing a dedicated holder

This choice deliberately trades:

* rigidity
* repeatable positioning
* controlled contact force
* long-term durability

for lower fabrication effort during proof-of-concept development.

---

## PM10: Architecture Independent of Exact Commercial Parts

The demonstrated mechanisms use specific commercially sourced parts.

Those parts provide reproducible implementation references, but most are not architectural requirements.

For example, the sorting mechanism currently uses commercially available:

* crawler and sprocket parts
* gearboxes
* universal plates
* universal arms
* SG90 servos
* a USB webcam

The stamping mechanism uses commercially available:

* rack-and-pinion components
* universal plates
* gearboxes
* a stationery stamp
* roll paper

The architecture should instead depend on functional characteristics such as:

* required motion
* usable dimensions
* mounting compatibility
* actuator requirements
* sensor requirements
* workpiece compatibility

A replacement part may be used if those required properties remain sufficient.

---

## PM11: Reconfiguration Requires Recalibration

Physical reconfiguration is not assumed to be free of side effects.

Changing the physical arrangement may require changes to:

* timing values
* servo command values
* sensor thresholds
* camera position
* image thresholds
* mechanism alignment
* workpiece guides
* paper path
* actuator limits

For example, the sorting demonstration currently uses a time-based delay between workpiece detection and camera capture.

Changing:

* conveyor speed
* sensor position
* workpiece geometry
* camera position

may require recalibrating that delay.

Similarly, changing the paper material or mechanism geometry in the stamping process may require adjusting:

* clamp geometry
* paper advancement
* press travel
* process timing

The architecture supports reconfiguration.

It does not claim automatic mechanical self-calibration.

---

## PM12: Precision Only When Required

The system does not introduce production-grade mechanical precision without a demonstrated requirement.

The current processes do not require:

* precision linear bearings
* closed-loop axis control
* high-accuracy encoders
* force-controlled pressing
* precision tool alignment
* machined production frames

For example, the current stamping mechanism does not measure:

* stamp position
* contact force
* resulting mark quality

Likewise, the current sorting gates do not use direct gate-position feedback.

Introducing these capabilities would increase:

* hardware
* fabrication
* calibration
* software
* integration effort

They should be added if a future proof-of-concept question requires them rather than by default.

---

## Current Physical Configurations

### Sorting Demonstration

The sorting mechanism currently includes:

* crawler-based conveyor
* geared DC motor drive
* universal mounting plate
* universal structural arms
* photo-reflector
* USB camera
* three SG90 sorting-gate servos
* small colored workpieces

The mechanism demonstrates:

```text
transport
→ detection
→ classification
→ gate selection
→ physical diversion
```

The exact mechanical products are implementation choices rather than mandatory elements of the architecture.

---

### Stamp Process Demonstration

The stamping mechanism currently includes:

* roll paper
* paper path
* two servo-operated clamps
* DC motor-driven rack-and-pinion mechanism
* commercial stamp
* second DC motor for paper take-up
* universal plates and mounting components

The mechanism demonstrates:

```text
clamp
→ press
→ hold
→ release
→ paper advance
→ repeat
```

The physical construction differs substantially from the sorting system while reusing the broader control, monitoring, and persistence architecture.

---

## Physical and Logical Reconfiguration

The project distinguishes two types of reconfiguration.

### Physical Reconfiguration

Examples include:

* moving a sensor
* changing a conveyor
* replacing a gate
* repositioning a camera
* changing a gearbox
* modifying a paper path
* replacing a process tool

### Logical Reconfiguration

Examples include:

* changing node assignment
* changing process sequence
* changing timing
* changing thresholds
* changing operation parameters
* changing destination mapping

A complete process change may require both.

Conceptually:

```text
Physical arrangement
        +
Node configuration
        +
Process parameters
        +
Host orchestration
        =
Demonstrated process configuration
```

---

## Reconfiguration Boundary

The intended reconfiguration boundary is not the individual screw or mechanical part.

The useful boundary is a functional process element.

Examples include:

* conveyor function
* sensing position
* gate function
* clamp function
* press function
* material-feed function

These elements may internally use different mechanisms while retaining a similar system-level responsibility.

This avoids tying the architecture to one detailed mechanical implementation.

---

## Risks and Mitigations

| Risk                                                   | Current mitigation                                                                               |
| ------------------------------------------------------ | ------------------------------------------------------------------------------------------------ |
| Prototype mechanism lacks rigidity                     | Use sufficient structural plates and supports for demonstrated operation                         |
| Temporary fixtures move or loosen                      | Keep mechanisms accessible for inspection and adjustment                                         |
| Reconfiguration changes calibration                    | Treat timing, thresholds, positions, and geometry as configuration-specific                      |
| Commercial part becomes unavailable                    | Architecture depends on functional properties rather than exact product identity where practical |
| Mechanism becomes too customized                       | Prefer general-purpose construction parts before dedicated fabrication                           |
| General-purpose parts limit performance                | Accept limitations while current proof-of-concept goals remain satisfied                         |
| Mechanism change affects electrical control            | Maintain actuator/sensor interface boundaries through modular boards                             |
| Physical outcome differs from software-visible state   | Use direct observation or additional sensing when physical proof is required                     |
| Too much mechanical optimization consumes project time | Add precision or custom fabrication only when required by the validation goal                    |
| Replacement component behaves differently              | Revalidate geometry, calibration, and process parameters after replacement                       |

---

## Comparison Summary

| Alternative                                 | Main strengths                                         | Main limitations                                            | Fit for current PoC |
| ------------------------------------------- | ------------------------------------------------------ | ----------------------------------------------------------- | :-----------------: |
| Fully custom fixed mechanism                | High structural optimization and repeatability         | High redesign and fabrication cost when process changes     |         Weak        |
| General-purpose reconfigurable construction | Fast modification, accessible parts, visible structure | Lower rigidity and precision                                |        Strong       |
| Mostly 3D-printed custom mechanism          | Geometry can be tailored to the process                | Requires design and fabrication iteration for many changes  |       Moderate      |
| Commercial automation modules               | Robust and production-oriented                         | Cost, size, and abstraction exceed current PoC requirements |         Weak        |
| Minimal temporary mock-up only              | Very fast initial validation                           | Low repeatability and weak long-term structure              |       Moderate      |

The ratings describe suitability for the current tabletop proof of concept.

They are not general rankings of mechanical design methods.

---

## Considered Alternatives

### Alternative A: Fully Custom Fixed Mechanism

A dedicated structure could be designed specifically for each process.

This could include:

* custom frame members
* dedicated actuator mounts
* fixed sensor brackets
* dedicated guides
* machined or fabricated mechanism components

#### Advantages

* geometry can be optimized for the process
* greater rigidity
* better repeatability
* cleaner packaging
* easier control of alignment and mechanical tolerances
* potential for higher cycle performance

#### Disadvantages

* greater design effort
* fabrication is required before system-level testing
* mechanism changes can require redesign
* process experiments become more expensive
* reuse across different demonstrations is reduced
* mechanical optimization may dominate development time

#### Evaluation

This approach would be suitable when the physical mechanism itself is the primary engineering target.

It was not selected because the current project focuses on validating system integration and architectural boundaries across multiple processes.

---

### Alternative B: General-Purpose Reconfigurable Construction

The process is built using reusable plates, arms, gearboxes, actuators, and simple fixtures.

#### Advantages

* rapid construction
* easier repositioning
* easier replacement
* common parts can be reused
* mechanisms remain physically visible
* limited fabrication requirement
* strong compatibility with tabletop development
* supports iterative changes

#### Disadvantages

* lower rigidity
* lower precision
* larger structures than optimized custom mechanisms
* more temporary or improvised mounting
* calibration may change after rearrangement
* appearance is less production-like

#### Evaluation

This approach provides the strongest fit for the current system.

It was selected.

---

### Alternative C: Primarily 3D-Printed Custom Mechanisms

Most mechanical parts could be designed digitally and printed for each configuration.

#### Advantages

* geometry can be tailored to the process
* custom mounts can be produced
* compact integrated structures are possible
* designs can be reproduced digitally
* useful transition between temporary prototypes and custom fabrication

#### Disadvantages

* CAD design effort is required
* every geometry change may require another design and print cycle
* printer capability and material properties become dependencies
* dimensional accuracy and strength depend on the process and material
* unnecessary parts may be designed before the mechanism is validated

#### Evaluation

3D printing is useful when a specific custom geometry becomes necessary.

It was not adopted as a requirement for the overall mechanism strategy because commercially available and simple construction parts were sufficient for the demonstrated processes.

Custom parts remain an available extension rather than the default construction method.

---

### Alternative D: Commercial Automation Modules

The system could use commercially available:

* conveyors
* industrial actuators
* linear stages
* machine frames
* sensor mounts
* industrial handling modules

#### Advantages

* robust construction
* known performance characteristics
* better mechanical repeatability
* production-oriented mounting
* industrial documentation
* potential integration with established machine systems

#### Disadvantages

* higher cost
* larger physical size
* reduced tabletop accessibility
* additional procurement requirements
* production capability exceeds current validation needs
* some low-level mechanical decisions become hidden inside purchased modules

#### Evaluation

Commercial automation equipment would become more appropriate when validating production deployment.

It was not selected for the current architecture because the project benefits from inexpensive, visible, and modifiable physical mechanisms.

---

### Alternative E: Minimal Temporary Mock-Up

The physical process could be assembled almost entirely through temporary fixtures without reusable structural components.

#### Advantages

* fastest initial setup
* low fabrication effort
* useful for testing one isolated physical idea
* inexpensive

#### Disadvantages

* poor repeatability
* difficult to maintain alignment
* weak support for repeated demonstrations
* harder to compare changes consistently
* physical responsibilities may become unclear
* adjustments may unintentionally change unrelated geometry

#### Evaluation

Temporary fixtures are useful within the selected prototype strategy.

However, using them as the complete structural strategy would reduce repeatability too far.

The current system therefore combines reusable general-purpose structure with temporary fixtures where they provide clear development value.

---

## Selected Physical Mechanism Strategy

The selected approach uses general-purpose, reconfigurable physical construction supported by temporary or custom fixtures where necessary.

The approach provides the strongest overall fit for:

* tabletop size
* iterative development
* multiple use cases
* low fabrication burden
* visible process behavior
* replaceability
* reuse
* adjustment
* individual development

The mechanism is optimized for architectural experimentation rather than production performance.

---

## Consequences

### Positive Consequences

The selected approach provides:

* lower fabrication effort
* faster physical iteration
* easier rearrangement
* reusable construction components
* visible mechanism behavior
* easier component replacement
* compatibility with multiple use cases
* easier access for debugging
* reduced dependency on dedicated manufacturing equipment

---

### Negative Consequences

The approach also introduces:

* lower mechanical rigidity
* lower positional repeatability
* temporary fixtures
* larger structures than optimized custom designs
* calibration changes after rearrangement
* less polished mechanical packaging
* more dependence on manual setup
* limited suitability for high-speed or high-force operation

---

### Context-Dependent Consequences

The current mechanical strategy is appropriate because the mechanism is primarily a means of testing system behavior.

If future requirements shift toward mechanical performance, the construction strategy may need to change.

Examples include:

| Future requirement         | Possible change                                          |
| -------------------------- | -------------------------------------------------------- |
| Higher positional accuracy | Rigid custom mounts, linear guides, or position feedback |
| Higher repeatability       | Dedicated fixtures and homing mechanisms                 |
| Higher force               | Stronger frame and actuator selection                    |
| Higher cycle speed         | Optimized motion system and reduced mechanism mass       |
| Automatic setup            | Homing and position sensing                              |
| Production durability      | Industrial structural and actuator components            |
| Controlled process force   | Force sensor and closed-loop actuation                   |
| Compact packaging          | Dedicated mechanical design                              |

The electrical and software responsibility boundaries can still remain useful even if the physical implementation becomes more production-oriented.

---

## Validation Through Current Implementation

The physical mechanism strategy is demonstrated through two substantially different process configurations.

The sorting demonstration uses:

* conveyor transport
* workpiece sensing
* camera observation
* three servo gates

The stamping demonstration uses:

* roll-paper handling
* servo clamping
* rack-and-pinion press motion
* paper take-up

Both use different physical structures while reusing major parts of the same:

* host platform
* distributed control architecture
* actuator-node concept
* firmware infrastructure
* communication interface
* monitoring system
* persistence layer
* visualization layer

This demonstrates that the physical mechanism can change significantly without requiring the complete system architecture to be rebuilt.

---

## Decision Boundary

This decision establishes:

* prototype-oriented reconfigurable physical construction
* preference for general-purpose mechanical parts
* acceptance of temporary fixtures where appropriate
* physical adjustment as part of proof-of-concept development
* separation of exact commercial products from architectural requirements
* recalibration after relevant physical changes

This decision does not establish:

* standardized industrial machine framing
* automatic mechanical Plug & Play
* automatic calibration
* precision positioning
* force control
* production durability
* certified guarding
* production-speed mechanisms
* universal interchangeability of mechanical components

The separation of monitoring and direct control is addressed in the next architectural decision.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)
* [ADL-007: Separation of Responsibilities](./ADL-007%20Separation%20of%20Responsibilities.md)
* [Hardware Overview](../Hardware/)
* [Sorting Demo](../Use_cases/Amazon-style_Sorting_Demo/)
* [Stamp Process Demo](../Use_cases/Stamp_Process_Demo/)
* [System Architecture](../Docs/System_Architecture/)
