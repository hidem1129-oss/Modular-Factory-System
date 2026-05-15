# Latest Events

`screen shot`

*This Timeline panel showing*

---

## Purpose

This panel visualizes ...

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

...

---

## Grafana Panel Settings

Recommended panel type:

- Table
