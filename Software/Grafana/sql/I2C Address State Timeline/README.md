# I2C Address State Timeline

<img width="1580" height="836" alt="Grafana state timeline showing I²C node states by address during a monitoring session" src="https://github.com/user-attachments/assets/e0acfbd3-5eae-4c1f-9a07-a129cf4ef093" />

*Grafana State timeline panel showing I²C node states by address during a completed monitoring session.*

---

## Overview

This panel visualizes the observed state of each I²C node across a monitoring session.

The I2C Debugger records periodic node snapshots as individual observations. This query reconstructs those point-in-time records as state intervals that can be displayed on a Grafana State timeline panel.

The panel is intended for retrospective analysis of completed monitoring sessions.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* In what order did node states change?
* Over what observed time range did each state appear?
* Which nodes changed state at approximately the same time?
* Did a node recover after an ERROR, ESTOP, or communication loss?
* What was the final observed state of each node when the session ended?
* Did multiple nodes show related changes during the same period?

The panel supports historical investigation rather than immediate operational monitoring.

For the current system:

```text
Active monitoring session
└─ I2C Debugger live UI

Completed monitoring session
└─ Grafana state timeline
```

---

## Displayed States

The panel can display operator-facing states recorded by the I2C Debugger, including:

* READY
* BUSY
* WARN
* ERROR
* ESTOP
* UNKNOWN
* Signal Lost
* No Device

The state values have already been interpreted by the I2C Debugger before they are written to SQLite.

Grafana does not independently decode firmware status flags or determine communication-failure states.

---

## Data Sources

The query reads from the following SQLite tables:

| Table              | Purpose in this query                                    |
| ------------------ | -------------------------------------------------------- |
| `node_snapshots`   | Provides timestamped node-state observations             |
| `monitor_sessions` | Provides the selected session and its confirmed end time |

The monitoring session is selected through the Grafana dashboard variable:

```text
$session_id
```

Only snapshots associated with the selected session are included.

---

## Observation Assumption

Each node snapshot represents the latest known state at its observation time.

The query assumes that the recorded state remains valid until the next snapshot for the same I²C address.

For example:

```text
10:00 READY
10:03 BUSY
10:08 READY
```

is interpreted as:

```text
10:00–10:03 READY
10:03–10:08 BUSY
10:08–session end READY
```

The resulting timeline therefore represents observed state intervals reconstructed from periodic snapshots.

It does not provide the exact physical instant at which a node changed state between polling cycles. The displayed transition time is limited by the monitoring polling interval.

---

## SQL Processing Model

The query uses several common table expressions to transform snapshots into Grafana timeline fields.

| Step            | Responsibility                                                                                |
| --------------- | --------------------------------------------------------------------------------------------- |
| `snaps`         | Reads snapshots for the selected session and finds the next observation time for each address |
| `sess`          | Reads the confirmed session end time                                                          |
| `segments`      | Converts each snapshot into a start and end interval                                          |
| `boundaries`    | Collects all interval start and end times                                                     |
| `timeline_rows` | Combines every time boundary with every configured I²C address and assigns the active state   |
| final `SELECT`  | Pivots I²C addresses into separate Grafana fields                                             |

### Finding the next observation

The query uses the SQLite `LEAD()` window function:

```sql
LEAD(ts_epoch) OVER (
  PARTITION BY session_id, address_hex
  ORDER BY ts_epoch
) AS next_ts_epoch
```

For each address, this returns the timestamp of the following snapshot in the same monitoring session.

### Closing the final interval

The last snapshot for an address has no following snapshot.

The query therefore uses the monitoring-session end time as the final interval boundary:

```sql
CASE
  WHEN s.next_ts_epoch IS NOT NULL THEN s.next_ts_epoch
  ELSE ms.ended_ts_epoch
END AS end_ts_epoch
```

### Aligning addresses

The query creates a shared set of timeline boundaries and combines them with the configured I²C address range.

This makes it possible to return one Grafana field for each address, even when different nodes change state at different times.

---

## SQL

```sql
WITH snaps AS (
  SELECT
    session_id,
    address_hex,
    state,
    ts_epoch,
    LEAD(ts_epoch) OVER (
      PARTITION BY session_id, address_hex
      ORDER BY ts_epoch
    ) AS next_ts_epoch
  FROM node_snapshots
  WHERE session_id = '$session_id'
),
sess AS (
  SELECT
    session_id,
    ended_ts_epoch
  FROM monitor_sessions
  WHERE session_id = '$session_id'
),
segments AS (
  SELECT
    s.address_hex,
    s.state,
    s.ts_epoch AS start_ts_epoch,
    CASE
      WHEN s.next_ts_epoch IS NOT NULL THEN s.next_ts_epoch
      ELSE ms.ended_ts_epoch
    END AS end_ts_epoch
  FROM snaps s
  JOIN sess ms
    ON ms.session_id = s.session_id
  WHERE ms.ended_ts_epoch IS NOT NULL
),
boundaries AS (
  SELECT start_ts_epoch AS ts_epoch FROM segments
  UNION
  SELECT end_ts_epoch AS ts_epoch FROM segments
),
timeline_rows AS (
  SELECT
    b.ts_epoch,
    a.address_hex,
    s.state
  FROM boundaries b
  CROSS JOIN (
    SELECT '0x10' AS address_hex UNION ALL
    SELECT '0x11' UNION ALL
    SELECT '0x12' UNION ALL
    SELECT '0x13' UNION ALL
    SELECT '0x14' UNION ALL
    SELECT '0x15' UNION ALL
    SELECT '0x16' UNION ALL
    SELECT '0x17' UNION ALL
    SELECT '0x18' UNION ALL
    SELECT '0x19'
  ) a
  LEFT JOIN segments s
    ON s.address_hex = a.address_hex
   AND b.ts_epoch >= s.start_ts_epoch
   AND b.ts_epoch <  s.end_ts_epoch
)
SELECT
  replace(datetime(ts_epoch, 'unixepoch', 'localtime'), ' ', 'T') || '+09:00' AS time,
  MAX(CASE WHEN address_hex = '0x10' THEN state END) AS "0x10",
  MAX(CASE WHEN address_hex = '0x11' THEN state END) AS "0x11",
  MAX(CASE WHEN address_hex = '0x12' THEN state END) AS "0x12",
  MAX(CASE WHEN address_hex = '0x13' THEN state END) AS "0x13",
  MAX(CASE WHEN address_hex = '0x14' THEN state END) AS "0x14",
  MAX(CASE WHEN address_hex = '0x15' THEN state END) AS "0x15",
  MAX(CASE WHEN address_hex = '0x16' THEN state END) AS "0x16",
  MAX(CASE WHEN address_hex = '0x17' THEN state END) AS "0x17",
  MAX(CASE WHEN address_hex = '0x18' THEN state END) AS "0x18",
  MAX(CASE WHEN address_hex = '0x19' THEN state END) AS "0x19"
FROM timeline_rows
GROUP BY ts_epoch
ORDER BY ts_epoch;
```

---

## Completed-Session Requirement

This timeline supports completed monitoring sessions only.

The final snapshot for each I²C address does not contain a following observation time. The query therefore requires `monitor_sessions.ended_ts_epoch` to determine where the final state interval ends.

An active session does not yet have a fixed end time.

It would be possible to extend the final interval to the current time, but this would introduce a moving time boundary that must be recalculated whenever the dashboard refreshes.

The current query already:

* reconstructs intervals from periodic snapshots
* calculates the next observation for each address
* combines all interval boundaries
* expands the configured I²C address range
* searches for the active state at each boundary
* pivots the result into separate Grafana fields

The query is already one of the more computationally expensive dashboard queries in the current SQLite-based prototype.

Supporting both a fixed session-end time and a continuously changing current time would increase SQL and dashboard complexity, add repeated recalculation, and blur the existing responsibility boundary between live monitoring and historical analysis.

For the current proof-of-concept scope, active-session visualization remains in the I2C Debugger live UI.

---

## Grafana Panel Settings

Recommended panel type:

* State timeline

Recommended dashboard variable:

* `$session_id`

Panel notes:

* Select a completed monitoring session.
* Display each I²C address as a separate field.
* Configure state-value mappings and colors in Grafana.
* Use the session table or dashboard variable to switch between monitoring runs.
* The panel is intended for retrospective analysis rather than live monitoring.

---

## Fixed Address Range

The displayed I²C address range is currently fixed to:

```text
0x10–0x19
```

The addresses are defined in two places inside the SQL:

1. the address list in `timeline_rows`
2. the final pivot columns in the last `SELECT`

If the monitored address range changes, both sections must be updated manually.

This fixed range matches the current Modular Factory System tabletop configuration.

---

## Time-Zone Assumption

The query currently formats timestamps using SQLite local time and appends the Japan Standard Time offset:

```sql
replace(datetime(ts_epoch, 'unixepoch', 'localtime'), ' ', 'T') || '+09:00'
```

This assumes that the SQLite host is configured for Japan Standard Time.

If the dashboard is deployed in another time zone, the appended offset must be changed or removed.

A future implementation could instead return Unix epoch values directly and allow Grafana to handle time-zone conversion.

---

## Performance Characteristics

The query operates on periodic node snapshots rather than only on state-change events.

Repeated snapshots with the same state are therefore processed as separate short intervals.

For example:

```text
10:00 READY
10:01 READY
10:02 READY
```

is internally reconstructed as:

```text
10:00–10:01 READY
10:01–10:02 READY
10:02–session end READY
```

Grafana normally renders adjacent intervals with the same state as a continuous visual region.

However, query cost increases with:

* monitoring-session duration
* polling frequency
* number of node snapshots
* number of configured I²C addresses
* number of generated timeline boundaries

The current implementation is intended for tabletop demonstrations and bounded monitoring sessions rather than long-running production history.

---

## Current Scope and Limitations

The current panel assumes:

* a completed monitoring session
* a confirmed `ended_ts_epoch`
* node snapshots recorded by the I2C Debugger
* a fixed address range from `0x10` to `0x19`
* a local SQLite data source
* Japan Standard Time formatting
* bounded prototype-scale monitoring sessions
* no continuously moving current-time boundary
* no production-scale historical database

These constraints are intentional for the current proof-of-concept scope.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Software overview → [`../../../README.md`](../../../README.md)
* Recent Sessions panel → [`../Recent Sessions/`](../Recent%20Sessions/)
* Latest Events panel → [`../Latest Events/`](../Latest%20Events/)
* Errors / ESTOP panel → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* Architecture Decision Log → [`../../../../ADL/`](../../../../ADL/)
