# Latest Events

<img width="1237" height="454" alt="image" src="https://github.com/user-attachments/assets/cee0038d-e305-48ea-9dc8-8b8a058c1150" />


*This Table panel showing...*

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
