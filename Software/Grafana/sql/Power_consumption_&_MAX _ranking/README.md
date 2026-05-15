#  Power consumption & MAX ranking 

<img width="1235" height="449" alt="image" src="https://github.com/user-attachments/assets/6e0a1a69-e091-48be-b2ae-937149d557fe" />

**

---

## Purpose

---

## SQL
```sql
SELECT
  port_id,
  ROUND(AVG(power_mW), 2) AS avg_power_mW,
  ROUND(MAX(power_mW), 2) AS max_power_mW
FROM power_port_snapshots
WHERE ts_epoch BETWEEN (${__from:date:seconds}) AND (${__to:date:seconds})
GROUP BY port_id
ORDER BY avg_power_mW DESC
```

---

## Note

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

