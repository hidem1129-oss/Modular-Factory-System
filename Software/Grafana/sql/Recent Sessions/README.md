# Recent Sessions

<img width="1246" height="455" alt="Grafana table showing recent monitoring sessions and session metadata" src="https://github.com/user-attachments/assets/c6293db3-47b7-4d58-a770-1907e2b8d40e" />

*Grafana Table panel showing recent monitoring sessions and navigation values for opening session-specific dashboard views.*

---

## Overview

This panel lists recent monitoring sessions recorded by the I2C Debugger.

It acts as an entry point for historical analysis.

Rather than presenting node or power-monitor behavior directly, the panel identifies individual monitoring runs and provides the values needed to open other dashboard panels with the corresponding:

* monitoring-session ID
* dashboard time range

A selected session can then be examined using the State Timeline, Latest Events, Errors / ESTOP, and power-monitor panels.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* What monitoring sessions were recorded most recently?
* When did each session begin and end?
* How long did each completed session run?
* Was the session performed using real or mock hardware access?
* What polling interval was used?
* How did the monitoring session finish?
* Which session ID should be used to filter the other panels?
* What Grafana time range covers the selected run?

The panel organizes recorded executions into identifiable analysis units rather than one continuous history.

---

## Role in the Dashboard

The Recent Sessions panel is primarily a navigation and context-selection panel.

```text
Recent Sessions
        ↓
Select monitoring session
        ↓
Set $session_id
        ↓
Set dashboard time range
        ↓
Review state, events, and power data
```

This allows observations from separate demonstrations or test runs to be reviewed independently.

Without session selection, records from different runs, configurations, reader modes, or test conditions may be mixed together.

---

## Displayed Information

The query returns the following fields:

| Field          | Meaning                                                                                             |
| -------------- | --------------------------------------------------------------------------------------------------- |
| `Started`      | Time-range start shown by the query, including a 10-second margin before the recorded session start |
| `Ended`        | Time-range end shown by the query, including a 10-second margin after the recorded session end      |
| `from_ms`      | Grafana navigation timestamp in milliseconds, 10 seconds before the session start                   |
| `to_ms`        | Grafana navigation timestamp in milliseconds, 10 seconds after the session end                      |
| `Session time` | Duration between the recorded session start and end                                                 |
| `Mode`         | Reader mode used by the I2C Debugger                                                                |
| `Poll / ms`    | Configured monitoring polling interval                                                              |
| `Result`       | Recorded session result                                                                             |
| `session_link` | Monitoring-session ID used for dashboard filtering or data links                                    |

The `from_ms`, `to_ms`, and `session_link` fields are primarily intended for Grafana navigation rather than direct interpretation by an operator.

---

## Data Source

The query reads from:

```text
monitor_sessions
```

Each row represents one monitoring session created by the I2C Debugger.

The session record provides:

* a unique session ID
* session start time
* session end time
* reader mode
* polling interval
* session result

Other monitoring tables use the same session ID to associate node observations, state transitions, state-duration records, and power measurements with the corresponding run.

---

## Session Identity

The `session_id` identifies one execution of the monitoring application.

In this query, it is returned as:

```sql
session_id AS session_link
```

The alias indicates that the value is intended to be configured as a Grafana data link.

A data link can pass the selected value to another dashboard or panel as:

```text
$session_id
```

This enables the destination panels to show only records belonging to the selected monitoring run.

---

## Time-Range Navigation

Grafana dashboards commonly apply a shared time range to time-series panels.

The query generates two helper values:

```sql
(started_ts_epoch - 10) * 1000 AS from_ms,
(ended_ts_epoch + 10) * 1000 AS to_ms
```

These values can be included in a Grafana data link to set the destination dashboard time range.

Grafana navigation timestamps are expressed in milliseconds, while the database stores session timestamps as Unix epoch seconds. The query therefore multiplies the values by `1000`.

### Hidden Navigation Fields

The `from_ms` and `to_ms` fields are helper values used by Grafana data links to set the destination dashboard time range.

They are not intended to be read directly by the dashboard viewer.

In the current panel configuration, removing these fields from the query result prevented the linked time range from being resolved correctly and also affected the expected `Ended` display behavior.

The fields are therefore retained in the query result but visually minimized or hidden in the table.

```text
Visible purpose
└─ session start, end, duration, mode, result, and session selection

Hidden implementation support
├─ from_ms
└─ to_ms
```

This is a presentation compromise rather than unused data.

The values remain necessary for the current Grafana navigation configuration even when they are not visually prominent.


---

## Navigation Margin

The generated time range includes a 10-second margin on both sides of the recorded monitoring session.

```text
Navigation start
└─ recorded session start minus 10 seconds

Navigation end
└─ recorded session end plus 10 seconds
```

The margin helps ensure that events or measurements close to the session boundaries remain visible after opening another dashboard view.

It also provides a small amount of surrounding context when investigating startup or shutdown behavior.

The margin affects the navigation range only. It does not change the session timestamps stored in SQLite.

---

## Display-Time Caveat

The current SQL also applies the 10-second navigation margin to the displayed `Started` and `Ended` columns:

```sql
datetime(started_ts_epoch - 10, 'unixepoch', 'localtime') AS Started,
datetime(ended_ts_epoch + 10,   'unixepoch', 'localtime') AS Ended
```

These columns therefore represent the dashboard navigation window rather than the exact recorded session boundaries.

The actual monitoring session begins 10 seconds after the displayed `Started` value and ends 10 seconds before the displayed `Ended` value.

For the current proof-of-concept dashboard, this keeps the visible table and linked time range aligned.

A future version could display the exact session timestamps separately while retaining `from_ms` and `to_ms` as hidden navigation fields.

For example:

```sql
datetime(started_ts_epoch, 'unixepoch', 'localtime') AS Started,
datetime(ended_ts_epoch,   'unixepoch', 'localtime') AS Ended,
(started_ts_epoch - 10) * 1000 AS from_ms,
(ended_ts_epoch + 10) * 1000 AS to_ms
```

This is an optional presentation improvement and is not required for the current dashboard behavior.

---

## SQL Processing Model

The query performs the following operations:

| Step                         | Responsibility                                                              |
| ---------------------------- | --------------------------------------------------------------------------- |
| Source selection             | Reads monitoring-session records from `monitor_sessions`                    |
| Navigation-range calculation | Adds a 10-second margin before and after the session                        |
| Time conversion              | Converts Unix epoch values into local display time and Grafana milliseconds |
| Duration calculation         | Calculates the elapsed time between session start and end                   |
| Metadata selection           | Returns reader mode, polling interval, result, and session ID               |
| Result ordering              | Places the newest session first                                             |
| Result limiting              | Returns the five most recent sessions                                       |

The query does not read node, event, or power-monitor records directly.

It provides the context needed to open panels that do.

---

## SQL

```sql
SELECT
  datetime(started_ts_epoch - 10, 'unixepoch', 'localtime') AS Started,
  datetime(ended_ts_epoch + 10,   'unixepoch', 'localtime') AS Ended,
  (started_ts_epoch - 10) * 1000 AS from_ms,
  (ended_ts_epoch + 10)   * 1000 AS to_ms,

  printf('%02d:%02d',
    (ended_ts_epoch - started_ts_epoch) / 60,
    (ended_ts_epoch - started_ts_epoch) % 60
  ) AS "Session time",

  reader_mode AS Mode,
  polling_ms AS "Poll / ms",
  result AS Result,
  session_id AS session_link
FROM monitor_sessions
ORDER BY started_ts_epoch DESC
LIMIT 5;
```

---

## Session-Duration Calculation

The query calculates session duration from:

```text
ended_ts_epoch - started_ts_epoch
```

The result is formatted as:

```text
minutes:seconds
```

using:

```sql
printf('%02d:%02d',
  (ended_ts_epoch - started_ts_epoch) / 60,
  (ended_ts_epoch - started_ts_epoch) % 60
)
```

For example:

```text
03:25
```

represents a session duration of 3 minutes and 25 seconds.

The current format is suitable for bounded prototype demonstrations.

For sessions longer than 59 minutes, the first value continues to represent total minutes rather than hours. A 90-minute session would therefore be displayed as:

```text
90:00
```

This behavior is acceptable for the current tabletop test scope but should be considered if long-running monitoring sessions are introduced.

---

## Active and Completed Sessions

A monitoring session may not yet have an `ended_ts_epoch` while it is active.

For such a record, the following values become `NULL`:

* `Ended`
* `to_ms`
* `Session time`

The session may still appear in the table because the query does not filter out active sessions.

However, a complete dashboard time-range link cannot be generated until the session has a confirmed end time.

The practical division is therefore:

```text
Active session
├─ visible as a session record
├─ no confirmed end time
└─ I2C Debugger live UI is the primary interface

Completed session
├─ confirmed start and end times
├─ complete navigation range
└─ suitable for Grafana retrospective analysis
```

If only completed sessions should be shown, the query could be extended with:

```sql
WHERE ended_ts_epoch IS NOT NULL
```

The current query leaves active sessions visible so their existence and current metadata can still be inspected.

---

## Reader Mode

The `Mode` field shows the reader implementation used by the I2C Debugger.

Depending on the monitoring configuration, this may distinguish between:

* physical SMBus or I²C access
* mock data operation

This information is important when comparing sessions because mock and physical runs do not represent the same operating conditions.

The exact reader-mode values are generated by the monitoring application.

---

## Polling Interval

The `Poll / ms` field shows the configured polling interval in milliseconds.

This value provides context for interpreting:

* state-transition timing
* snapshot density
* database record volume
* monitoring responsiveness
* query cost

A shorter polling interval can detect changes sooner but also generates more observations and increases database growth.

The session metadata allows this configuration to remain visible when reviewing historical results.

---

## Session Result

The `Result` field shows the outcome recorded when the monitoring session was closed.

The value is supplied by the I2C Debugger.

It provides high-level context for distinguishing normally completed sessions from sessions that ended under a different recorded condition.

The exact set and meaning of result values are defined by the monitoring application rather than Grafana.

Grafana displays the stored value without independently determining whether the session succeeded or failed.

---

## Result Ordering and Limit

Sessions are ordered by `started_ts_epoch` in descending order.

The most recently started session therefore appears first.

The query currently returns a maximum of:

```text
5 sessions
```

This keeps the navigation panel compact.

The limit does not remove older session records from SQLite. It only limits the number of rows returned to this panel.

If older sessions need to be reviewed, the limit can be increased or an additional session-selection mechanism can be introduced.

---

## Time-Zone Assumption

The query formats displayed timestamps using SQLite local time:

```sql
datetime(..., 'unixepoch', 'localtime')
```

The displayed values therefore depend on the local-time configuration of the SQLite host.

The query does not append an explicit time-zone offset.

If the Grafana environment and SQLite host use different time zones, the displayed session times should be verified.

The navigation values `from_ms` and `to_ms` remain Unix epoch-based and are not affected by display formatting in the same way.

A future implementation could return epoch timestamps directly and allow Grafana to control all time-zone conversion.

---

## Relationship with Other Panels

### Recent Sessions

```text
Recent Sessions
└─ Which monitoring run should be examined?
```

### State Timeline

```text
State Timeline
└─ How did node states develop during that run?
```

### Latest Events

```text
Latest Events
└─ What state changes were recorded?
```

### Errors / ESTOP

```text
Errors / ESTOP
└─ Which nodes entered selected abnormal states?
```

### Power Panels

```text
Power panels
└─ How did electrical load change during that period?
```

The Recent Sessions panel supplies the common session and time context used to navigate these analytical views.

---

## Grafana Panel Settings

Recommended panel type:

* Table

Recommended configuration:

* Display the newest session at the top.
* Configure `session_link` as a Grafana data link.
* Pass `session_link` to the destination dashboard as `$session_id`.
* Pass `from_ms` and `to_ms` as the destination dashboard time range.
* Hide `from_ms` and `to_ms` from the visible table if they are used only for links.
* Keep the session ID visible when it is useful for troubleshooting.
* Apply value mappings to reader mode or result values when helpful.
* Consider visually distinguishing active sessions with no end time.

A typical data-link structure uses:

```text
var-session_id=${__data.fields.session_link}
from=${__data.fields.from_ms}
to=${__data.fields.to_ms}
```

The exact field-reference syntax may depend on the Grafana version and panel configuration.

---

## Current Scope and Limitations

The current panel assumes:

* monitoring sessions recorded in `monitor_sessions`
* Unix epoch timestamps stored in seconds
* Grafana navigation timestamps expressed in milliseconds
* a fixed 10-second navigation margin
* SQLite host local time for displayed timestamps
* session duration displayed as total minutes and seconds
* a maximum result count of five sessions
* no pagination
* no search by session ID
* no automatic comparison of two selected sessions
* incomplete navigation values for active sessions without an end time

The panel does not:

* display node states directly
* display event details directly
* display power measurements directly
* determine whether a session result is successful
* close an active monitoring session
* modify monitoring records
* control the physical system

These constraints are intentional for the current proof-of-concept scope.

---

## Possible Future Extensions

Possible extensions include:

* separate exact session-time and navigation-window columns
* filtering completed and active sessions
* searching by session ID
* increasing or configuring the result limit
* formatting long durations as hours, minutes, and seconds
* links to separate state and power dashboards
* selecting two sessions for comparison
* displaying event or error counts per session
* displaying the number of responding nodes per session
* adding notes or test-condition metadata

These extensions are not required for the current tabletop prototype.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Latest Events → [`../Latest Events/`](../Latest%20Events/)
* Errors / ESTOP → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* PORT_X / MAIN LINE → [`../PORT_X MAIN_LINE/`](../PORT_X%20MAIN_LINE/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Software overview → [`../../../README.md`](../../../README.md)
