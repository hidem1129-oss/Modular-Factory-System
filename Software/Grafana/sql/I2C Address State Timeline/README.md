# I2C Address State Timeline

`screen shot`

---

## Purpose

This panel visualizes the following over time:

- Which device addresses are connected
- What is the status of the connected devices?

This information is displayed chronologically.

---

## SQL 

```text
WITH snaps AS (
  SELECT
    session_id,
    address_hex,
    state,
    ts_epoch,
    LEAD(ts_epoch) OVER (
      PARTITION BY session_id, address_hex
      ORDER BY ts_epoch
    ) AS next_ts_epoch
  FROM node_snapshots
  WHERE session_id = '$session_id'
),
sess AS (
  SELECT
    session_id,
    ended_ts_epoch
  FROM monitor_sessions
  WHERE session_id = '$session_id'
),
segments AS (
  SELECT
    s.address_hex,
    s.state,
    s.ts_epoch AS start_ts_epoch,
    CASE
      WHEN s.next_ts_epoch IS NOT NULL THEN s.next_ts_epoch
      ELSE ms.ended_ts_epoch
    END AS end_ts_epoch
  FROM snaps s
  JOIN sess ms
    ON ms.session_id = s.session_id
  WHERE ms.ended_ts_epoch IS NOT NULL
),
boundaries AS (
  SELECT start_ts_epoch AS ts_epoch FROM segments
  UNION
  SELECT end_ts_epoch AS ts_epoch FROM segments
),
timeline_rows AS (
  SELECT
    b.ts_epoch,
    a.address_hex,
    s.state
  FROM boundaries b
  CROSS JOIN (
    SELECT '0x10' AS address_hex UNION ALL
    SELECT '0x11' UNION ALL
    SELECT '0x12' UNION ALL
    SELECT '0x13' UNION ALL
    SELECT '0x14' UNION ALL
    SELECT '0x15' UNION ALL
    SELECT '0x16' UNION ALL
    SELECT '0x17' UNION ALL
    SELECT '0x18' UNION ALL
    SELECT '0x19'
  ) a
  LEFT JOIN segments s
    ON s.address_hex = a.address_hex
   AND b.ts_epoch >= s.start_ts_epoch
   AND b.ts_epoch <  s.end_ts_epoch
)
SELECT
  replace(datetime(ts_epoch, 'unixepoch', 'localtime'), ' ', 'T') || '+09:00' AS time,
  MAX(CASE WHEN address_hex = '0x10' THEN state END) AS "0x10",
  MAX(CASE WHEN address_hex = '0x11' THEN state END) AS "0x11",
  MAX(CASE WHEN address_hex = '0x12' THEN state END) AS "0x12",
  MAX(CASE WHEN address_hex = '0x13' THEN state END) AS "0x13",
  MAX(CASE WHEN address_hex = '0x14' THEN state END) AS "0x14",
  MAX(CASE WHEN address_hex = '0x15' THEN state END) AS "0x15",
  MAX(CASE WHEN address_hex = '0x16' THEN state END) AS "0x16",
  MAX(CASE WHEN address_hex = '0x17' THEN state END) AS "0x17",
  MAX(CASE WHEN address_hex = '0x18' THEN state END) AS "0x18",
  MAX(CASE WHEN address_hex = '0x19' THEN state END) AS "0x19"
FROM timeline_rows
GROUP BY ts_epoch
ORDER BY ts_epoch;
```
---

## Note

Visualization is not possible until the end time is confirmed.
