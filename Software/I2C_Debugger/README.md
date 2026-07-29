# I2C Debugger - Node & Power Monitor

---

<img width="1920" height="1032" alt="Screenshot of the I2C Debugger" src="https://github.com/user-attachments/assets/04d2dc7e-5b6e-4300-918a-7d2fca8a3bf3" />

*The I2C Debugger showing node states, selected-node details, event logs, and power-monitor values.*

---

## Overview

The I2C Debugger is the real-time monitoring application for the Modular Factory System.

It observes distributed I²C nodes, interprets their register values, displays their current condition, records state changes, and collects power-monitor measurements.

The application is primarily deployed on the Raspberry Pi 5 used as the host controller for the tabletop system.

It can also run in mock mode on a general-purpose PC for UI, state-logic, logging, and failure-handling tests without connected hardware.

---

## Why This Application Exists

A basic I²C scanner can confirm whether a device responds at an address.

That is not sufficient to understand the condition of a distributed hardware system.

During system operation, it is also necessary to determine:

* which type of module is connected
* whether the module is ready or currently executing
* whether the firmware has reported a warning, error, or emergency stop
* whether communication failed temporarily or has been lost
* whether a node was never connected or disappeared after being detected
* what command, setpoint, target time, and feedback values are currently exposed
* whether branch or main-line electrical load is approaching its configured limit
* when a state changed and how long the condition continued

The I2C Debugger provides this observation layer.

It converts low-level register values into information that can be used during debugging, demonstrations, and later investigation.

---

## Main Features

* monitors I²C node addresses from `0x10` to `0x19`
* identifies module types through the shared register interface
* displays READY, BUSY, WARN, ERROR, ESTOP, Signal Lost, No Device, and UNKNOWN states
* decodes status flags and command values
* displays setpoint, target-time, and feedback values when supported
* records node-state transitions in SQLite
* records periodic node snapshots
* records state-duration segments
* collects power-monitor values from INA219 sensors
* displays branch and main-line electrical load
* supports configurable polling intervals
* supports real I²C and mock reader modes
* provides a PyQt5 desktop UI
* creates session-based monitoring logs for later analysis
* provides persisted data for Grafana dashboards

---

## Application Responsibilities

The I2C Debugger owns:

* polling node and power-monitor registers
* interpreting raw register values
* maintaining the latest observed node state
* distinguishing absent nodes from previously detected nodes
* detecting state transitions
* presenting current system condition to the operator
* recording observations and monitoring sessions
* exposing persisted data for later visualization

The application does not own:

* device-local actuator or sensor behavior
* authoritative firmware lifecycle state
* emergency-stop implementation
* low-level safety shutdown
* process-sequence execution
* historical dashboard rendering

Those responsibilities remain in firmware, orchestration software, hardware protection, and Grafana respectively.

The reasoning behind these system-wide boundaries is documented in the Architecture Decision Log.

---

## Application Structure

| File or directory             | Responsibility                                                                                  |
| ----------------------------- | ----------------------------------------------------------------------------------------------- |
| `UI/`                         | PyQt UI files generated from Qt Designer                                                        |
| `i_2_c_debugger_prototype.py` | Application entry point                                                                         |
| `mainwindow.py`               | Main-window coordination, UI updates, polling timer, mode selection, and session handling       |
| `debugger_model.py`           | Node polling, latest-state management, communication-failure handling, and transition detection |
| `readers.py`                  | Real SMBus reader and simulated register reader                                                 |
| `node_logic.py`               | Status-bit decoding and operator-facing state selection                                         |
| `models.py`                   | Raw, view, and transition data models                                                           |
| `constants.py`                | Address range, polling options, state bits, display values, and application constants           |
| `db_logger.py`                | SQLite schema and persistence operations                                                        |
| `power_monitor.py`            | Power-monitor polling and logging service                                                       |
| `power_config.py`             | Logical port and physical monitor mapping                                                       |
| `power_calc.py`               | Conversion of INA219 raw values into voltage, current, and power                                |
| `run_i2c_debugger.sh`         | Raspberry Pi launcher script                                                                    |

---

## Internal Data Flow

```text
I²C nodes and power monitors
              ↓
          Register readers
              ↓
      Raw node and power data
              ↓
     Debugger model and services
              ↓
    Status and measurement logic
         ├─ current UI state
         ├─ transition events
         └─ periodic snapshots
              ↓
         PyQt UI + SQLite
```

The hardware-access layer, state interpretation, user interface, and persistence layer are separated so that each can be tested or changed with limited impact on the others.

---

## Node Monitoring

The monitor polls the configured I²C node range.

Default address range:

```text
0x10 - 0x19
```

For each responding node, the application reads the common register interface and builds a view of its current condition.

The core information includes:

* module type
* raw status value
* decoded status flags
* current command
* setpoint value, when supported
* target time, when supported
* feedback value, when supported
* last successful observation time
* current communication state

Optional registers are read independently.

A module can therefore remain visible as a valid node even when a device-specific parameter or feedback register is not implemented.

---

## Display States

The UI presents one primary state for each node.

| State         | Meaning                                                              |
| ------------- | -------------------------------------------------------------------- |
| READY         | The node is available for a new operation                            |
| BUSY          | The node is currently executing an operation                         |
| WARN          | The node or related measurement has reported a warning condition     |
| ERROR         | The node has reported an error condition                             |
| ESTOP         | The emergency-stop state is active                                   |
| Signal Lost   | The node responded previously, but communication has since been lost |
| No Device     | No node has been detected at the address                             |
| UNKNOWN       | The current condition cannot yet be classified reliably              |
| Session Ended | The monitoring session has been closed                               |

---

## From Protocol Flags to Display State

Firmware exposes status as a bit field.

The UI does not display every possible bit combination as a separate state. Instead, it selects the most operationally important condition.

The current priority is:

```text
ESTOP
  ↓
ERROR
  ↓
WARN
  ↓
BUSY
  ↓
READY
  ↓
UNKNOWN
```

This prevents a lower-priority state such as READY or BUSY from hiding a more important warning or failure condition.

The application retains the decoded flags separately, so the operator-facing state is a summary rather than a replacement for the raw protocol information.

Power-related warnings can also be merged into the displayed state.

For example, a node may be protocol-level READY or BUSY while the UI presents WARN because the observed electrical condition requires attention.

---

## Communication-Failure Handling

A failed I²C read does not immediately prove that a node has disappeared.

Temporary communication failures may occur because of bus timing, electrical noise, multiplexer switching, or another transient condition.

The debugger therefore distinguishes three situations.

### Temporary uncertainty

A small number of consecutive read failures produces `UNKNOWN`.

This indicates that the current condition cannot be confirmed yet, without immediately declaring a disconnection.

### No Device

If an address has never responded and the configured failure threshold is reached, the address is classified as `No Device`.

This normally represents:

* an unused address
* a disconnected module
* an incorrect address setting
* a module that was not powered when monitoring began

### Signal Lost

If a node responded previously but later exceeds the communication-failure threshold, it is classified as `Signal Lost`.

This normally represents:

* power loss
* cable disconnection
* node firmware failure
* bus disruption
* an unexpected reset or shutdown

The distinction preserves operational context.

A node that disappeared during operation is more significant than an address that was unused from the beginning.

---

## Transition Detection

The debugger records changes between displayed states.

Examples include:

```text
READY → BUSY
BUSY → READY
BUSY → WARN
READY → Signal Lost
ERROR → READY
```

The first polling cycle establishes the initial observed condition and is not treated as a transition from an invented previous state.

Each transition record includes:

* timestamp
* node address
* module name
* previous state
* new state
* monitoring session

This makes it possible to reconstruct the order of events after a demonstration or test run.

---

## Power Monitoring

The application collects power-monitor data from INA219 sensors connected through TCA9548A I²C multiplexers.

Each logical power port is mapped to:

* TCA9548A address
* TCA9548A channel
* INA219 address
* logical port identifier

The monitoring layer records:

* port ID
* TCA address
* TCA channel
* INA219 address
* bus voltage
* raw shunt value
* calculated current
* calculated power
* monitoring session ID
* timestamp

The conversion from INA219 raw values is implemented in `power_calc.py`.

---

## Main-Line and Branch Monitoring

The UI displays both individual branch measurements and the aggregate condition of the main power line.

For the main line, the application calculates:

* total current
* total power
* configured current usage percentage
* configured power usage percentage

The larger of the current and power utilization ratios is used as the main-line load indicator.

This gives the operator a quick view of remaining electrical capacity without requiring manual comparison of every branch.

The configured limits are application-level monitoring thresholds. They do not replace hardware current limiting, fuses, power-supply protection, or firmware safety behavior.

---

## Logging Model

Logs are stored in SQLite.

Default database path:

```text
/var/lib/i2c_debugger/i2c_debugger_events.sqlite3
```

The current database includes:

| Table                  | Purpose                                           |
| ---------------------- | ------------------------------------------------- |
| `event_logs`           | Human-readable and system-generated event records |
| `node_snapshots`       | Periodic records of observed node conditions      |
| `run_sessions`         | Execution-related session records                 |
| `state_segments`       | Continuous periods spent in a state               |
| `monitor_sessions`     | Monitoring application sessions and configuration |
| `power_port_snapshots` | Periodic power-monitor measurements               |

The application stores both events and snapshots because they answer different questions.

* transition events show what changed
* snapshots show what was observed at a specific time
* state segments show how long a condition continued
* sessions provide the context in which observations were collected

These records are used by the application and by Grafana dashboards.

---

## Monitoring Sessions

A unique monitoring session is created when the application starts.

The session records context such as:

* reader mode
* polling interval
* session start
* session end
* node and power observations generated during the run

Session IDs prevent data from separate demonstrations or test runs from being mixed without context.

They also make it possible to compare real and mock operation or investigate one specific run in Grafana.

---

## Reader Modes

The application supports replaceable register readers.

| Mode   | Description                                                                                               |
| ------ | --------------------------------------------------------------------------------------------------------- |
| `real` | Reads actual I²C nodes and power-monitor devices through SMBus                                            |
| `mock` | Simulates nodes, register states, power values, transient errors, missing devices, and communication loss |

Both readers provide the same application-facing interface.

The model and UI therefore do not require separate architectures for real and simulated operation.

### Real mode

Real mode is the primary deployment mode.

It opens the configured SMBus interface on the Raspberry Pi 5 and reads the actual node and power-monitor registers.

If the SMBus interface cannot be opened, the application can return to mock mode instead of terminating without a usable UI.

### Mock mode

Mock mode supports testing without the physical system.

It is used to exercise conditions such as:

* READY nodes
* BUSY nodes
* warning and error states
* ESTOP
* unused addresses
* temporary read failures
* communication loss after successful detection
* representative branch power values

Mock mode is therefore not only a visual demonstration mode. It is also a repeatable failure-scenario source for UI and state-logic testing.

---

## Polling Model

The application uses a configurable periodic polling loop.

At each polling cycle it:

1. reads the configured node addresses
2. updates the latest observed node models
3. evaluates communication-failure counters
4. decodes protocol values
5. determines operator-facing states
6. detects state transitions
7. reads power-monitor values
8. updates the UI
9. records events and snapshots when required

The polling interval can be selected in the application UI.

A shorter interval improves observation latency but increases I²C traffic, database activity, and UI update frequency.

A longer interval reduces system load but may miss or delay the presentation of short-lived conditions.

The current implementation is intended for tabletop observation rather than hard real-time control.

---

## Launch

The launcher script activates the Python virtual environment and starts the PyQt application.

```bash
./run_i2c_debugger.sh
```

The script contains placeholder paths.

Update `APP_DIR` to match the directory where `I2C_Debugger` is installed on the Raspberry Pi 5.

The application can also be started directly through its Python entry point after the required environment has been configured.

---

## Requirements

* Python 3
* PyQt5
* smbus2
* SQLite support from the Python standard library
* Raspberry Pi I²C interface for real mode
* access permission for the configured SQLite database path

---

## Relationship to Grafana

The I2C Debugger and Grafana serve different purposes.

| Component    | Primary role                                                     |
| ------------ | ---------------------------------------------------------------- |
| I2C Debugger | Current-state monitoring, event detection, and persistence       |
| SQLite       | Durable boundary between observation and later analysis          |
| Grafana      | Historical visualization, session comparison, and trend analysis |

Grafana does not access the hardware directly and is not part of the control path.

It reads observations that have already been written to SQLite by the monitoring application.

---

## Design Notes

The application intentionally separates:

* register access from state interpretation
* state interpretation from UI rendering
* live observation from persistent logging
* physical hardware access from mock scenarios
* monitoring from device-local control
* current-state display from historical visualization

This README describes how those boundaries appear inside the I2C Debugger.

The broader reasoning behind the boundaries between hardware, firmware, monitoring, orchestration, persistence, and visualization is documented in the Architecture Decision Log.

---

## Current Scope and Limitations

The current application is optimized for the Modular Factory System tabletop prototype.

It currently assumes:

* a known I²C node address range
* a Raspberry Pi 5 as the primary host
* a local SMBus interface
* a local SQLite database
* one active monitoring application instance
* fixed power-monitor port mapping
* no redundant monitoring path
* no hard real-time guarantee
* no production-grade authentication or remote access control

These limitations are intentional for the current proof-of-concept scope.

---

## Related Documents

* Software overview → [`../README.md`](../README.md)
* Firmware overview → [`../../Firmware/`](../../Firmware/)
* Hardware overview → [`../../Hardware/`](../../Hardware/)
* Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
* System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
* Power Monitor Board → [`../../Hardware/Power_Monitor_Board/`](../../Hardware/Power_Monitor_Board/)
* Grafana → [`../Grafana/`](../Grafana/)
* Architecture Decision Log → [`../../ADL/`](../../ADL/)
