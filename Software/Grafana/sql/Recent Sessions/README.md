# Recent Sessions

<img width="1246" height="455" alt="image" src="https://github.com/user-attachments/assets/c6293db3-47b7-4d58-a770-1907e2b8d40e" />

*This table displays an overview of each session, including its start and end times.*

---

## Purpose

This panel lists recent monitoring sessions recorded by the I2C Debugger.

It shows:

- when each monitoring session started
- when each monitoring session ended
- session duration
- reader mode
- polling interval
- session result
- session ID used for filtering other panels

This panel is useful as an entry point for selecting a session and reviewing its node timeline, events, and power monitor values.

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

## Note

The `from_ms` and `to_ms` fields are helper values for Grafana data links or dashboard time-range navigation.

They are calculated with a small margin:

- `from_ms` starts 10 seconds before the session start
- `to_ms` ends 10 seconds after the session end

This makes it easier to open another dashboard view focused on the selected monitoring session.

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

- Use this panel as a session selector or navigation table.
- `session_link` can be used as a data link value for setting `$session_id`.
- `from_ms` and `to_ms` can be used to update the dashboard time range.
- Hide helper columns such as `from_ms` and `to_ms` if they are only used for data links.
