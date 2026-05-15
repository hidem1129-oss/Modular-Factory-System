# Latest Events

<img width="1237" height="454" alt="image" src="https://github.com/user-attachments/assets/cee0038d-e305-48ea-9dc8-8b8a058c1150" />

*This panel showing the latest I²C node state change events.*

---

## Purpose

This panel lists recent state change events recorded by the I2C Debugger.

It shows:

- when each state transition occurred
- which I²C address changed state
- the previous state
- the new state

This is useful for quickly checking recent activity during or after a monitoring session.

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

## Note

This query reads from the `event_logs` table and filters records where `event_type` is `STATE_CHANGE`.

When `$session_id` is set to `__all`, the panel shows recent events across all sessions.  
Otherwise, it shows events only for the selected monitoring session.

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

- Use `$session_id` as a dashboard variable.
- Sort by `event_time` descending.
- Limit the result count if the table becomes too large.
