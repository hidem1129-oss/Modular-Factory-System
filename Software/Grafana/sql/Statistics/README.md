# Statistics

<img width="1231" height="441" alt="image" src="https://github.com/user-attachments/assets/d503331e-2af3-4771-af0b-dd471731f32f" />


*This table panel showing current and power statistics for each monitored power port.*

---

## Purpose

This panel summarizes current and power statistics for each monitored power port.

It shows:

- maximum current
- minimum current
- peak-to-peak current difference
- maximum power
- minimum power
- peak-to-peak power difference

This is useful for checking how much each power line fluctuated during the selected time range.

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

This query reads from the `power_port_snapshots` table and groups the result by `port_id`.

The `p2p_current_mA` column shows the difference between the maximum and minimum current during the selected Grafana time range.

The `p2p_power_mW` column shows the difference between the maximum and minimum power during the same range.

These values are useful for identifying ports with large load changes.

---

## Grafana Panel Settings

Recommended panel type:

- Table

Notes:

- Use the Grafana time range to select the period to analyze.
- Apply milliamp units to current columns.
- Apply milliwatt units to power columns.
- Sort by `p2p_current_mA` or `p2p_power_mW` when looking for ports with large fluctuations.
