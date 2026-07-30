# Latest Events

<img width="1237" height="454" alt="Grafana table showing recent I²C node state-change events" src="https://github.com/user-attachments/assets/cee0038d-e305-48ea-9dc8-8b8a058c1150" />

*Grafana Table panel showing the latest recorded I²C node state-change events.*

---

## Overview

This panel lists recent node-state transitions recorded by the I2C Debugger.

Unlike the State Timeline panel, which reconstructs state behavior across an entire monitoring session, this panel focuses only on recorded changes.

It provides a compact event list that can be used as an entry point for investigating recent system activity.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* What state changes occurred most recently?
* Which I²C address changed state?
* What was the previous state?
* What state did the node enter?
* Did several nodes change state near the same time?
* Did an abnormal state occur before another recorded event?

The panel is intended for quick event inspection rather than full state-duration analysis.

For a continuous view of node states across a completed session, see:

* [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)

---

## Displayed Information

Each row represents one recorded state-transition event.

The panel displays:

| Field         | Meaning                                         |
| ------------- | ----------------------------------------------- |
| `event_time`  | Local time at which the transition was recorded |
| `address_hex` | I²C address of the node                         |
| `old_state`   | Previously observed operator-facing state       |
| `new_state`   | Newly observed operator-facing state            |

The states have already been interpreted by the I2C Debugger before the event is written to SQLite.

Grafana does not independently decode firmware flags or determine node state.

---

## Data Source

The query reads from:

```text
event_logs
```

Only records with the following event type are included:

```text
STATE_CHANGE
```

The event table contains transitions detected by the monitoring application.

This panel does not read periodic records from `node_snapshots`. It therefore shows changes rather than every polling result.

---

## Session Filtering

The panel uses the Grafana dashboard variable:

```text
$session_id
```

The query supports two filtering modes.

### Selected Session

When `$session_id` contains a monitoring-session ID, only events associated with that session are shown.

This is useful when reviewing one demonstration, test run, or failure investigation.

### All Sessions

When `$session_id` is:

```text
__all
```

the query shows the most recent matching events across all monitoring sessions.

This provides a quick cross-session overview but may combine events from unrelated runs.

For detailed analysis, selecting one session is generally more useful.

---

## SQL Processing Model

The query performs four main operations:

| Step              | Responsibility                      |
| ----------------- | ----------------------------------- |
| Source selection  | Reads records from `event_logs`     |
| Event filtering   | Includes only `STATE_CHANGE` events |
| Session filtering | Selects one session or all sessions |
| Result limiting   | Returns the 20 most recent events   |

The panel does not reconstruct state intervals or calculate duration.

It shows the recorded transition events directly.

---

## SQL

```sql
SELECT
  datetime(ts_epoch, 'unixepoch', 'localtime') AS event_time,
  address_hex,
  old_state,
  new_state
FROM event_logs
WHERE event_type = 'STATE_CHANGE'
  AND ('$session_id' = '__all' OR session_id = '$session_id')
ORDER BY ts_epoch DESC
LIMIT 20;
```

---

## Result Ordering and Limit

Events are ordered by `ts_epoch` in descending order.

The newest recorded transition therefore appears first.

The query currently returns a maximum of:

```text
20 events
```

This limit keeps the panel compact and prevents the dashboard table from expanding indefinitely.

The limit does not mean that older events have been deleted from SQLite. It only limits the rows returned to this panel.

If deeper historical inspection is required, the limit can be increased or the selected session can be narrowed.

---

## Time-Zone Assumption

The query formats timestamps using SQLite local time:

```sql
datetime(ts_epoch, 'unixepoch', 'localtime')
```

The displayed time therefore depends on the local-time configuration of the SQLite host.

Unlike the State Timeline query, this query does not append an explicit time-zone offset.

If the dashboard and database host use different time-zone settings, the displayed event time should be verified.

A future implementation could return Unix epoch values directly and allow Grafana to handle time-zone conversion consistently.

---

## Relationship with Other Panels

### State Timeline

The State Timeline shows how node states continue across time.

```text
State Timeline
└─ What state was each node in over the session?
```

### Latest Events

The Latest Events panel shows only the moments at which a state change was recorded.

```text
Latest Events
└─ What changed most recently?
```

### Errors / ESTOP

The Errors / ESTOP panel filters the event history further and shows transitions into abnormal states.

```text
Errors / ESTOP
└─ Which nodes entered ERROR or ESTOP?
```

The Latest Events panel therefore acts as a general event list between the full timeline and the abnormal-event-only view.

---

## Grafana Panel Settings

Recommended panel type:

* Table

Recommended dashboard variable:

* `$session_id`

Panel notes:

* Sort by `event_time` descending.
* Display the newest event at the top.
* Use a completed or active session ID when reviewing one monitoring run.
* Use `__all` only when a cross-session overview is useful.
* Apply state-specific value mappings or colors when needed.
* Increase the query limit only when additional history is required.

---

## Current Scope and Limitations

The current panel assumes:

* state-change events recorded by the I2C Debugger
* an `event_type` value of `STATE_CHANGE`
* session IDs generated by the monitoring application
* timestamps stored as Unix epoch values
* SQLite host local time for display
* a maximum result count of 20 events
* no pagination inside the Grafana panel

The panel lists recorded changes only.

It does not:

* show unchanged periodic snapshots
* calculate how long a state continued
* reconstruct missing event history
* determine the cause of a state transition
* issue control commands
* replace the I2C Debugger live UI

These constraints are intentional for the current proof-of-concept scope.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Errors / ESTOP → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* Recent Sessions → [`../Recent Sessions/`](../Recent%20Sessions/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Software overview → [`../../../README.md`](../../../README.md)
