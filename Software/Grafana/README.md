# Grafana

This directory contains the Grafana dashboards, SQL queries, screenshots, and configuration notes used to visualize monitoring data recorded by the I2C Debugger.

Grafana is the historical visualization and analysis component of the Modular Factory System.

It reads persisted observations from SQLite and presents system behavior over time. It does not communicate directly with hardware and is not part of the device-control or safety path.

---

## Overview

The Modular Factory System provides two complementary views of system condition.

| Component    | Primary viewer                              | Primary question                   |
| ------------ | ------------------------------------------- | ---------------------------------- |
| I2C Debugger | On-site operator or developer               | What is happening now?             |
| Grafana      | Supervisor, system owner, or decision-maker | What has been happening over time? |

The I2C Debugger presents the latest observed condition of nodes and power-monitor channels.

Grafana uses the observations persisted by the debugger to show timelines, trends, events, state duration, and differences between monitoring sessions.

In this sense, the live UI presents system condition as a current point, while Grafana presents system behavior as a sequence over time.

---

## Why Grafana Is Used

A live monitoring screen is useful when an operator needs to respond to the current condition of the physical system.

For example, the operator may need to determine:

* which nodes are currently READY or BUSY
* whether an ERROR or ESTOP state is active
* whether a node has disappeared
* what command and feedback values are currently exposed
* what current, voltage, and power values are currently being measured

The latest state alone is not sufficient for supervision or later analysis.

A supervisor or system owner may instead need to determine:

* when a problem started
* which state preceded it
* how long the condition continued
* whether the same problem occurred repeatedly
* whether power consumption changed during the event
* whether behavior differed between test runs
* whether a change improved or degraded system behavior

Grafana supports this second type of observation by presenting persisted monitoring data across time.

---

## Responsibilities

Grafana owns:

* historical visualization of recorded node states
* visualization of state transitions and notable events
* inspection of warning, error, and ESTOP history
* monitoring-session selection and comparison
* visualization of time spent in each state
* visualization of current, voltage, and power measurements
* comparison of average and maximum power consumption
* trend and statistical analysis based on persisted observations

Grafana does not own:

* communication with I²C nodes
* communication with power-monitor devices
* interpretation of raw protocol flags
* determination of node display states
* communication-failure classification
* event detection
* database writes
* device-local control
* process-sequence execution
* safety or emergency-stop decisions

These responsibilities remain in the I2C Debugger, firmware, orchestration software, and hardware protection layers.

---

## Data Source and Boundary

Grafana uses the SQLite database created by the I2C Debugger.

Default database path:

```text
/var/lib/i2c_debugger/i2c_debugger_events.sqlite3
```

The SQLite database acts as the boundary between real-time observation and historical analysis.

```text
Firmware nodes and power monitors
                ↓
          I2C Debugger
          ├─ live operator UI
          └─ SQLite logging
                ↓
              Grafana
       └─ historical analysis
```

Grafana reads observations that have already been interpreted and recorded by the monitoring application.

It does not poll the I²C bus or independently determine whether a node is READY, BUSY, WARN, ERROR, ESTOP, missing, or disconnected.

This separation allows dashboards and SQL queries to be changed without changing node firmware or the hardware-access path.

---

## Intended Viewers

### On-Site Operator

The on-site operator primarily uses the I2C Debugger.

The live UI is intended to support immediate operational questions such as:

* Is the node responding now?
* Is the node ready to accept another command?
* Is an error or emergency-stop condition active?
* What value is currently being measured?
* Has communication just been lost?

The operator-facing view prioritizes the latest observed state and short-term response.

### Supervisor or System Owner

Grafana is intended for viewers who need broader operational context.

This may include:

* a supervisor reviewing a completed test
* a developer investigating repeated failures
* a system owner comparing different configurations
* a project member evaluating whether the PoC behaved as expected
* a decision-maker reviewing state-duration or power-consumption trends

This view prioritizes patterns, duration, comparisons, and changes over time.

The distinction is not based on access authority alone. It reflects the type of decision being made:

```text
Live UI
└─ immediate operational decision

Grafana
└─ supervisory, analytical, or improvement decision
```

---

## Dashboard Structure

The current Grafana material is organized around two main dashboard areas.

### State Timeline Dashboard

The state timeline dashboard is used to inspect system behavior across time.

It includes panels for:

* I²C node-state timelines
* recent state-transition events
* ERROR and ESTOP events
* recent monitoring sessions

This dashboard helps answer questions such as:

* In what order did node states change?
* How long did a node remain BUSY or in an error state?
* Did communication loss occur before or after another event?
* Did the same state pattern occur in multiple sessions?
* Was the system restored after an ERROR or ESTOP event?

### Power Dashboard

The power dashboard is used to inspect electrical measurements recorded by the power-monitoring layer.

It includes panels for:

* Which branch had the highest average recorded calculated power?
* Which channel produced the highest recorded calculated-power sample?
* the main power line
* current, voltage, and power trends
* average and maximum recorded calculated-power values
* recorded current and calculated-power maximum, minimum, and maximum-to-minimum ranges

This dashboard helps answer questions such as:

* Which port consumed the most power?
* Did power consumption change during a node-state transition?
* Was the main-line load approaching the configured monitoring limit?
* Did one test session consume more power than another?
* Which channel showed the widest recorded maximum-to-minimum range?

The configured application limits shown in the monitoring system are observation thresholds. Grafana does not replace hardware current limiting, fuses, power-supply protection, firmware shutdown behavior, or other safety mechanisms.

---

## Dashboard Data Model

Grafana uses monitoring records written by the I2C Debugger.

| SQLite data            | Grafana use                                                   |
| ---------------------- | ------------------------------------------------------------- |
| `event_logs`           | Recent state transitions, including WARN, ERROR, and ESTOP changes |
| `node_snapshots`       | Node-state timelines and point-in-time inspection             |
| `state_segments`       | Duration spent in each observed state                         |
| `monitor_sessions`     | Session selection, filtering, and comparison                  |
| `power_port_snapshots` | Current, voltage, and power trends                            |

The database schema and logging behavior are owned by the monitoring application.

For detailed logging behavior, see:

* [`../I2C_Debugger/README.md`](../I2C_Debugger/README.md)

---

## Session-Based Analysis

Each execution of the I2C Debugger creates a monitoring-session ID.

Node observations, state-transition events, state-duration records, and power measurements can be associated with that session.

Grafana can use the session ID to:

* isolate one demonstration or test run
* avoid mixing observations from separate executions
* compare real and mock operation
* compare behavior before and after a configuration change
* inspect one failure without unrelated historical data
* compare power behavior between sessions

Session-based filtering turns the persisted data into a sequence of identifiable test runs rather than one continuous, context-free stream.

---

## SQL Query Directories

The current SQL query examples are organized by panel.

| Panel                           | Query directory                                                                       | Purpose                                                               | Dashboard      |
| ------------------------------- | ------------------------------------------------------------------------------------- | --------------------------------------------------------------------- | -------------- |
| I2C Address State Timeline      | [`sql/I2C Address State Timeline/`](./sql/I2C%20Address%20State%20Timeline/)          | Shows I²C node states over time                                       | State timeline |
| Latest Events                   | [`sql/Latest Events/`](./sql/Latest%20Events/)                                        | Lists recent state-change events                                      | State timeline |
| Errors / ESTOP                  | [`sql/Errors  ESTOP/`](./sql/Errors%20%20ESTOP/)                                      | Shows recent ERROR and ESTOP events                                   | State timeline |
| Recent Sessions                 | [`sql/Recent Sessions/`](./sql/Recent%20Sessions/)                                    | Lists recent monitoring sessions                                      | State timeline |
| PORT_X / MAIN_LINE              | [`sql/PORT_X MAIN_LINE/`](./sql/PORT_X%20MAIN_LINE/)                                  | Shows voltage, current, and power for a selected port                 | Power          |
| Power Consumption & MAX Ranking | [`sql/Power_consumption_&_MAX ranking/`](./sql/Power_consumption_%26_MAX%20_ranking/) | Compares average and maximum power consumption by port                | Power          |
| Statistics                      | [`sql/Statistics/`](./sql/Statistics/)                                                | Shows current and power minimum, maximum, and peak-to-peak statistics | Power          |

Each directory may contain:

* SQL queries
* dashboard screenshots
* panel configuration notes
* time-range settings
* session-filter notes

---

## Design Intent

Grafana is intentionally separated from real-time hardware access.

This separation provides several benefits:

* historical analysis does not add traffic to the I²C bus
* dashboard changes do not require firmware changes
* monitoring history remains available after the live UI closes
* supervisors can review completed runs without operating the physical system
* live observation and historical analysis can evolve independently
* recorded sessions can be compared using the same visualization layer

The I2C Debugger answers the immediate operational question.

```text
What is the system doing now?
```

Grafana answers the supervisory and analytical questions.

```text
What happened over time?
How often did it happen?
How long did it continue?
What changed between runs?
```

The detailed reasoning behind the separation of observation, persistence, visualization, and control is documented in the Architecture Decision Log.

---

## Current Scope and Limitations

The current Grafana configuration is intended for the Modular Factory System tabletop prototype.

It currently assumes:

* a local SQLite data source
* monitoring records generated by the I2C Debugger
* local or trusted dashboard access
* session IDs generated by the monitoring application
* manually configured dashboards and SQL queries
* no production-grade authentication or role-based access design
* no direct control path from Grafana to the physical system

The current dashboards are intended for debugging, demonstrations, architecture validation, and retrospective analysis rather than production-factory operation.

---

## Related Documents

* Software overview → [`../README.md`](../README.md)
* I2C Debugger → [`../I2C_Debugger/`](../I2C_Debugger/)
* Power Monitor Board → [`../../Hardware/Power_Monitor_Board/`](../../Hardware/Power_Monitor_Board/)
* Firmware overview → [`../../Firmware/`](../../Firmware/)
* System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
* Architecture Decision Log → [`../../ADL/`](../../ADL/)
