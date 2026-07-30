# Errors / ESTOP

<img width="1242" height="449" alt="Grafana table showing recent ERROR and ESTOP state-change events" src="https://github.com/user-attachments/assets/97970d42-69b5-4771-be76-b9a1dcd1c7c2" />

*Grafana Table panel showing recent transitions into ERROR and ESTOP states.*

---

## Overview

This panel lists recent transitions into abnormal node states recorded by the I2C Debugger.

It is a focused view of the state-transition history and includes only events where a node entered:

* ERROR
* ESTOP

Unlike the Latest Events panel, which displays general node-state changes, this panel prioritizes transitions that may require investigation, recovery, or confirmation of system safety behavior.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* Which node most recently entered ERROR or ESTOP?
* When was the abnormal transition recorded?
* Did the same I²C address enter an abnormal state repeatedly?
* Did an ESTOP occur during the selected monitoring session?
* Which address should be investigated first?
* Did abnormal transitions occur across multiple sessions?

The panel identifies recorded abnormal transitions.

It does not determine their physical, electrical, firmware, or process-level cause.

---

## Displayed Information

Each row represents one recorded transition into ERROR or ESTOP.

The panel displays:

| Field         | Meaning                                                                                         |
| ------------- | ----------------------------------------------------------------------------------------------- |
| `event_time`  | Local time at which the abnormal transition was recorded                                        |
| `address_hex` | I²C address of the affected node                                                                |
| `new_state`   | Abnormal state entered by the node                                                              |
| `error_count` | Number of ERROR or ESTOP transitions recorded for that address within the selected filter scope |

Although the column is currently named `error_count`, it includes both ERROR and ESTOP transitions.

The value is therefore an abnormal-transition count rather than an ERROR-only count.

---

## Data Source

The query reads from:

```text
event_logs
```

It includes records where:

```text
new_state = ERROR
```

or:

```text
new_state = ESTOP
```

The states have already been interpreted by the I2C Debugger before being written to SQLite.

Grafana does not independently:

* decode firmware state flags
* determine whether an emergency stop is required
* trigger an emergency stop
* classify the cause of an error
* perform recovery actions

---

## Session Filtering

The panel uses the Grafana dashboard variable:

```text
$session_id
```

The query supports two filtering modes.

### Selected Session

When `$session_id` contains a monitoring-session ID, the panel shows ERROR and ESTOP transitions associated with that session.

The count is also calculated within the selected session.

This mode is useful when investigating one demonstration, test run, or abnormal shutdown.

### All Sessions

When `$session_id` is:

```text
__all
```

the panel shows the most recent matching transitions across all monitoring sessions.

The count is then calculated across all matching sessions.

This can reveal addresses that have repeatedly entered ERROR or ESTOP, but it may also combine unrelated test conditions and system configurations.

For detailed investigation, one selected session generally provides clearer context.

---

## SQL Processing Model

The query performs five main operations:

| Step                   | Responsibility                                           |
| ---------------------- | -------------------------------------------------------- |
| Source selection       | Reads records from `event_logs`                          |
| State filtering        | Includes transitions whose `new_state` is ERROR or ESTOP |
| Session filtering      | Selects one session or all sessions                      |
| Address-based counting | Counts matching transitions for each I²C address         |
| Result limiting        | Returns the five most recent matching transitions        |

The query uses a window function for the count:

```sql
COUNT(*) OVER (PARTITION BY address_hex)
```

This calculates the number of matching rows for each I²C address without reducing the result to one row per address.

The panel can therefore show both:

* individual recent abnormal transitions
* the total number of matching transitions for the same address

---

## SQL

```sql
SELECT
  datetime(ts_epoch, 'unixepoch', 'localtime') AS event_time,
  address_hex,
  new_state,
  COUNT(*) OVER (PARTITION BY address_hex) AS error_count
FROM event_logs
WHERE new_state IN ('ERROR', 'ESTOP')
  AND ('$session_id' = '__all' OR session_id = '$session_id')
ORDER BY ts_epoch DESC
LIMIT 5;
```

---

## Count Behavior

The `error_count` value is calculated after applying the state and session filters but before applying the final result limit.

For example, an address may have:

```text
error_count = 8
```

even when only one or two rows for that address appear in the five displayed results.

This is because:

* the count represents all matching transitions in the selected filter scope
* the table displays only the five most recent matching rows overall

The displayed rows and the count therefore answer different questions:

```text
Displayed rows
└─ What abnormal transitions happened most recently?

error_count
└─ How many matching abnormal transitions were recorded for this address?
```

---

## Result Ordering and Limit

Results are ordered by `ts_epoch` in descending order.

The most recently recorded ERROR or ESTOP transition appears first.

The query currently returns a maximum of:

```text
5 events
```

This limit keeps the panel compact and directs attention to the most recent abnormal transitions.

The limit does not delete or discard older events from SQLite. It only limits the rows returned to this Grafana panel.

The count may still include older matching records that are not visible in the table.

---

## Relationship with Other Panels

### State Timeline

The State Timeline reconstructs the observed condition of each node across a completed monitoring session.

```text
State Timeline
└─ How did node states develop over time?
```

### Latest Events

The Latest Events panel displays recent transitions into any recorded state.

```text
Latest Events
└─ What changed most recently?
```

### Errors / ESTOP

The Errors / ESTOP panel displays only transitions into selected abnormal states.

```text
Errors / ESTOP
└─ Which nodes most recently entered ERROR or ESTOP?
```

This panel is therefore a filtered investigation view rather than a complete event history.

---

## Operational Interpretation

An ERROR or ESTOP row indicates that the monitoring application recorded a node entering that operator-facing state.

It does not by itself prove:

* the root cause of the condition
* whether the failure was electrical, mechanical, firmware-related, or communication-related
* whether an operator intervention occurred
* whether the system recovered successfully
* whether the same physical fault caused repeated transitions

The panel should be used as an entry point for further investigation.

Related evidence may include:

* the State Timeline before and after the transition
* Latest Events around the same timestamp
* power-monitor trends
* firmware logs or status registers
* physical inspection of the affected module

---

## Active and Completed Sessions

This query does not require a confirmed session end time.

It can therefore display abnormal transitions from:

* an active monitoring session
* a completed monitoring session
* all recorded sessions

For immediate operational response, the I2C Debugger live UI remains the primary interface.

The Grafana panel provides a compact persisted record that can be reviewed during or after the monitoring session.

---

## Time-Zone Assumption

The query formats timestamps using SQLite local time:

```sql
datetime(ts_epoch, 'unixepoch', 'localtime')
```

The displayed event time therefore depends on the local-time configuration of the SQLite host.

The query does not append an explicit time-zone offset.

If the Grafana environment and SQLite host use different time-zone settings, the displayed event time should be verified.

A future implementation could return Unix epoch values directly and allow Grafana to apply time-zone conversion consistently.

---

## Grafana Panel Settings

Recommended panel type:

* Table

Recommended dashboard variable:

* `$session_id`

Panel notes:

* Display the newest transition at the top.
* Highlight ERROR and ESTOP using distinct value mappings.
* Use one selected session for detailed incident investigation.
* Use `__all` only when comparing abnormal-transition frequency across sessions.
* Keep the result limit small when the panel is used as a compact abnormal-event summary.
* Increase the limit when deeper historical inspection is required.
* Consider renaming the displayed `error_count` field to `ERROR / ESTOP Count` in Grafana.

---

## Current Scope and Limitations

The current panel assumes:

* ERROR and ESTOP states interpreted by the I2C Debugger
* abnormal transitions recorded in `event_logs`
* session IDs generated by the monitoring application
* timestamps stored as Unix epoch values
* SQLite host local time for display
* a maximum result count of five transitions
* counts grouped by I²C address
* no root-cause classification

The panel does not:

* display WARN transitions
* display transitions out of ERROR or ESTOP unless another panel is used
* distinguish ERROR count from ESTOP count
* determine whether repeated events share the same cause
* calculate abnormal-state duration
* verify physical emergency-stop operation
* perform recovery or control actions

These constraints are intentional for the current proof-of-concept scope.

---

## Possible Future Extensions

Possible extensions include:

* separate ERROR and ESTOP counts
* filtering by abnormal-state type
* links from each row to the corresponding State Timeline period
* grouping repeated abnormal transitions into incidents
* displaying the previous state before ERROR or ESTOP
* correlation with power-monitor measurements
* configurable result limits

These extensions are not required for the current tabletop prototype.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* Latest Events → [`../Latest Events/`](../Latest%20Events/)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Recent Sessions → [`../Recent Sessions/`](../Recent%20Sessions/)
* PORT_X / MAIN_LINE → [`../PORT_X MAIN_LINE/`](../PORT_X%20MAIN_LINE/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Software overview → [`../../../README.md`](../../../README.md)
