# ADL-004: Distributed Node Architecture

## Purpose of This Document

This document explains the decision to use a distributed device-node architecture in the Modular Factory System.

The architecture assigns local actuator and sensor responsibilities to individual Raspberry Pi Pico-based nodes while retaining process-level coordination on the host platform.

This decision is evaluated in relation to the requirements, system context, development constraints, and host-platform decision defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)

This document focuses on the distribution of device-control responsibilities.

The detailed communication interface and common register specification are documented separately.

---

## Decision Summary

The system uses multiple Raspberry Pi Pico-based device nodes rather than connecting all actuators and sensors directly to the Raspberry Pi 5 host.

Each node owns a limited device-local responsibility.

Current node categories include:

* DC motor control
* servo control
* sensor acquisition

The host coordinates these nodes as a process but does not directly implement the low-level behavior of every connected device.

Conceptually:

```text
Raspberry Pi 5 host
        ↓
Process-level orchestration
        ↓
Common node interface
        ↓
Distributed device nodes
        ↓
Local actuator or sensor behavior
        ↓
Physical mechanism
```

The architecture was selected to reduce unnecessary change propagation, make responsibilities easier to identify, and allow device functions to be replaced or extended independently.

---

## Decision Context

The tabletop system contains multiple physical devices with different responsibilities.

Examples include:

* conveyor motors
* stamp-drive motors
* paper take-up motors
* sorting-gate servos
* paper-clamp servos
* photo sensors

A centralized implementation could connect these devices directly to one controller.

However, this would cause the central controller to own:

* process sequence
* low-level device behavior
* hardware-specific GPIO assignments
* actuator timing
* sensor acquisition
* device-specific validation
* process-specific decision logic

As the number and type of devices increased, these responsibilities would become increasingly coupled.

The system therefore needed a boundary between:

* process-level coordination
* device-local behavior

The distributed node architecture establishes this boundary.

---

## Architectural Goals

The distributed architecture was selected to support the following goals.

| ID  | Goal                                                       |
| --- | ---------------------------------------------------------- |
| N1  | Localize device-specific behavior                          |
| N2  | Reduce change propagation                                  |
| N3  | Allow modules to be replaced independently                 |
| N4  | Provide a common host-facing model                         |
| N5  | Support different combinations of devices                  |
| N6  | Improve debugging and fault isolation                      |
| N7  | Reuse shared firmware behavior                             |
| N8  | Keep process orchestration separate from low-level control |
| N9  | Support incremental addition of new device types           |
| N10 | Remain practical for tabletop-scale implementation         |

---

## N1: Localize Device-Specific Behavior

Each node should own behavior that is specific to its connected device.

Examples include:

* GPIO configuration
* PWM generation
* ADC acquisition
* actuator-specific parameter interpretation
* sensor-specific measurement handling
* local completion conditions
* local safe shutdown behavior

The host should not need to know how each GPIO or peripheral is physically implemented.

The host should instead interact with the node through an externally defined interface.

This allows the physical implementation of a node to change without necessarily changing the process-level orchestration.

---

## N2: Reduce Change Propagation

A change to one actuator or sensor should affect the smallest practical part of the system.

For example:

* changing motor-control behavior should not require modification of servo firmware
* changing sensor acquisition should not require redesign of host-side motor logic
* changing one device profile should not require rewriting the common lifecycle behavior
* changing a process sequence should not require rebuilding device-local control logic when the node interface remains unchanged

The architecture does not guarantee zero change propagation.

It provides boundaries intended to reduce unnecessary propagation.

---

## N3: Independent Module Replacement

A node should be replaceable without requiring reconstruction of the complete control system.

A practical replacement boundary includes:

* the Raspberry Pi Pico
* the node firmware
* the associated actuator or sensor interface
* the node configuration
* the connection to the host communication bus

This allows a failed or modified device function to be handled locally where practical.

The replacement boundary is particularly useful in an individually developed prototype where replacing a low-cost module may be more efficient than redesigning the complete system.

---

## N4: Common Host-Facing Model

Different device types should expose a sufficiently common model to the host.

The current firmware architecture provides shared behavior for:

* identification
* status
* commands
* parameters
* feedback
* lifecycle state
* staged parameter application
* operation start and stop
* completion reporting
* E-STOP handling

The host can therefore interact with different node categories without directly controlling their internal peripherals.

The common model does not require every device to behave identically.

Device-specific parameters, feedback, and operation modes remain possible behind the common boundary.

---

## N5: Different Device Combinations

The physical process should be constructed from the combination of nodes required by the current use case.

For example, one process may require:

* one motor node
* one sensor node
* several servo nodes

Another process may require:

* multiple motor nodes
* servo nodes
* no camera-controlled sorting gates

The architecture should not assume that every process uses the same device arrangement.

The host-side process definition determines how the available nodes are combined.

---

## N6: Debugging and Fault Isolation

A distributed architecture provides explicit boundaries for investigation.

When a process does not behave as expected, the system can be examined at several levels.

Questions can include:

* is the host communicating with the expected node?
* does the node expose the expected identification?
* did the node receive valid parameters?
* did the node enter BUSY?
* did the node publish feedback?
* did the node report completion?
* did the physical device actually perform the intended action?

This does not automatically identify every fault.

However, the node boundary reduces the number of responsibilities that must be considered at once.

---

## N7: Shared Firmware Behavior

The nodes are distributed physically and functionally, but they do not use completely independent firmware architectures.

The current implementation separates:

```text
Shared common firmware
        +
Node-specific implementation
        +
Selected node profile
```

The shared firmware owns common behavior such as:

* register handling
* staged parameter handling
* command validation
* lifecycle management
* RUN and STOP handling
* completion-state handling
* feedback-update signaling
* E-STOP lifecycle handling
* invocation of node-specific callbacks

The node-specific implementation owns:

* local hardware initialization
* device-specific parameter interpretation
* device-specific actuator or sensor behavior
* device-specific validation
* local feedback generation
* target-completion decisions
* safe local output shutdown

This structure avoids duplicating common communication and lifecycle logic across every node type.

---

## N8: Separation from Process Orchestration

Device nodes should not own the complete factory-process sequence.

For example, a motor node may know:

* how to drive a motor
* how to apply a requested mode
* when its local operation reaches completion

It should not need to know:

* which sorting gate should operate next
* which color was identified by the camera
* whether another unrelated actuator has completed
* the overall use-case sequence

These process-level decisions remain on the host.

This separation allows the same device node behavior to be reused in different process configurations.

---

## N9: Extension Through New Node Types

The architecture should allow another device category to be added without creating a completely independent firmware system.

A new node can be introduced by defining:

* a node-specific implementation
* a node profile
* supported capabilities
* device-specific callbacks
* required build configuration

The existing shared lifecycle and communication behavior can remain in the common firmware core when the new device fits the existing model.

If a genuinely new shared requirement is discovered, the common interface should be updated explicitly rather than hiding the behavior inside one node implementation.

---

## N10: Tabletop Practicality

The distributed architecture must remain physically practical at tabletop scale.

The current implementation uses compact Raspberry Pi Pico-based nodes that can be located near their associated devices.

This provides practical benefits such as:

* shorter local actuator and sensor wiring
* accessible module replacement
* visible physical responsibility boundaries
* incremental construction
* compact installation
* inexpensive spare controllers

The architecture is not intended to demonstrate large-distance industrial distributed control.

Communication distances remain within the assumptions of the tabletop prototype.

---

## Current Node Architecture

The current firmware architecture uses Raspberry Pi Pico devices as local nodes.

Each firmware image combines:

* the shared common firmware
* one selected node-specific implementation
* one selected node profile

Current implementations include:

| Node type   | Main local responsibility                |
| ----------- | ---------------------------------------- |
| Motor node  | DC motor-specific control behavior       |
| Servo node  | Servo-specific control behavior          |
| Sensor node | Sensor acquisition and feedback behavior |

The common firmware provides the host-facing lifecycle and register behavior.

Node-specific firmware converts that common model into physical device behavior.

---

## Responsibility Boundary

The main responsibility split is:

| Host responsibility                           | Device-node responsibility            |
| --------------------------------------------- | ------------------------------------- |
| Define process sequence                       | Execute local device behavior         |
| Coordinate multiple nodes                     | Control one local device function     |
| Apply process-specific decision logic         | Interpret device-local parameters     |
| Use camera or multi-node information          | Operate local GPIO and peripherals    |
| Decide when another process step should begin | Determine local completion conditions |
| Monitor exposed node state                    | Produce available local feedback      |
| Record and visualize observations             | Maintain local safe device behavior   |

This boundary is logical rather than absolute.

Some processing may move between the host and node if future requirements justify a different allocation.

---

## Common Core and Node-Specific Boundary

The firmware is internally divided between common and device-specific responsibilities.

### Common Core

The common core owns behavior that should remain consistent across node types.

This includes:

* register behavior
* command handling
* parameter staging
* lifecycle transitions
* completion-state handling
* E-STOP lifecycle behavior
* common feedback signaling
* node-profile integration

### Node-Specific Layer

The node-specific layer owns behavior that depends on the connected physical device.

This includes:

* PWM or peripheral initialization
* actuator output
* sensor acquisition
* parameter conversion
* operation-mode validation
* feedback interpretation
* local completion logic

Node-specific firmware should use the public common API rather than directly modifying shared lifecycle state.

---

## Current Runtime Model

The current node firmware uses:

* Raspberry Pi Pico SDK
* a cooperative polling loop
* common polling at approximately 1 ms
* node-specific periodic processing at approximately 10 ms
* short-distance local host communication
* node addressing based on a base address and a four-bit DIP setting

The current implementation is designed for tabletop prototyping.

It does not provide hard real-time guarantees.

The timing model is sufficient for the physical processes currently demonstrated.

---

## Risks and Mitigations

| Risk                                                 | Current mitigation                                                    |
| ---------------------------------------------------- | --------------------------------------------------------------------- |
| Too many independent firmware implementations        | Shared common firmware core                                           |
| Inconsistent node behavior                           | Common lifecycle and register model                                   |
| Node-specific code modifies shared state incorrectly | Public common API and callback boundary                               |
| Address conflicts                                    | Configurable address selection using base address and DIP setting     |
| Difficult node extension                             | Defined node profile and callback model                               |
| Communication fault affects process                  | Node state can be inspected independently from physical outcome       |
| One node failure stops part of a process             | Device responsibilities remain localized to replaceable nodes         |
| Common-core change affects all node types            | Common behavior is documented and separated from device-specific code |

---

## Considered Alternatives

### Alternative A: Direct Host GPIO Control

The Raspberry Pi 5 could directly control actuators and read sensors through its own GPIO and peripheral interfaces.

#### Advantages

* fewer controllers
* fewer firmware images
* simpler initial hardware count
* no separate host-to-node protocol required
* potentially lower component cost for a very small system

#### Disadvantages

* host becomes responsible for low-level device behavior
* GPIO allocation becomes tightly coupled to the complete process
* device replacement can affect host-side implementation
* wiring converges on one physical location
* process orchestration and device control become more tightly coupled
* failures or experiments can affect more of the system
* expansion becomes increasingly difficult as device count grows

#### Evaluation

Direct host control would be practical for a very small fixed demonstration.

It was not selected because the project required responsibility separation, reconfiguration, and reuse across different physical processes.

---

### Alternative B: Single Central Microcontroller

A single microcontroller could control all actuators and sensors while the Raspberry Pi 5 communicates only with that controller.

#### Advantages

* local control can be separated from the Linux host
* fewer distributed controllers
* centralized embedded timing behavior
* one firmware image can coordinate all local I/O
* reduced communication-node count

#### Disadvantages

* device-specific behavior becomes concentrated in one firmware image
* GPIO and peripheral allocation become process-dependent
* adding new mechanisms can require changes to central firmware
* one embedded-controller failure affects all attached devices
* replacement of one device type may require changes to unrelated code
* responsibility boundaries between device categories become less explicit

#### Evaluation

A central microcontroller would provide a useful separation between the host and physical I/O.

It was not selected because it would move many low-level responsibilities into another centralized component rather than localizing them by device function.

---

### Alternative C: Distributed Device Nodes

Each physical device function is assigned to a small local controller.

#### Advantages

* device-specific behavior remains local
* modules can be developed and tested independently
* hardware changes can remain localized
* common host-facing behavior can be reused
* different node combinations can form different processes
* fault investigation can be divided by node
* physical modules can be replaced independently
* node-specific firmware can extend a shared common core

#### Disadvantages

* more controllers are required
* each node requires firmware and configuration
* node identification and addressing must be managed
* a communication interface is required
* common firmware must remain compatible across node types
* system-level debugging must consider communication as an additional failure source

#### Evaluation

This approach provides the strongest fit for the modular tabletop architecture.

It was therefore selected.

---

### Alternative D: Commercial Smart I/O or Industrial Remote I/O

Commercial distributed I/O or smart actuator modules could provide standardized remote device interfaces.

#### Advantages

* established industrial hardware
* robust connectors and electrical interfaces
* standardized installation methods
* possible diagnostic support
* production-oriented lifecycle and maintenance models

#### Disadvantages

* higher cost
* larger physical size
* increased setup requirements
* capabilities beyond the current PoC
* reduced visibility into low-level implementation
* weaker fit with the goal of demonstrating custom hardware, firmware, and interface boundaries

#### Evaluation

Commercial distributed I/O would be a strong option for an industrial deployment.

It was not selected because the current project is intended to expose and examine the architecture from the physical device layer through the host software layer.

---

## Comparison Summary

| Alternative                    | Main strengths                                                        | Main limitations                                                | Fit for current PoC |
| ------------------------------ | --------------------------------------------------------------------- | --------------------------------------------------------------- | :-----------------: |
| Direct Raspberry Pi 5 GPIO     | Minimal controller count and simple initial setup                     | Strong coupling between host, wiring, and device behavior       |         Weak        |
| Single central microcontroller | Separates embedded I/O from Linux host                                | Concentrates device-specific responsibilities in one controller |       Moderate      |
| Distributed Pico nodes         | Localized responsibilities, replaceability, reusable common interface | More controllers and communication/configuration overhead       |        Strong       |
| Commercial remote I/O          | Robust and production-oriented distributed control                    | Cost, size, and abstraction beyond current PoC purpose          |         Weak        |

The ratings apply to the current project rather than to general industrial use.

---

## Selected Architecture

The distributed Raspberry Pi Pico node architecture was selected because it provides the best overall fit for:

* responsibility separation
* process reconfiguration
* localized changes
* module replacement
* fault isolation
* firmware reuse
* host-side abstraction
* tabletop physical integration
* individual development

The decision introduces additional node and communication management, but that complexity is directly related to the architectural questions the project is intended to explore.

---

## Consequences

### Positive Consequences

The selected architecture provides:

* clear separation between host orchestration and local device behavior
* reusable device-local controllers
* independent node development
* localized hardware and firmware changes
* easier replacement of failed modules
* reuse of common firmware behavior
* support for multiple process configurations
* explicit host-to-device boundaries
* practical extension to additional device categories

---

### Negative Consequences

The architecture also introduces:

* more microcontrollers
* more firmware builds
* more physical connections
* node-address management
* communication-bus dependency
* additional configuration
* more potential points of communication failure
* need for common-interface compatibility
* need to keep shared firmware and node-specific code correctly separated

---

### Context-Dependent Consequences

Distribution does not automatically improve every aspect of the system.

For example:

* a very small fixed machine may be simpler with one controller
* additional nodes increase hardware count
* centralized timing across many nodes may be more difficult
* bus failure can affect multiple otherwise independent modules

The value of distribution depends on the need for reconfiguration, replacement, responsibility separation, and incremental extension.

These needs are significant in the current project.

---

## Extension Boundary

The current distributed architecture can be extended when a new physical device is required.

A new node type should first be evaluated against the existing common model.

If the new device can use the current:

* lifecycle
* command model
* parameter staging
* feedback model
* completion paths
* callback contract

then only device-specific behavior and a new profile should be required.

If the new device requires a genuinely different common behavior, the shared architecture should be reconsidered explicitly.

The new requirement should not be hidden inside node-specific firmware simply to preserve the appearance of compatibility.

---

## Validation Through Current Implementation

The current implementation demonstrates the architecture through multiple device categories.

Evidence includes:

* shared firmware core used by different node types
* motor-specific node behavior
* servo-specific node behavior
* sensor-specific node behavior
* common host-facing register behavior
* shared lifecycle handling
* profile-based firmware builds
* node-specific callbacks
* reuse of distributed nodes across different physical use cases

The firmware documentation also defines a repeatable procedure for adding another node type without duplicating the common lifecycle and register implementation.

---

## Decision Boundary

This decision establishes:

* distribution of device-local responsibilities
* Raspberry Pi Pico-based local nodes for the current implementation
* separation between common node behavior and device-specific behavior
* separation between device control and host-side process orchestration

This decision does not establish:

* the detailed communication transport
* the detailed register layout
* the final allocation of every software responsibility
* automatic Plug & Play behavior
* production-scale distributed control
* hard real-time multi-node synchronization

Those concerns are addressed separately or remain outside the current proof-of-concept scope.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [Firmware Overview](../Firmware/)
* [Adding a New Node](../Firmware/common/docs/Adding_New_Node.md)
* [System Architecture](../Docs/System_Architecture/)
* [Register Map](../Docs/Register_Map/)
* [Use Cases](../Use_cases/)
