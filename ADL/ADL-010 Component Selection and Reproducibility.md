# ADL-010: Component Selection and Reproducibility

## Purpose of This Document

This document explains the component-selection and procurement principles used to improve reproducibility, maintainability, and reviewability in the Modular Factory System.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)
* [ADL-007: Separation of Responsibilities](./ADL-007%20Separation%20of%20Responsibilities.md)
* [ADL-008: Reconfigurable Physical Mechanisms](./ADL-008%20Reconfigurable%20Physical%20Mechanisms.md)
* [ADL-009: Monitoring and Control Separation](./ADL-009%20Monitoring%20and%20Control%20Separation.md)

The earlier decisions establish the system architecture.

This document focuses on how physical components, PCB parts, and procurement information are selected and documented so that the implementation can be inspected, reproduced, repaired, or modified with reasonable effort.

---

## Decision Summary

The Modular Factory System prefers components that are:

* identifiable
* documented
* available through established procurement channels
* electrically and mechanically understandable
* replaceable where practical
* suitable for prototype assembly
* compatible with the current tabletop scale
* supported by reproducible manufacturing information

The current hardware documentation includes:

* board schematics
* Gerber fabrication data
* DigiKey MyList / BOM references
* board-specific implementation notes
* manufacturing guidance
* substitution guidance

The architecture does not require every reproduction to use the exact same manufacturer part number forever.

Instead, reproducibility is supported by preserving enough information to determine:

* what component was used
* what function it performs
* what characteristics matter
* how it connects to the rest of the system
* what must be checked before substitution

---

## Decision Context

A physical proof-of-concept can become difficult to reproduce even when its software is fully available.

Common causes include:

* undocumented modules
* unknown component manufacturers
* unavailable marketplace products
* changed product revisions
* missing PCB fabrication files
* unclear connector types
* undocumented substitutions
* unavailable mechanical parts
* reliance on one-off assembly knowledge

This creates a difference between:

```text
"The prototype worked once"
```

and:

```text
"The prototype can be inspected,
rebuilt, repaired, or adapted later"
```

The project therefore treats procurement and component traceability as part of the system architecture rather than as an administrative task performed after design.

---

## Selection and Reproducibility Goals

The component strategy was selected to support the following goals.

| ID   | Goal                                                         |
| ---- | ------------------------------------------------------------ |
| CR1  | Use identifiable components where practical                  |
| CR2  | Prefer established procurement channels                      |
| CR3  | Preserve manufacturer and part information                   |
| CR4  | Provide PCB manufacturing data                               |
| CR5  | Provide procurement references                               |
| CR6  | Support reasoned component substitution                      |
| CR7  | Avoid undocumented dependency on obscure modules             |
| CR8  | Preserve electrical and mechanical compatibility information |
| CR9  | Support repair and replacement                               |
| CR10 | Keep component choices appropriate to PoC scale              |
| CR11 | Separate exact implementation from architectural requirement |
| CR12 | Make reproducibility limits explicit                         |

---

## CR1: Identifiable Components

PCB-mounted components should be identifiable through manufacturer and part information where practical.

This allows the developer or reviewer to determine:

* datasheet
* electrical limits
* package
* pinout
* tolerance
* temperature range
* rated voltage
* rated current
* expected behavior

An identifiable component provides a stronger engineering reference than an undocumented module whose internal design or component revision is unknown.

This does not imply that every mechanical item or consumable must have complete manufacturer traceability.

The level of traceability should match the role of the component.

---

## CR2: Established Procurement Channels

The current PCB component procurement references primarily use DigiKey MyLists.

Established distributors provide useful information such as:

* manufacturer
* manufacturer part number
* package
* lifecycle information
* stock status
* datasheet links
* ordering quantities
* technical parameters

This reduces ambiguity compared with procurement methods where the delivered internal component or revision may change without notice.

The architecture does not require DigiKey specifically.

The important requirement is that the selected procurement path provides sufficient traceability and technical information.

---

## CR3: Manufacturer and Part Information

Where practical, a component should be selected using a known manufacturer part number rather than only a generic description.

For example:

```text
Preferred documentation:
Manufacturer + manufacturer part number
```

rather than only:

```text
"10 kΩ resistor"
"motor driver module"
"current sensor board"
```

The generic function remains important for substitution.

However, the original implementation should be traceable to a concrete component.

This provides a reference point for:

* reproduction
* troubleshooting
* comparison
* substitution
* lifecycle review

---

## CR4: PCB Manufacturing Data

Custom PCBs are reproduced through fabrication data stored with the corresponding board.

The repository includes Gerber data for the current board designs.

Before manufacturing, the current guidance requires review of items such as:

* board revision
* board dimensions
* board outline
* layer count
* drill recognition
* board thickness
* copper weight
* trace and clearance compatibility
* solder-mask visibility
* silkscreen visibility
* connector orientation
* polarized-component orientation
* mounting-hole position
* fabrication preview

Gerber availability improves reproducibility.

It does not guarantee that every stored revision is universally production-ready.

The board documentation and schematic must still be reviewed before ordering.

---

## CR5: Procurement References

Each major custom board has a procurement reference associated with it.

Current examples include:

* Controller Board
* DC Motor Board
* Servo Board
* Sensor Board
* Power Monitor Board
* Pi5 Wiring Auxiliary

The manufacturing documentation links each board to:

* board documentation
* Gerber data
* DigiKey MyList / BOM reference

This allows the physical implementation to be followed from:

```text
board function
    ↓
schematic
    ↓
PCB fabrication data
    ↓
component procurement
    ↓
assembly
```

The BOM or procurement list does not replace the schematic or datasheet.

Each source answers a different question.

---

## CR6: Controlled Substitution

Exact component availability can change.

The architecture therefore permits substitution when the replacement has been reviewed for relevant compatibility.

Substitution must not be based only on:

* similar name
* similar appearance
* equal package size
* marketplace description

The required comparison depends on component category.

---

### Passive Components

For resistors and capacitors, relevant checks may include:

* nominal value
* tolerance
* power rating
* voltage rating
* dielectric type
* ESR or frequency behavior where relevant
* temperature coefficient
* package
* land pattern

A passive component from another manufacturer may be acceptable when the required electrical and mechanical characteristics remain compatible.

---

### Shunt Resistors

Relevant checks include:

* resistance
* tolerance
* power rating
* temperature coefficient
* package
* current-sense suitability

Because the component participates in measurement, substitution may directly affect measurement behavior.

---

### Ferrite Beads

Relevant checks include:

* impedance characteristic
* rated current
* DC resistance
* package
* frequency range

A component with the same package is not necessarily electrically equivalent.

---

### Resettable Fuses

Relevant checks include:

* hold current
* trip current
* trip-time behavior
* ambient-temperature derating
* package
* voltage rating

The protection behavior must remain appropriate after substitution.

---

### Semiconductors and Integrated Circuits

Semiconductor substitution requires particular care.

Relevant checks include:

* pinout
* logic levels
* electrical limits
* timing
* control behavior
* register compatibility
* thermal behavior
* startup behavior

A device with a similar function name or package should not automatically be treated as a drop-in replacement.

---

### Connectors and Switches

Relevant checks include:

* pitch
* keying
* orientation
* mating part
* current rating
* contact arrangement
* mechanical height
* locking behavior

Mechanical compatibility is part of electrical-system reproducibility.

---

## CR7: Avoiding Undocumented Modules

The architecture avoids unnecessary reliance on poorly documented third-party modules where a traceable component or self-documented implementation is practical.

The primary concerns with obscure modules include:

* unknown internal component revisions
* inconsistent schematics
* undocumented substitutions
* uncertain electrical limits
* silent design changes
* disappearance from the market
* difficulty reproducing the same behavior later

This does not mean that every low-cost module is technically unsuitable.

A module may still be useful when:

* its internal implementation is sufficiently understood
* the application is temporary or exploratory
* replacement risk is acceptable
* exact reproduction is not required

The decision is therefore based on dependency risk rather than on price or vendor category alone.

---

## CR8: Electrical and Mechanical Compatibility

Reproducibility is not limited to electrical values.

A replacement component may fail to reproduce the implementation even if its nominal electrical function is similar.

Relevant mechanical properties may include:

* footprint
* package dimensions
* connector orientation
* mating connector
* mounting holes
* component height
* shaft dimensions
* actuator geometry

The component-selection process therefore considers:

```text
Electrical compatibility
        +
Mechanical compatibility
        +
Functional compatibility
```

rather than any one factor alone.

---

## CR9: Repair and Replacement

The project favors structures where failed or unavailable parts can be replaced without redesigning the complete system.

Examples include:

* replaceable PCB-mounted components
* separate actuator-interface boards
* separate sensor-interface boards
* replaceable control nodes
* standard connectors where practical
* general-purpose mechanical parts

Replacement may still require:

* soldering
* firmware configuration
* address configuration
* recalibration
* software parameter changes
* mechanical adjustment

Replaceability therefore means that a practical recovery path exists.

It does not mean that every component is hot-swappable or automatically detected.

---

## CR10: Proof-of-Concept Scale

Component selection must remain proportional to the scale and purpose of the system.

The project does not require every component to provide:

* industrial temperature qualification
* redundant supply-chain sourcing
* safety certification
* automotive qualification
* aerospace traceability
* production-volume lifecycle guarantees

Those requirements would significantly increase:

* component cost
* procurement effort
* documentation effort
* design constraints

The current system instead selects components sufficient for supervised tabletop proof-of-concept use.

Higher assurance should be added only when required by the validation objective.

---

## CR11: Implementation Part and Architectural Requirement

A specific component used in the current implementation is not automatically an architectural requirement.

For example, the architecture may require:

```text
a 5 V-compatible servo actuator
```

while the demonstrated implementation may use:

```text
a specific SG90 servo product
```

Similarly, an architecture may require:

```text
a suitable resistor with defined value,
tolerance, power rating, and package
```

without requiring one manufacturer forever.

This distinction allows the architecture to survive component lifecycle changes.

The implementation remains documented as the reference configuration.

---

## CR12: Reproducibility Limits

The project aims for practical reproducibility rather than guaranteed exact duplication.

Several factors can still vary:

* component availability
* distributor stock
* manufacturer revisions
* PCB fabrication tolerances
* assembly quality
* actuator variation
* mechanical setup
* workpiece characteristics
* calibration
* upstream power source
* software environment

The repository therefore provides:

* source information
* fabrication data
* procurement references
* implementation notes
* substitution criteria

rather than claiming that two independently assembled systems will behave identically without validation.

---

## Current Reproducibility Structure

The current hardware reproduction path can be summarized as:

```text
Repository documentation
        ↓
Board README
        ↓
Schematic
        ↓
Gerber files
        ↓
BOM / DigiKey MyList
        ↓
Component datasheets
        ↓
PCB fabrication
        ↓
Assembly
        ↓
Electrical checks
        ↓
Firmware installation
        ↓
System integration
```

Each stage provides different evidence.

No single file represents the complete physical system.

---

## Source-of-Truth Hierarchy

Different sources are authoritative for different questions.

| Question                                | Source                                  |
| --------------------------------------- | --------------------------------------- |
| What is the board intended to do?       | Board README                            |
| How is it electrically connected?       | Schematic                               |
| What is manufactured as the PCB?        | Gerber data                             |
| What parts were selected?               | BOM / procurement references            |
| What are the component limits?          | Manufacturer datasheet                  |
| What does firmware do?                  | Firmware source                         |
| What does the public node interface do? | Register-map and firmware documentation |

This prevents one convenience document from becoming the unintended authority for all aspects of the implementation.

---

## Assembly as Part of Reproduction

A reproducible design also requires reasonable assembly practices.

The current manufacturing guidance recommends:

* visual inspection before soldering
* footprint confirmation
* polarity confirmation
* staged assembly where practical
* continuity checks
* current-limited first power-on
* connector-orientation review
* schematic verification
* functional testing before actuator connection

This is prototype assembly guidance.

It is not a certified manufacturing process.

---

## Risks and Mitigations

| Risk                                        | Current mitigation                                                                |
| ------------------------------------------- | --------------------------------------------------------------------------------- |
| Component becomes unavailable               | Define functional and compatibility requirements for substitution                 |
| Marketplace product changes silently        | Prefer traceable components and established procurement references                |
| Equivalent-looking part behaves differently | Require electrical, mechanical, and functional comparison                         |
| PCB cannot be reproduced                    | Store Gerber fabrication data                                                     |
| Board BOM becomes unclear                   | Maintain board-specific procurement references                                    |
| Wrong revision is manufactured              | Require board and Gerber revision review                                          |
| Connector replacement does not mate         | Include pitch, orientation, mating, and keying checks                             |
| Semiconductor replacement breaks behavior   | Require pinout, limits, logic, timing, and register review                        |
| New assembly is damaged at first power-on   | Use staged inspection and current-limited bring-up                                |
| Reproduction is assumed to be exact         | Explicitly document calibration and implementation variability                    |
| Procurement documentation becomes stale     | Datasheet, lifecycle, stock, and board revision must be rechecked before ordering |
| Exact commercial part disappears            | Preserve architectural requirements separately from implementation references     |

---

## Comparison Summary

| Alternative                                       | Main strengths                                    | Main limitations                                | Fit for current PoC |
| ------------------------------------------------- | ------------------------------------------------- | ----------------------------------------------- | :-----------------: |
| Buy whatever is immediately available             | Lowest initial procurement effort                 | Weak traceability and difficult reproduction    |         Weak        |
| Use only exact fixed part numbers                 | Strong reference configuration                    | Fragile when parts become unavailable           |       Moderate      |
| Traceable components with controlled substitution | Strong reproducibility with lifecycle flexibility | Requires documentation and compatibility review |        Strong       |
| Industrial approved-vendor-list process           | Strong supply-chain governance                    | Excessive process for individual tabletop PoC   |         Weak        |
| Fully custom fabrication of most components       | Maximum control over implementation               | Very high design and manufacturing burden       |         Weak        |

The ratings describe suitability for the current proof-of-concept project.

They are not general rankings of procurement strategies.

---

## Considered Alternatives

### Alternative A: Immediate-Availability Selection

Components could be selected primarily according to what is easiest to buy at the moment.

#### Advantages

* fast initial procurement
* low selection effort
* broad marketplace choice
* potentially low cost

#### Disadvantages

* part identity may be unclear
* internal design may change
* documentation may be limited
* future availability is uncertain
* reproduction becomes difficult
* substitutions may happen without technical review

#### Evaluation

This strategy can be effective for disposable experiments.

It was not selected as the primary component strategy because the project aims to remain inspectable and reproducible beyond one build.

---

### Alternative B: Exact Fixed Part Numbers Only

The project could require every future build to use exactly the same manufacturer part number.

#### Advantages

* strongest reference to the demonstrated configuration
* limited ambiguity during reproduction
* simple BOM comparison
* easier expectation of similar electrical behavior

#### Disadvantages

* components can become obsolete
* stock can disappear
* lead times can increase
* a minor unavailable component can block reproduction
* acceptable equivalent components cannot be used

#### Evaluation

Exact manufacturer part numbers are useful as the reference implementation.

However, they are too restrictive as the only long-term reproducibility strategy.

The project therefore documents exact parts while permitting reviewed substitution.

---

### Alternative C: Traceable Components with Controlled Substitution

The selected approach records the actual implementation while defining what must be checked before replacement.

#### Advantages

* strong reference configuration
* supports future reordering
* supports component lifecycle changes
* enables reasoned substitution
* improves troubleshooting
* makes technical review possible
* avoids unnecessary dependence on one supplier

#### Disadvantages

* more documentation
* substitution requires engineering review
* equivalent behavior cannot be assumed
* procurement records must be maintained
* some replacements may require validation or recalibration

#### Evaluation

This provides the strongest balance between reproducibility and practical component availability.

It was selected.

---

### Alternative D: Industrial Approved-Vendor Process

The project could introduce formal controls such as:

* approved vendor lists
* qualified alternates
* lifecycle monitoring
* multiple-source qualification
* incoming inspection
* controlled change approval

#### Advantages

* strong procurement governance
* clearer production change management
* supply-chain risks can be actively managed
* suitable for larger production programs

#### Disadvantages

* large administrative burden
* substantial qualification effort
* excessive for an individual proof of concept
* does not directly improve the architectural demonstration enough to justify the effort

#### Evaluation

This would become more relevant if the platform moved toward production deployment.

It was not selected for the current scope.

---

### Alternative E: Extensive Custom Fabrication

Many components could be designed or fabricated specifically for the project.

#### Advantages

* direct control over interfaces
* fewer dependencies on commercial product geometry
* potentially optimized integration
* design files can remain under project control

#### Disadvantages

* much greater engineering effort
* increased manufacturing burden
* more validation work
* custom components may themselves become difficult to reproduce
* project focus shifts away from system-level validation

#### Evaluation

Custom fabrication is appropriate when no practical commercial component satisfies the requirement.

It was not selected as the default approach.

---

## Selected Component Strategy

The selected strategy is:

```text
Identify requirement
        ↓
Prefer documented and traceable component
        ↓
Record actual implementation part
        ↓
Provide fabrication / procurement information
        ↓
Define relevant compatibility checks
        ↓
Allow reviewed substitution
        ↓
Revalidate after meaningful change
```

This provides the strongest fit for:

* prototype reproducibility
* repair
* replacement
* reviewability
* component lifecycle changes
* individual development
* reasonable procurement effort

---

## Consequences

### Positive Consequences

The selected strategy provides:

* clearer component traceability
* easier reordering
* easier technical review
* more practical repair paths
* documented PCB reproduction
* controlled substitution
* reduced dependence on undocumented modules
* better long-term maintainability
* clearer distinction between architecture and implementation

---

### Negative Consequences

The strategy also introduces:

* BOM maintenance
* procurement documentation
* datasheet review effort
* substitution validation
* revision management
* possible revalidation after component changes
* more initial component-selection effort

---

### Context-Dependent Consequences

The current strategy is appropriate for a tabletop PoC.

Future requirements could justify stronger controls.

| Future requirement           | Possible change                                      |
| ---------------------------- | ---------------------------------------------------- |
| Production manufacturing     | Formal BOM and revision control                      |
| Long product lifecycle       | Lifecycle monitoring and qualified alternates        |
| Higher safety requirements   | Certified components and controlled substitution     |
| Large production volume      | Approved vendors and incoming inspection             |
| Multiple manufacturing sites | Formal assembly and test procedures                  |
| Regulatory compliance        | Traceable compliance documentation                   |
| Global procurement           | Multi-source qualification                           |
| Higher measurement accuracy  | Tighter component tolerances and calibration control |

---

## Validation Through Current Implementation

The current repository demonstrates this strategy through:

* custom board schematics
* Gerber manufacturing data
* board-specific documentation
* DigiKey MyList / BOM references
* manufacturing checklists
* substitution guidance
* documented board roles
* documented prototype limitations
* component datasheet review as the final authority for device limits

The six documented board categories currently include:

* Controller Board
* DC Motor Board
* Servo Board
* Sensor Board
* Power Monitor Board
* Pi5 Wiring Auxiliary

These artifacts make the physical implementation more inspectable and reproducible than a prototype defined only by photographs or source code.

---

## Decision Boundary

This decision establishes:

* preference for identifiable and documented components
* use of established procurement references
* preservation of PCB manufacturing information
* controlled rather than assumed substitution
* separation of exact reference parts from architectural requirements
* prototype-level assembly and bring-up guidance
* explicit acknowledgment of reproducibility limits

This decision does not establish:

* guaranteed permanent component availability
* mandatory use of DigiKey
* automatic approval of substitute parts
* industrial supply-chain qualification
* formal configuration-management certification
* guaranteed identical physical behavior between independent builds
* production manufacturing readiness
* regulatory compliance

Prototype scope and intentionally deferred features are addressed in the next architectural decision.

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
* [ADL-008: Reconfigurable Physical Mechanisms](./ADL-008%20Reconfigurable%20Physical%20Mechanisms.md)
* [ADL-009: Monitoring and Control Separation](./ADL-009%20Monitoring%20and%20Control%20Separation.md)
* [Hardware Overview](../Hardware/)
* [Manufacturing and Procurement](../Hardware/Manufacturing/)
