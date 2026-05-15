# Errors / ESTOP

<img width="1242" height="449" alt="image" src="https://github.com/user-attachments/assets/97970d42-69b5-4771-be76-b9a1dcd1c7c2" />


*Grafana Table panel showing recent ERROR and ESTOP state change events.*

---

## Purpose

This panel lists recent error-related state change events recorded by the I2C Debugger.

It shows:

- when an error-related state transition occurred
- which I²C address entered an error or emergency stop state
- the previous state
- the new state

This is useful for quickly checking abnormal events during or after a monitoring session.

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

## Note

The `error_count` column shows how many ERROR or ESTOP transitions occurred for each I²C address within the selected session.

When `$session_id` is set to `__all`, the count is calculated across all matching sessions.

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

- Use $session_id as a dashboard variable.
- Sort by event_time descending.
- Limit the result count if the table becomes too large.
