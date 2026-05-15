# Recent Sessions

<img width="1246" height="455" alt="image" src="https://github.com/user-attachments/assets/c6293db3-47b7-4d58-a770-1907e2b8d40e" />

*This table displays an overview of each session, including its start and end times.*

---

## Purpose

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



---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

