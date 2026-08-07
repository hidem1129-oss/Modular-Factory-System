# ADL-006: Common Register Interface

## Purpose of This Document

This document explains the decision to use a common register-based host interface across Raspberry Pi Pico device nodes in the Modular Factory System.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)

ADL-004 establishes distributed device nodes.

ADL-005 establishes I²C as the current communication transport.

This document defines the interface model exposed through that transport.

The detailed address and bit definitions remain in the register-map documentation and firmware source.

---

## Decision Summary

All current Raspberry Pi Pico device nodes expose a shared register-based host interface.

The common register window occupies:

```text
0x00–0x3F
```

The interface provides common regions for:

* identification
* module and profile information
* capability information
* lifecycle state
* command results
* E-STOP information
* completion state
* feedback values
* configuration values
* operation mode
* setpoints
* timing values
* limits
* command requests

The design intentionally combines:

* fixed common semantics where host behavior should remain consistent
* profile-defined semantics where device-specific interpretation is necessary
* reserved fields for future extension

The goal is not to force all devices to behave identically.

The goal is to provide a stable host-facing structure while allowing device-specific behavior behind that structure.

---

## Decision Context

The distributed architecture contains different device categories.

Current examples include:

* DC motor nodes
* servo nodes
* sensor nodes

These devices have different physical behaviors.

A motor may require:

* speed or direction parameters
* timing
* completion conditions

A servo may require:

* target position
* movement parameters

A sensor may require:

* acquisition behavior
* measurement feedback

If every node defined a completely independent host protocol, host software would need device-specific communication logic for each node type.

For example:

```text
Motor node
→ motor-specific commands
→ motor-specific status
→ motor-specific feedback format

Servo node
→ servo-specific commands
→ servo-specific status
→ servo-specific feedback format

Sensor node
→ sensor-specific commands
→ sensor-specific status
→ sensor-specific feedback format
```

This would weaken the responsibility boundary established by the distributed-node architecture.

The host would still need detailed knowledge of every node implementation.

The common register interface therefore introduces a shared external model.

---

## Interface Goals

The common interface was designed to support the following goals.

| ID    | Goal                                                                 |
| ----- | -------------------------------------------------------------------- |
| RGI1  | Provide consistent node identification                               |
| RGI2  | Provide common lifecycle observation                                 |
| RGI3  | Provide common command handling                                      |
| RGI4  | Separate transport from device semantics                             |
| RGI5  | Reduce host-side device-specific communication logic                 |
| RGI6  | Support profile-specific device behavior                             |
| RGI7  | Support parameter staging before execution                           |
| RGI8  | Provide explicit command results                                     |
| RGI9  | Support monitoring and debugging                                     |
| RGI10 | Reserve extension space without immediately implementing every field |
| RGI11 | Support node replacement and addition                                |
| RGI12 | Remain simple enough for individual development                      |

---

## RGI1: Consistent Node Identification

The host should be able to inspect a node before interpreting its device-specific behavior.

The common interface therefore exposes identification fields such as:

* protocol signature
* module type
* module ID
* hardware revision
* firmware revision
* profile ID
* capability flags

This allows host-side software to distinguish between:

* a reachable node
* a node with the expected interface
* a node category
* a selected node profile
* advertised capabilities

The current protocol signature is represented through:

* `SIGNATURE0`
* `SIGNATURE1`

The current implementation initializes these to:

```text
0xC0
0xDE
```

This gives the host a basic way to determine whether the responding device exposes the expected register interface.

---

## RGI2: Common Lifecycle Observation

Different device types should expose a consistent high-level lifecycle where practical.

The common interface therefore provides shared state information such as:

* READY
* BUSY
* ESTOP
* DATA_READY
* completion state
* last command
* last command result

The lifecycle model does not describe the physical details of every node.

For example:

* READY does not mean that a motor and sensor perform the same function
* BUSY does not define the same internal operation for every profile
* completion may have different physical meaning for different devices

The shared lifecycle provides a common external coordination model.

---

## RGI3: Common Command Handling

The host should not require a completely different basic command mechanism for each node type.

The common interface therefore provides shared command concepts including:

* RUN
* STOP behavior through RUN state handling
* `LATCH_APPLY`
* E-STOP reset request

These common commands control lifecycle transitions.

Device-specific logic is invoked after common validation.

This allows common firmware to reject requests such as:

* RUN while already BUSY
* RUN while E-STOP is active
* RUN before parameters are latched
* invalid state transitions

Device-specific validation remains the responsibility of the selected node implementation.

---

## RGI4: Transport and Semantic Separation

I²C defines how bytes are transferred.

The register interface defines how those bytes are organized and interpreted.

These concerns are separate.

Conceptually:

```text
Host-side logic
      ↓
Common register semantics
      ↓
I²C register transactions
      ↓
Node common firmware
      ↓
Node-specific behavior
```

The current interface is implemented over I²C, but the meaning of:

* READY
* BUSY
* `LATCH_APPLY`
* feedback values
* command results

does not inherently depend on I²C.

This separation preserves the possibility of changing the transport without redefining every node responsibility.

---

## RGI5: Reduced Host-Side Communication Logic

A common register structure allows host tools to perform generic operations across node types.

Examples include:

* read signature
* read module type
* read profile ID
* inspect lifecycle state
* inspect command result
* write staged parameters
* issue `LATCH_APPLY`
* issue RUN
* inspect completion
* read feedback
* inspect update information

The host may still need profile-specific interpretation.

However, it does not need to implement a completely independent transport protocol for every device category.

---

## RGI6: Profile-Specific Behavior

A common interface must not erase meaningful differences between physical devices.

The current architecture therefore allows profile-defined interpretation.

Examples include:

* operation modes
* setpoint meaning
* feedback meaning
* capability flags
* profile-extension fields

The common interface currently provides three 16-bit feedback channels:

* `FB_VALUE0`
* `FB_VALUE1`
* `FB_VALUE2`

Their exact meaning is defined by the active node profile.

Similarly, not every node type must use every common parameter field.

The common structure therefore acts as an envelope rather than a rigid universal device model.

---

## RGI7: Parameter Staging

Physical actuation should not begin from partially updated multi-byte parameters.

The current common interface therefore stages selected 16-bit command parameters.

These include:

* `SETPOINT0`
* `SETPOINT1`
* `TARGET_TIME`
* `HOLD_TIME`
* `LIMIT0`
* `LIMIT1`

The current write sequence is:

```text
Write high byte
      ↓
Write low byte
      ↓
Commit staged 16-bit value
      ↓
Clear LATCHED
      ↓
Increment UPDATE_CNT
      ↓
Set DATA_READY
```

The host must then issue:

```text
LATCH_APPLY
```

before starting operation with the modified parameters.

This separates:

* parameter transport
* parameter activation
* operation start

---

## RGI8: Explicit Command Results

A successful I²C transaction does not necessarily mean that the requested operation was accepted.

For example:

* the node may be BUSY
* E-STOP may be active
* parameters may not be latched
* current state may reject the request
* device-specific validation may fail

The common interface therefore exposes command-result information.

Current result categories include:

| Result                    | Meaning                                                 |
| ------------------------- | ------------------------------------------------------- |
| `CMDRES_OK`               | Command accepted or completed successfully              |
| `CMDRES_REJECT_BUSY`      | Rejected because the node is BUSY                       |
| `CMDRES_REJECT_UNLATCHED` | RUN rejected because parameters are not latched         |
| `CMDRES_REJECT_ESTOP`     | Rejected because E-STOP is active                       |
| `CMDRES_REJECT_STATE`     | Rejected by current state or device-specific validation |
| `CMDRES_ACCEPTED_NOOP`    | Accepted but no state change was required               |

This distinguishes:

```text
Transport succeeded
```

from:

```text
Requested operation was accepted
```

---

## RGI9: Monitoring and Debugging

The register model should support inspection without requiring internal firmware access.

Useful common information includes:

* identification
* status
* last command
* last command result
* completion state
* feedback
* update counter
* E-STOP source

This allows host-side debugging tools to inspect nodes through the same interface used by normal host software.

The interface does not prove the final physical outcome.

It exposes firmware-level state and feedback that can be compared with independently observed physical behavior.

---

## RGI10: Reserved Extension Space

The register map includes fields that are not fully implemented by the current common API.

Examples include:

* unit metadata fields
* state extension flags
* feedback sequence fields
* feedback flags
* configuration values
* configuration flags
* extension values
* immediate-command fields

These fields preserve address space for future extension.

Their presence does not mean that they currently provide implemented functionality.

A field must not be treated as supported merely because storage exists at the corresponding register address.

---

## RGI11: Node Replacement and Addition

A common host-facing interface reduces the amount of software that must change when nodes are replaced or extended.

A compatible replacement node can retain:

* common identification layout
* common lifecycle behavior
* common command behavior
* common parameter structure
* common feedback access model

The node may still differ in:

* profile
* capabilities
* operation modes
* physical behavior
* feedback interpretation

The goal is compatible architectural structure rather than transparent interchangeability between unrelated devices.

---

## RGI12: Individual Development Practicality

The interface needed to remain simple enough to:

* implement in Pico firmware
* inspect manually
* document completely
* debug through ordinary I²C tools
* use from Python host software
* extend without a specialized middleware stack

A compact 64-byte register window provides a sufficiently small model for the current proof of concept.

The design avoids requiring:

* schema servers
* message brokers
* generated RPC frameworks
* complex serialization
* device-specific middleware

---

## Current Register Structure

The current common register window is:

```text
0x00–0x3F
```

The logical regions are:

| Address range | Responsibility                                                              |
| ------------- | --------------------------------------------------------------------------- |
| `0x00–0x0F`   | Identification, revision, profile, and capability information               |
| `0x10–0x1F`   | Runtime status, command results, E-STOP, completion, and update information |
| `0x20–0x2F`   | Feedback and profile-related configuration                                  |
| `0x30–0x3F`   | Commands, setpoints, timing values, and limits                              |

Conceptually:

```text
0x00 ───────────────────────────
      Identity / capabilities
0x10 ───────────────────────────
      Runtime state / completion
0x20 ───────────────────────────
      Feedback / configuration
0x30 ───────────────────────────
      Commands / parameters
0x3F ───────────────────────────
```

This organization gives host software a predictable location for common categories of information.

---

## Access Model

The interface includes several access patterns.

### Read-Only

Used for values primarily produced by firmware, such as:

* identification
* status
* command result
* feedback
* completion

### Read / Write

Used for host-controlled values such as:

* operation mode
* configuration values
* command parameters

### Write-One-to-Clear

The current `STATUS` behavior uses write-one-to-clear semantics for `DATA_READY`.

### Staged Write

Selected 16-bit parameters are written into staging storage before activation.

---

## 16-Bit Value Handling

The current common interface uses:

```text
High byte
Low byte
```

for 16-bit register values.

For staged parameters:

* high byte is written first
* low byte commits the full value

This gives the firmware a defined point at which a new 16-bit value becomes visible in the register window.

The active runtime configuration is still updated separately through `LATCH_APPLY`.

---

## Update Signaling

The common interface provides:

* `DATA_READY`
* `UPDATE_CNT`

These indicate that relevant common state or published information has changed.

They are not lifecycle states.

`UPDATE_CNT` provides a monotonically changing indication of common updates within its implemented counter behavior.

`DATA_READY` provides a host-visible update flag and is currently cleared through write-one-to-clear behavior.

These mechanisms allow host software to distinguish:

* unchanged repeated polling
* newly published or changed state

---

## Common and Profile-Specific Boundary

The register model intentionally separates common and profile-defined responsibilities.

| Common responsibility       | Profile-specific responsibility         |
| --------------------------- | --------------------------------------- |
| Signature format            | Device category                         |
| Lifecycle model             | Physical operation                      |
| Command-result model        | Device-specific validation              |
| Parameter staging mechanism | Parameter interpretation                |
| Feedback register locations | Feedback meaning                        |
| Capability representation   | Supported capabilities                  |
| Completion reporting        | Physical completion criteria            |
| Common command handling     | Local actuator or sensor implementation |

This prevents either extreme:

```text
Everything is device-specific
```

or:

```text
Every device is forced into identical semantics
```

The architecture instead uses a stable common envelope with profile-specific interpretation.

---

## Risks and Mitigations

| Risk                                                  | Current mitigation                                                           |
| ----------------------------------------------------- | ---------------------------------------------------------------------------- |
| Common interface becomes too generic                  | Profile-specific operation modes and feedback semantics remain available     |
| Common interface becomes too device-specific          | Shared fields are limited to cross-node lifecycle and data concepts          |
| Host assumes every field is supported                 | Capability and profile information identify supported behavior               |
| Partially written parameters become active            | Staged 16-bit write and `LATCH_APPLY` model                                  |
| Successful transport is mistaken for accepted command | Explicit last-command result                                                 |
| New node does not fit existing model                  | Common specification must be reconsidered rather than hiding incompatibility |
| Reserved fields are mistaken for implemented features | Documentation distinguishes implemented and reserved behavior                |
| Common-core change affects all nodes                  | Common interface is centralized and documented                               |
| Host depends on internal firmware behavior            | Host interacts through documented public register semantics                  |

---

## Comparison Summary

| Alternative                                      | Main strengths                                                  | Main limitations                                          | Fit for current PoC |
| ------------------------------------------------ | --------------------------------------------------------------- | --------------------------------------------------------- | :-----------------: |
| Independent register map per node type           | Maximum device-specific flexibility                             | Host must understand a different interface for every node |         Weak        |
| Common register map with profile-specific fields | Stable host interface with controlled device-specific extension | Requires careful boundary design                          |        Strong       |
| Message-based custom protocol                    | Flexible payloads and extensibility                             | Requires framing, parsing, and command schema design      |       Moderate      |
| RPC or service-style interface                   | Explicit operations and higher-level semantics                  | Additional software stack and implementation complexity   |         Weak        |
| Direct device-specific host code                 | Minimal abstraction for first implementation                    | Strong coupling between host and device internals         |         Weak        |

The comparison applies to the current tabletop proof of concept.

---

## Considered Alternatives

### Alternative A: Independent Register Map per Node Type

Each device category could expose its own independent address layout.

For example:

```text
Motor register map
Servo register map
Sensor register map
```

#### Advantages

* each map can exactly match device needs
* little unused register space
* device-specific concepts can be represented directly
* no need to design a common abstraction

#### Disadvantages

* host software must know each map
* monitoring software becomes device-specific
* lifecycle behavior may drift between implementations
* adding a node type requires additional host protocol logic
* common debugging tools become harder to build
* device replacement may propagate changes into host software

#### Evaluation

This approach maximizes local flexibility but weakens the architectural boundary between host and node.

It was not selected.

---

### Alternative B: Common Register Map with Profile-Specific Interpretation

All nodes expose the same structural register window while selected fields remain profile-defined.

#### Advantages

* common host-side access model
* consistent identification
* consistent lifecycle
* reusable monitoring tools
* shared command handling
* shared parameter-staging mechanism
* controlled profile-specific extension
* easier addition of new node types
* stronger separation between host and device internals

#### Disadvantages

* some fields may be unused by certain profiles
* common abstractions require design discipline
* a badly chosen common field can constrain future node types
* compatibility must be maintained across common-core changes
* host must still understand profile-specific meanings

#### Evaluation

This approach provides the strongest balance between common behavior and device-specific flexibility.

It was selected.

---

### Alternative C: Message-Based Custom Protocol

The host and node could exchange variable-length messages such as:

```text
COMMAND
PARAMETERS
FEEDBACK
STATUS
```

#### Advantages

* flexible payload structure
* easy addition of new message types
* can represent complex device-specific data
* less constrained by a fixed address window

#### Disadvantages

* framing must be designed
* parsing logic is required
* message-version compatibility must be managed
* host and firmware implementations become larger
* debugging raw communication becomes more complex
* current small command and feedback values do not require this flexibility

#### Evaluation

A message-based protocol could become useful for more complex devices or larger payloads.

It was not required for the current register-oriented control model.

---

### Alternative D: RPC or Service-Style Interface

Each node could expose named operations conceptually similar to:

```text
start()
stop()
set_parameter()
get_status()
get_feedback()
```

#### Advantages

* high-level semantics
* explicit operations
* potentially clear software API
* natural mapping to service-oriented systems

#### Disadvantages

* additional protocol or middleware is required
* serialization and dispatch mechanisms must be implemented
* increased firmware and host complexity
* more difficult to inspect directly at byte level
* unnecessary abstraction for small Pico nodes and simple control transactions

#### Evaluation

This model could be appropriate for networked computers or more capable embedded systems.

It was not selected for the current low-overhead tabletop node architecture.

---

### Alternative E: Direct Device-Specific Host Logic

The host could directly issue low-level commands based on each physical device implementation.

#### Advantages

* fast initial implementation
* little interface-design effort
* no shared abstraction required

#### Disadvantages

* host becomes tightly coupled to device internals
* physical changes propagate into orchestration code
* monitoring and debugging become device-specific
* reuse across processes decreases
* responsibility separation becomes unclear

#### Evaluation

This approach may be acceptable for a single fixed prototype.

It was not selected because it conflicts with the modular and reconfigurable goals of the system.

---

## Selected Interface Model

The common register interface was selected because it provides the strongest overall fit for:

* distributed node abstraction
* common lifecycle management
* host-side reuse
* monitoring
* debugging
* node extension
* staged parameter updates
* explicit command acceptance
* device-specific profiles
* individual implementation practicality

The selected model intentionally does not attempt to define every possible device behavior.

It defines enough shared structure to support reusable host interaction while leaving physical behavior within the node profile.

---

## Consequences

### Positive Consequences

The selected interface provides:

* consistent host-facing structure across node types
* reusable host access logic
* reusable monitoring behavior
* common lifecycle observation
* explicit command results
* parameter staging
* profile-aware extension
* common debugging workflow
* predictable register categories
* room for future extension

---

### Negative Consequences

The interface also introduces:

* common-specification maintenance
* unused fields for some node types
* need to preserve compatibility
* requirement to distinguish common and profile-specific semantics
* possibility that future devices may not fit the current model
* risk that reserved fields are interpreted prematurely
* additional abstraction compared with direct device control

---

### Context-Dependent Consequences

The fixed 64-byte register model is sufficient for current nodes.

It may become less appropriate if future devices require:

* large data payloads
* high-rate streaming
* complex structured configuration
* large numbers of feedback channels
* asynchronous message exchange
* dynamically sized data

In those cases, possible approaches include:

* extending the register model
* adding profile-specific windows
* introducing a message protocol
* separating high-bandwidth data from control registers

The current interface should therefore be treated as a fit-for-purpose control interface rather than a universal device protocol.

---

## Validation Through Current Implementation

The current implementation demonstrates the common interface through:

* motor nodes
* servo nodes
* sensor nodes
* shared identification fields
* shared lifecycle state
* shared command processing
* shared parameter staging
* shared completion reporting
* shared update signaling
* three common feedback channels
* profile-specific interpretation
* host-side register inspection

The same structural interface is reused across different physical device categories.

This demonstrates that the current common abstraction is sufficient for the implemented proof-of-concept nodes.

---

## Implementation Notes and Current Limitations

The documented access labels describe the intended public interface.

The current generic I²C write handler does not enforce read-only protection for every register marked `RO`.

Some nominally read-only backing registers may therefore be writable through the generic handler unless they receive special handling.

Host software should follow the documented access rules.

Firmware-side enforcement remains an extension point.

Several fields also exist in the register window without complete current common-core behavior.

Examples include:

* unit metadata
* feedback sequence
* feedback flags
* configuration fields
* extension fields
* immediate-command placeholders

These are reserved architectural space, not evidence of completed functionality.

---

## Source of Truth

The ADL describes the architectural decision.

The current firmware source remains authoritative for implemented register behavior.

| Topic                                           | Authoritative source                                                                       |
| ----------------------------------------------- | ------------------------------------------------------------------------------------------ |
| Register addresses and common bit definitions   | [`../Firmware/common/include/node_core.h`](../Firmware/common/include/node_core.h)         |
| Register write, staging, and lifecycle behavior | [`../Firmware/common/core/node_core.c`](../Firmware/common/core/node_core.c)               |
| Node profiles, module types, and capabilities   | [`../Firmware/common/platform/node_profile.h`](../Firmware/common/platform/node_profile.h) |
| Human-readable register map                     | [`../Docs/Register_Map/`](../Docs/Register_Map/)                                           |

Where the ADL, documentation, historical spreadsheets, and current implementation differ, the current source code determines implemented behavior.

---

## Decision Boundary

This decision establishes:

* a common host-facing register model
* the `0x00–0x3F` common register window
* common identification structure
* common lifecycle representation
* common command-result model
* common parameter-staging behavior
* common feedback locations
* profile-specific interpretation where required
* reserved extension space

This decision does not establish:

* the physical meaning of every profile-specific parameter
* the physical meaning of every feedback value
* the detailed behavior of each actuator or sensor
* the complete ownership boundary between software layers
* production protocol compatibility
* high-bandwidth streaming behavior
* universal compatibility with future devices

The separation of responsibilities between host, common firmware, node-specific firmware, monitoring, and physical mechanisms is addressed in the next architectural decision.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [Register Map](../Docs/Register_Map/)
* [Firmware Overview](../Firmware/)
* [System Architecture](../Docs/System_Architecture/)
* [I2C Debugger](../Software/I2C_Debugger/)
