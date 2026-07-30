# PORT_X / MAIN_LINE Power Monitor

<img width="1237" height="454" alt="Grafana time-series panel showing voltage, current, and power measurements for a monitored power port" src="https://github.com/user-attachments/assets/dcf77cd9-ea83-4f6a-b24d-c1525713b750" />

*Grafana Time series panel showing voltage, current, and power measurements for a selected branch port or the main monitored power line.*

---

## Overview

This panel visualizes electrical measurements recorded for one selected power-monitor channel.

The target can be:

* one branch power line from `PORT_1` to `PORT_8`
* the main monitored power line identified as `MAIN_LINE`

The panel displays bus voltage, current, and power across the selected Grafana time range.

It is intended for examining how the electrical load changed during a demonstration, test run, or selected monitoring period.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* How did the bus voltage change over time?
* When did current consumption increase or decrease?
* When did the measured power reach its highest or lowest level?
* Did a branch load change during a particular operation?
* Did the main monitored line change at approximately the same time as a branch?
* Was an electrical change temporary or sustained?
* Did the selected line remain stable during the displayed period?

The panel shows measured electrical behavior.

It does not independently determine which software command, module action, or physical event caused a change.

---

## Displayed Measurements

The panel returns three measurement fields:

| Field | Meaning | Recommended Grafana unit |
|---|---|---|
| `bus_voltage_V` | Measured bus voltage for the selected power-monitor channel | Volts |
| `current_mA` | Measured current for the selected power-monitor channel | Milliamps |
| `power_mW` | Power calculated by the I2C Debugger from the recorded voltage and current values | Milliwatts |

`power_mW` is not an independently measured channel.

The I2C Debugger calculates it from the acquired voltage and current values before storing the snapshot in SQLite.

All three values are plotted against the same timestamp field:

```text
ts_epoch
```

The panel returns two acquired measurement fields and one calculated field.

This makes it possible to compare voltage, current, and power behavior across the same displayed period.

---

## Data Source

The query reads from:

```text
power_port_snapshots
```

The table contains timestamped voltage and current measurements acquired from the Power Monitor Board, together with power values calculated by the I2C Debugger.

Each record used by this panel contains:

* measurement timestamp
* power-port identifier
* bus voltage
* current
* power

Grafana reads the stored measurements and displays them as time-series data.

It does not directly communicate with the Power Monitor Board.

---

## Port Model

The current monitoring model distinguishes between branch channels and the main monitored line.

### Branch Ports

The following identifiers represent individual branch power lines:

```text
PORT_1
PORT_2
PORT_3
PORT_4
PORT_5
PORT_6
PORT_7
PORT_8
```

A branch-port panel is used to examine the electrical behavior associated with one monitored output path.

### Main Line

The following identifier represents the main monitored power line:

```text
MAIN_LINE
```

The MAIN_LINE panel provides a higher-level view of the monitored electrical load.

The README and SQL use the same query structure for both branch ports and the main line. Only the `port_id` value changes.

---

## Relationship Between PORT_X and MAIN_LINE

The branch and main-line panels provide different levels of observation.

```text
MAIN_LINE
└─ Overall monitored electrical behavior

PORT_1–PORT_8
└─ Electrical behavior of individual monitored branches
```

The MAIN_LINE view can show when the overall monitored load changed.

An individual PORT_X view can help narrow that change to a specific branch.

However, this panel does not itself calculate or prove that:

```text
MAIN_LINE = PORT_1 + PORT_2 + ... + PORT_8
```

Any comparison between the main line and branch measurements should account for the actual monitoring configuration, measurement timing, and available recorded data.

---

## Target-Port Selection

The SQL contains the following placeholder:

```sql
AND port_id = 'XXX'
```

Replace `XXX` with the identifier of the channel displayed by the panel.

Examples include:

```sql
AND port_id = 'PORT_1'
```

```sql
AND port_id = 'PORT_5'
```

```sql
AND port_id = 'PORT_8'
```

```sql
AND port_id = 'MAIN_LINE'
```

The current dashboard design uses one query per selected target.

A panel can therefore be duplicated and configured with a different `port_id` when another branch or the main line needs to be displayed.

---

## SQL Processing Model

The query performs four main operations:

| Step                 | Responsibility                                                   |
| -------------------- | ---------------------------------------------------------------- |
| Source selection     | Reads timestamped measurements from `power_port_snapshots`       |
| Time-range filtering | Includes only records inside the current Grafana dashboard range |
| Port filtering       | Includes only records for the configured `port_id`               |
| Result ordering      | Returns measurements in chronological order                      |

Unlike the State Timeline query, this SQL does not reconstruct intervals.

Unlike the event panels, it does not filter state transitions.

It returns the stored electrical snapshots directly.

---

## SQL

```sql
SELECT
  ts_epoch AS time,
  bus_voltage_V,
  current_mA,
  power_mW
FROM power_port_snapshots
WHERE ts_epoch BETWEEN (${__from} / 1000.0) AND (${__to} / 1000.0)
  AND port_id = 'XXX'
ORDER BY ts_epoch
```

---

## Grafana Time-Range Filtering

The query uses Grafana’s built-in dashboard variables:

```text
${__from}
${__to}
```

Grafana provides these values in milliseconds.

The database stores `ts_epoch` as Unix epoch seconds.

The query therefore divides the Grafana values by `1000.0`:

```sql
${__from} / 1000.0
${__to} / 1000.0
```

Only measurements whose timestamps fall inside the current dashboard range are returned.

---

## Relationship with Recent Sessions

The Recent Sessions panel can provide:

* a selected monitoring-session ID
* a dashboard start time
* a dashboard end time

This power-monitor query uses the dashboard time range but does not directly filter by `$session_id`.

```text
Recent Sessions
        ↓
from_ms and to_ms
        ↓
Grafana dashboard time range
        ↓
PORT_X / MAIN_LINE query
```

The displayed electrical records are therefore selected primarily by time.

This differs from panels such as Latest Events or Errors / ESTOP, which can filter records directly using `$session_id`.

---

## Time-Range Selection Caveat

Because the query filters by timestamp rather than session ID, any matching power records inside the selected time range may be displayed.

For the current proof-of-concept workflow, monitoring sessions are reviewed using bounded dashboard ranges generated from the corresponding session start and end times.

This keeps the selected electrical history aligned with the intended demonstration or test run.

However, the SQL itself does not verify that every returned row belongs to one specific monitoring session.

If overlapping or closely spaced sessions are introduced, direct session filtering may be required to prevent unrelated records from appearing in the same time range.

---

## Snapshot Interpretation

Each row represents one stored electrical observation.

The query does not assume that a value remains valid until the next measurement in the same way as the State Timeline query.

Grafana connects the returned points visually as a time series.

The displayed line therefore represents a visualization of discrete recorded measurements rather than a continuously measured electrical waveform.

The measurement resolution is limited by the rate at which the I2C Debugger records power-monitor snapshots.

Short changes occurring between recorded samples may not appear in the panel.

---

## Operational Interpretation

A change in current or power indicates that the stored electrical measurement changed for the selected channel.

The panel does not by itself prove:

* which module caused the change
* which command was active
* whether the change was expected
* whether a mechanical load increased
* whether a fault occurred
* whether the measurement difference is operationally significant

The panel should be interpreted alongside other evidence, such as:

* I2C Address State Timeline
* Latest Events
* Errors / ESTOP
* session metadata
* the physical system configuration
* the corresponding demonstration sequence

For example, a power increase near a node transition may suggest a relationship, but the two observations alone do not establish causation.

---

## Active and Completed Sessions

This query does not require a confirmed session end time.

It can display any stored power-monitor measurements inside the current Grafana time range.

This includes measurements associated with:

* an active monitoring period
* a completed monitoring period
* a manually selected historical range

For immediate monitoring, the I2C Debugger live UI remains the primary interface.

Grafana is primarily used to review persisted electrical behavior across a selected period.

---

## Result Ordering

Measurements are ordered by:

```sql
ORDER BY ts_epoch
```

The records are therefore returned from oldest to newest within the selected time range.

This chronological order is appropriate for Grafana Time series panels.

The query does not apply a row limit.

The number of returned records therefore depends on:

* selected time-range length
* power-monitor sampling or polling frequency
* amount of data stored for the selected port

---

## Performance Characteristics

Query cost and visualization density increase with:

* the length of the selected Grafana time range
* the number of recorded power snapshots
* the measurement interval
* the number of duplicated port panels
* the number of channels displayed simultaneously

A separate query is normally used for each selected port.

Displaying many branch ports over a long time range may therefore increase database and dashboard load.

The current implementation is intended for bounded tabletop demonstrations and selected monitoring periods rather than unrestricted long-duration history.

---

## Grafana Panel Settings

Recommended panel type:

* Time series

Recommended configuration:

* Replace `XXX` with the target `port_id`.
* Use one query or panel for each target branch or main line.
* Display `bus_voltage_V`, `current_mA`, and `power_mW` as separate series.
* Apply the following units:

  * `bus_voltage_V`: volts
  * `current_mA`: milliamps
  * `power_mW`: milliwatts
* Use `${__from}` and `${__to}` to follow the dashboard time range.
* Keep the result ordered by `ts_epoch`.
* Use a bounded time range when inspecting high-density measurements.
* Compare related PORT_X and MAIN_LINE panels over the same dashboard range.
* Use separate axes or panel configuration when differences in measurement scale make one series difficult to read.

---

## Current Scope and Limitations

The current panel assumes:

* measurements stored in `power_port_snapshots`
* timestamps stored as Unix epoch seconds
* Grafana dashboard timestamps supplied in milliseconds
* a configured `port_id`
* branch identifiers from `PORT_1` to `PORT_8`
* a main-line identifier of `MAIN_LINE`
* voltage stored in volts
* current stored in milliamps
* power stored in milliwatts
* time-based rather than session-ID-based filtering
* discrete recorded snapshots rather than continuous waveform capture
* bounded prototype-scale monitoring periods

The panel does not:

* automatically select a port
* show all ports from one query
* directly filter by `$session_id`
* calculate energy consumption
* calculate average or maximum values
* detect electrical faults
* determine the cause of load changes
* issue control commands
* replace the I2C Debugger live UI

These constraints are intentional for the current proof-of-concept scope.

---

## Possible Future Extensions

Possible extensions include:

* selecting `port_id` through a Grafana dashboard variable
* filtering directly by monitoring-session ID
* displaying multiple branch ports in one panel
* comparing MAIN_LINE with the sum of selected branch measurements
* configurable snapshot aggregation for long time ranges
* threshold annotations
* links from power changes to state-transition events
* calculated energy consumption
* automatic selection of related session and power data

These extensions are not required for the current tabletop prototype.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* Recent Sessions → [`../Recent Sessions/`](../Recent%20Sessions/)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Latest Events → [`../Latest Events/`](../Latest%20Events/)
* Errors / ESTOP → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* Power Consumption & MAX Ranking → [`../Power_consumption_&_MAX ranking/`](../Power_consumption_%26_MAX%20ranking/)
* Statistics → [`../Statistics/`](../Statistics/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Power Monitor Board → [`../../../../Hardware/Power_Monitor_Board/`](../../../../Hardware/Power_Monitor_Board/)
* Software overview → [`../../../README.md`](../../../README.md)
