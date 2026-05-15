# Power consumption & MAX ranking 

<img width="1235" height="449" alt="image" src="https://github.com/user-attachments/assets/6e0a1a69-e091-48be-b2ae-937149d557fe" />

*This table panel showing average and maximum power consumption for each monitored power port.*

---

## Purpose

This panel compares power consumption between monitored power ports.

It shows:

- average power consumption for each port
- maximum power consumption for each port
- which branch or main line consumed the most power during the selected time range

This is useful for identifying high-load ports and comparing electrical load between modules during a demo run.

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

This query reads from the `power_port_snapshots` table.

The result is grouped by `port_id`, so each row represents one monitored power line.

`avg_power_mW` shows the average power consumption during the selected Grafana time range.

`max_power_mW` shows the highest recorded power value during the same range.

The table is sorted by average power consumption in descending order.

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

- Use the Grafana time range to select the period to analyze.
- Apply milliwatt units to avg_power_mW and max_power_mW.
- Sort by avg_power_mW or max_power_mW depending on whether average load or peak load is more important.

