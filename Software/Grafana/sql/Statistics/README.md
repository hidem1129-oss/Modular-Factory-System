# Statistics

<img width="1231" height="441" alt="image" src="https://github.com/user-attachments/assets/d503331e-2af3-4771-af0b-dd471731f32f" />


**

---

## Purpose

---

## SQL
```sql
SELECT
  port_id,

  ROUND(MAX(current_mA), 2) AS max_current_mA,
  ROUND(MIN(current_mA), 2) AS min_current_mA,
  ROUND(MAX(current_mA) - MIN(current_mA), 2) AS p2p_current_mA,

  ROUND(MAX(power_mW), 2) AS max_power_mW,
  ROUND(MIN(power_mW), 2) AS min_power_mW,
  ROUND(MAX(power_mW) - MIN(power_mW), 2) AS p2p_power_mW

FROM power_port_snapshots
WHERE ts_epoch BETWEEN (${__from:date:seconds}) AND (${__to:date:seconds})
GROUP BY port_id
ORDER BY port_id
```

---

## Note


---

## Grafana Panel Settings

Recommended panel type:

- Table


