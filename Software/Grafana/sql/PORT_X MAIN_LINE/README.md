# # PORT_X / MAIN_LINE Power Monitor

<img width="1237" height="454" alt="image" src="https://github.com/user-attachments/assets/dcf77cd9-ea83-4f6a-b24d-c1525713b750" />

*This time series visualizes the power status of the target port. This is for port 5.*

---

## Purpose

This panel visualizes power monitor values for a selected power port.

It shows:

- bus voltage
- current
- power
- changes over time within the selected Grafana time range

This is useful for checking how much electrical load is applied to each branch or to the main power line during a demo run.

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
---

## Note

Replace `XXX` in the SQL with the target `port_id`.

Examples:

- `PORT_1`
- `PORT_5`
- `PORT_8`
- `MAIN_LINE`

`PORT_1` to `PORT_8` represent branch power lines.  
`MAIN_LINE` represents the main monitored power line.

The values are recorded by the I2C Debugger from the Power Monitor Board and stored in the `power_port_snapshots` table.

---

## Grafana Panel Settings

Recommended panel type:

- Time series

Notes:

- Use this query once per target port, or duplicate the panel and change `port_id`.
- Display `bus_voltage_V`, `current_mA`, and `power_mW` as separate series.
- Apply appropriate units in Grafana:
  - `bus_voltage_V`: volts
  - `current_mA`: milliamps
  - `power_mW`: milliwatts
- Use Grafana time range variables `${__from}` and `${__to}` to filter the displayed period.

---

## Related Documents

- Grafana overview → [`../../README.md`](../../README.md)
- I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
- Power Monitor Board → [`../../../../Hardware/Power_Monitor_Board/`](../../../../Hardware/Power_Monitor_Board/)
