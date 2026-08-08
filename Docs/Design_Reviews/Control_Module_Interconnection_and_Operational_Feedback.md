# Control Module Interconnection and Operational Feedback

## Purpose

This document records physical interconnection and usability issues identified during operation of the Modular Factory System.

The current system remains functional, and no immediate redesign has been selected.

The purpose of this document is to preserve:

* observed failures and usability issues
* likely causes and contributing conditions
* the architectural impact of possible changes
* reasons for deferring a redesign
* conditions that should trigger future reconsideration

This document is a design-review and operational-feedback record.

It does not define a new physical interconnection architecture.

---

## Background

The current control modules use custom harnesses for the shared I²C bus and separate ribbon-style connections to actuator and sensor boards.

The original approach was selected during early development because:

* the I²C bus arrangement was still evolving
* cable lengths could be adjusted to the physical prototype
* custom harnesses could be produced quickly
* the initial material cost was low
* exact production-style interconnection was not required for the tabletop proof of concept

This approach was sufficient to build and operate the initial system.

Practical use later exposed additional factors that were not significant during the initial design stage.

---

## Observed Harness Failure

During operation, one custom harness developed a connection failure.

The observed result was that control modules located beyond the affected connection could no longer reliably:

* receive signals
* return state information

A spare harness was installed as a temporary replacement, allowing operation to continue.

The exact failure mechanism was not conclusively identified.

However, the failed harness showed substantial variation in individual wire lengths and an off-center wire bundle relative to the connector.

A plausible failure mechanism is that uneven wire length or mechanical stress caused a contact to move or pull partially out of the connector housing.

This remains an engineering inference rather than a confirmed root cause.

---

## Why the Harness Is Not Being Rebuilt Immediately

Simply manufacturing another harness using the same method would reproduce the same process that has already produced at least one failure.

It would therefore provide limited confidence that the underlying weakness had been removed.

Commercial cable assemblies could provide:

* more consistent manufacturing
* easier replacement
* potentially better connector retention
* reduced manual assembly work

However, this is not necessarily a local cable replacement.

A suitable commercial assembly would need to match:

* connector pitch
* pin count
* pin assignment
* cable length
* electrical requirements
* physical routing requirements

If an appropriate assembly cannot be found for the existing connectors, changing the cable would also require changing the control-module connectors.

---

## Change Propagation

Changing the harness can affect substantially more than the harness itself.

Possible consequences include:

```text
Harness selection
        ↓
Board-side connector selection
        ↓
Connector orientation and PCB placement
        ↓
Control-module PCB layout
        ↓
Module-to-module physical arrangement
        ↓
Bus topology and cable length
        ↓
I²C electrical conditions
        ↓
Manufacturing and documentation updates
```

A redesign may require reconsideration of:

* PCB connector type
* connector orientation
* pin assignment
* polarity and keying
* retention method
* cable bend radius
* mechanical strain
* module spacing
* module installation direction
* replacement procedure
* power and ground conductor allocation
* I²C SDA/SCL routing
* total bus capacitance
* pull-up configuration
* compatibility with existing control modules

The change therefore approaches a physical interconnection architecture redesign rather than a maintenance replacement.

---

## Physical Layout Coupling

The current harnesses do more than carry electrical signals.

Their physical properties also influence the system layout.

Observed or potential effects include:

* module spacing being constrained by harness length
* preferred module orientation being influenced by cable direction
* cable bending and restoring force moving lightweight boards
* mechanical load being transferred into connectors
* cable routing interfering with neighboring connections
* module addition and removal being affected by harness routing

This means that electrical interconnection and physical placement are partially coupled through the harness.

A future redesign should therefore not evaluate cable reliability in isolation.

---

## Physical Interconnection Questions for a Future Revision

A future control-module revision should first define the desired physical connection model.

Relevant questions include:

| Evaluation area     | Question                                                                                                         |
| ------------------- | ---------------------------------------------------------------------------------------------------------------- |
| Module placement    | Should arbitrary placement remain possible, or should modules use a standardized layout?                         |
| Module spacing      | Should spacing remain variable or use a fixed pitch?                                                             |
| Connection order    | Should the system remain daisy-chained, or should branching or hub connections be supported?                     |
| Mechanical mounting | Should modules be fixed independently from wiring or mechanically integrated with the interconnection structure? |
| Module replacement  | Should one module be removable without disconnecting neighboring modules?                                        |
| Cable direction     | Should connection direction be standardized or remain flexible?                                                  |
| Electrical limits   | What cable length, branch structure, bus capacitance, and current should be supported?                           |
| Compatibility       | Should the new system remain compatible with existing control modules?                                           |

Maximum physical flexibility is not necessarily the preferred result.

For example, a standardized linear arrangement could be acceptable if it substantially improves:

* connection reliability
* replacement speed
* setup repeatability
* routing clarity

The appropriate trade-off should be determined by the requirements of the next revision.

---

## Possible Future Interconnection Approaches

Possible approaches include the following.

### Commercial Ribbon-Cable Daisy Chain

Potential advantages:

* repeatable cable manufacturing
* straightforward replacement
* simple architecture

Potential limitations:

* module order becomes more constrained
* cable direction becomes more constrained
* physical arrangement remains linked to cable geometry

---

### Short Independent Module Cables

Each control module could use an individual short connection to a shared distribution point.

Potential advantages:

* greater placement independence
* individual cables can be replaced

Potential limitations:

* more connectors
* more wiring
* more connection points
* greater management effort

---

### Backplane or Common Bus Board

Modules could connect directly to a standardized bus structure.

Potential advantages:

* highly repeatable module spacing
* reduced loose wiring
* standardized connection quality
* simpler replacement

Potential limitations:

* physical layout becomes strongly constrained
* additional PCB design is required
* module pitch and orientation become architectural decisions

---

### Hub or Distribution Board

A central board could provide separate connections to individual control modules.

Potential advantages:

* independent physical placement
* clearer cable ownership
* easier individual module removal

Potential limitations:

* additional PCB and connectors
* longer aggregate wiring
* I²C branching and capacitance must be evaluated
* the hub may introduce a new common failure point

---

## I²C Considerations

A topology change cannot be evaluated only by maximum cable length.

Relevant factors include:

* total cable length
* branch length
* number of connected devices
* connector capacitance
* cable capacitance
* pull-up resistance
* signal rise time
* physical routing

A hub or branching structure may improve physical organization while making the electrical I²C environment more difficult.

Any future topology change should therefore be validated electrically after physical design decisions are made.

---

## Control Module Usability Feedback

Operation also exposed physical usability issues on the current control module.

The shared-bus connections and actuator/sensor connections can constrain access around the board.

In particular, connected cables can interfere with access to the local E-STOP button.

Other observed usability issues include:

* cable tension affecting board placement
* harness length limiting the intended arrangement
* connectors occupying space around controls
* difficulty maintaining a predictable layout after reconfiguration

These issues do not currently prevent operation of the tabletop proof of concept.

However, they show that electrical connectivity, physical arrangement, and operator interaction were more tightly coupled than initially expected.

---

## Local E-STOP Usability

The current control modules provide a local E-STOP input.

Practical operation revealed several usability limitations.

The button is:

* relatively small
* located among cables and connectors
* not always immediately visible
* associated with an individual control module rather than an obvious system-level stopping point

The operator may therefore need to:

1. identify the abnormal device
2. determine which control module operates that device
3. locate the corresponding board
4. locate the small button on that board
5. operate it while avoiding connected cables

This introduces cognitive and physical interaction cost during a situation where rapid stopping may be desirable.

This human-factor cost was not a primary evaluation criterion during the original electrical and firmware design.

---

## Stop-Control Responsibilities

Practical use suggests that different stopping functions should be treated separately.

| Function            | Intended purpose                                                   |
| ------------------- | ------------------------------------------------------------------ |
| System-level stop   | Stop the complete system without requiring module identification   |
| Normal process stop | End or interrupt the process through orchestration                 |
| Local module stop   | Stop or inhibit one module during adjustment or maintenance        |
| Reset               | Explicitly clear a stopped state after the cause has been reviewed |

The current local buttons remain useful as module-level controls.

They are less suitable as the only obvious way to stop the complete reconfigurable system quickly.

---

## Possible System-Level Stop Unit

A future revision may introduce a dedicated system-level stop unit.

The objective would be to provide:

* one clearly visible control
* easy physical access
* no requirement to identify a specific module
* stopping behavior applied to all relevant nodes

A possible implementation could connect to the existing common E-STOP mechanism.

The design priority would be:

```text
Low operator decision cost
        +
Easy physical access
        +
Broad stopping coverage
```

rather than precise selection of the individual module to stop.

For the current tabletop scale, stopping additional modules is a smaller disadvantage than failing to stop the intended module because the operator must first determine which local control to use.

This would be an operational improvement within the proof-of-concept system.

It would not constitute a safety-rated industrial emergency-stop architecture.

---

## Existing Alternative Stop Methods

The current tabletop system can also be stopped by:

* stopping the host-side orchestration
* removing or disabling system power

These options reduce the immediate operational impact of the local-button usability issue.

They do not eliminate the value of evaluating a clearer system-level stop interface in a future revision.

---

## Why Immediate Redesign Is Deferred

The current issues are acknowledged but do not currently justify rebuilding the control-module architecture.

The present conditions are:

* the harness failure has not become a continuously recurring operational problem
* a spare harness restored operation
* the current control-module size is acceptable
* no new requirement currently demands higher module density
* no new requirement currently demands substantial module miniaturization
* existing demonstrations remain operational
* the physical connection redesign would affect multiple hardware and integration layers
* redesign would require new PCB fabrication and system revalidation

The expected improvement therefore does not currently justify the change cost and disruption.

The current decision is:

```text
Continue using the existing implementation
while recording the observed limitations
and reconsider the physical interconnection
strategy when a broader control-module
revision becomes justified.
```

---

## Reconsideration Triggers

The physical interconnection strategy should be reconsidered if one or more of the following occurs:

* harness failures become repeated or operationally significant
* module replacement becomes frequent
* reconfiguration effort becomes a meaningful bottleneck
* control modules need to become smaller
* module count or placement density increases
* a suitable low-cost commercial cable assembly compatible with the existing connectors becomes available
* the I²C topology is changed for another reason
* power distribution is redesigned
* another requirement already makes a control-module PCB revision necessary

The last case is particularly important.

If the control-module PCB must already be revised, the fixed costs of:

* schematic changes
* PCB layout
* fabrication
* assembly
* validation
* documentation updates

can be shared with the interconnection improvements.

---

## Design Lesson

The main result of this operational feedback is not that the original harness or control-module design was simply incorrect.

The original design was sufficient to validate:

* electrical connectivity
* distributed control
* common firmware behavior
* physical tabletop operation

Operation later exposed additional evaluation criteria that were not obvious during the initial design.

These include:

* harness manufacturing repeatability
* mechanical strain
* module placement effects
* replacement convenience
* control accessibility
* operator recognition cost during stopping

This is one of the purposes of building a physical proof of concept.

A design that is functionally correct in schematics, firmware, and block diagrams can still reveal important mechanical and human-interface constraints after actual operation.

---

## Relationship to Future Architectural Decisions

No replacement interconnection architecture has currently been selected.

If a future board revision chooses among approaches such as:

* standardized commercial harnesses
* a revised daisy-chain interface
* a hub architecture
* a backplane
* another physical interconnection method

the reasoning and final selection can be recorded in a new Architecture Decision Log document.

Until that decision exists, this document remains the record of:

* observed operational evidence
* known design limitations
* possible directions
* redesign scope
* reconsideration conditions

---

## Related Documents

* [Architecture Decision Log](../../ADL/)
* [ADL-002: Design and Development Constraints](../../ADL/ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-005: Communication Interface Selection](../../ADL/ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-007: Separation of Responsibilities](../../ADL/ADL-007%20Separation%20of%20Responsibilities.md)
* [ADL-011: Prototype Scope and Deferred Features](../../ADL/ADL-011%20Prototype%20Scope%20and%20Deferred%20Features.md)
* [Hardware Overview](../../Hardware/)
* [Controller Board](../../Hardware/Controller_Board/)
