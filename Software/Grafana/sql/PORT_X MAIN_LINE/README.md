# PORT_X / MAIN_LINE

<img width="1237" height="454" alt="image" src="https://github.com/user-attachments/assets/dcf77cd9-ea83-4f6a-b24d-c1525713b750" />

*This time series visualizes the power status of the target port. This is for port 5.*

---

## Purpose


---

## SQL

```sql
SELECT
  ts_epoch AS time,
  bus_voltage_V,
  current_mA,
  power_mW
FROM power_port_snapshots
WHERE ts_epoch BETWEEN (${__from} / 1000.0) AND (${__to} / 1000.0)
  AND port_id = 'XXX'
ORDER BY ts_epoch
```

Replace XXX with PORT_X or MAIN_LINE to specify which port to monitor.

Port number has 1 to 8.

---

## Note

---

## Grafana Panel Settings

Recommended panel type:

- Time series

Notes:
