# Power Consumption & MAX Ranking

<img width="1235" height="449" alt="Grafana table comparing average and maximum calculated power values for monitored power channels" src="https://github.com/user-attachments/assets/6e0a1a69-e091-48be-b2ae-937149d557fe" />

*Grafana Table panel comparing average and maximum recorded calculated-power values for each monitored branch port and the main monitored power line.*

---

## Overview

This panel compares calculated power levels across monitored power channels during the selected Grafana time range.

For each `port_id`, it displays:

* the average of the recorded calculated-power samples
* the highest recorded calculated-power sample

The panel is intended for comparing typical and peak electrical load between branch ports during a demonstration, test run, or selected monitoring period.

`MAIN_LINE` is also included as an overall-load reference.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* Which branch had the highest average recorded power?
* Which branch produced the highest recorded power sample?
* Which ports remained lightly loaded during the selected period?
* Did one branch show a high peak but a relatively low average?
* Did one branch remain consistently more heavily loaded than the others?
* How did the branch-level values compare with the MAIN_LINE reference?
* Which port may warrant closer inspection in the time-series view?

The panel provides a summary comparison.

It does not show when the values changed or what caused the electrical load.

---

## Displayed Values

The query returns one row for each monitored `port_id`.

| Field          | Meaning                                                                             | Unit       |
| -------------- | ----------------------------------------------------------------------------------- | ---------- |
| `port_id`      | Monitored branch port or main-line identifier                                       | —          |
| `avg_power_mW` | Arithmetic mean of the recorded calculated-power samples in the selected time range | Milliwatts |
| `max_power_mW` | Highest recorded calculated-power sample in the selected time range                 | Milliwatts |

The power values are not independently measured channels.

The I2C Debugger calculates `power_mW` from the acquired voltage and current values before storing each snapshot in SQLite.

This panel then aggregates those stored calculated values.

---

## Data Source

The query reads from:

```text
power_port_snapshots
```

The table contains timestamped electrical snapshots for:

* `PORT_1` to `PORT_8`
* `MAIN_LINE`

Each snapshot includes acquired voltage and current values together with a power value calculated by the I2C Debugger.

Grafana does not:

* communicate directly with the Power Monitor Board
* recalculate power from voltage and current
* determine whether a recorded value is normal or abnormal
* identify the operational cause of a high-load condition

---

## Port Comparison Model

The current monitoring model contains two different observation levels.

### Branch Ports

```text
PORT_1–PORT_8
└─ Individual monitored branch power lines
```

These channels can be compared with one another to identify differences in branch-level electrical load.

### Main Line

```text
MAIN_LINE
└─ Overall monitored electrical behavior
```

`MAIN_LINE` is included in the same result table as a system-level reference.

It should not always be interpreted as directly comparable to one individual branch.

A higher MAIN_LINE value may be expected because it represents a different observation level from `PORT_1` to `PORT_8`.

The query does not calculate or prove that:

```text
MAIN_LINE = PORT_1 + PORT_2 + ... + PORT_8
```

Any comparison should account for the actual measurement arrangement, sampling timing, and system configuration.

---

## SQL Processing Model

The query performs five main operations:

| Step                    | Responsibility                                                      |
| ----------------------- | ------------------------------------------------------------------- |
| Source selection        | Reads calculated-power snapshots from `power_port_snapshots`        |
| Time-range filtering    | Includes only snapshots inside the selected Grafana dashboard range |
| Port grouping           | Groups matching snapshots by `port_id`                              |
| Statistical aggregation | Calculates the average and maximum recorded power for each port     |
| Result ordering         | Sorts the rows by average calculated power in descending order      |

The query produces summary statistics rather than time-series data.

It does not preserve the timestamp of each individual snapshot in the final result.

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

## Grafana Time-Range Filtering

The query uses Grafana’s selected dashboard time range:

```text
${__from:date:seconds}
${__to:date:seconds}
```

These expressions provide the start and end of the Grafana time range as Unix epoch seconds.

Only snapshots whose `ts_epoch` values fall inside that period are included in the aggregation.

The selected time range therefore directly affects:

* which samples are included
* the calculated average
* the recorded maximum
* the resulting ranking order

Changing the dashboard range may produce a different comparison.

---

## Relationship with Recent Sessions

The Recent Sessions panel can provide the dashboard time range associated with one monitoring run.

```text
Recent Sessions
        ↓
from_ms and to_ms
        ↓
Grafana dashboard time range
        ↓
Power Consumption & MAX Ranking
```

This query uses the resulting time range but does not directly filter by `$session_id`.

For the current proof-of-concept workflow, bounded time ranges are used to align the ranking with the intended demonstration or test run.

However, the SQL itself does not verify that every included row belongs to one specific monitoring session.

If sessions overlap or occur very close together, records from more than one run may be included.

---

## Average-Power Interpretation

The query calculates:

```sql
AVG(power_mW)
```

This is the arithmetic mean of the stored power samples for each `port_id`.

For example:

```text
100 mW
200 mW
300 mW
```

produces:

```text
avg_power_mW = 200 mW
```

Each stored sample has equal weight in the calculation.

If the recording interval is stable, this provides a useful approximation of typical power level during the selected period.

However, it is not a duration-weighted average.

If sampling intervals vary, records are missing, or polling is interrupted, the result may not represent a strict time-weighted mean.

The value should therefore be interpreted as:

```text
average of the recorded calculated-power samples
```

rather than an exact continuous-time average.

---

## Maximum-Power Interpretation

The query calculates:

```sql
MAX(power_mW)
```

This returns the highest stored calculated-power value for each port inside the selected time range.

It represents:

```text
highest recorded calculated-power sample
```

It does not necessarily represent the absolute physical peak that occurred.

A short-duration peak may not be captured if it occurs between stored snapshots.

The maximum value is therefore limited by:

* power-monitor polling frequency
* snapshot storage interval
* successful acquisition of voltage and current
* the selected Grafana time range

For detailed inspection of when the maximum occurred, use the corresponding PORT_X or MAIN_LINE time-series panel.

---

## Average and Maximum Answer Different Questions

The two aggregated values describe different load characteristics.

```text
Average calculated power
└─ What power level was typical across the recorded samples?

Maximum recorded calculated power
└─ What was the highest stored sample?
```

A port can have:

* a high average and high maximum
* a low average but a short high peak
* a moderate average with little variation
* missing or sparse data that affects interpretation

The panel should therefore not rank ports using only one value without considering the other.

---

## Ranking Behavior

The SQL sorts rows using:

```sql
ORDER BY avg_power_mW DESC
```

The channel with the highest average calculated power appears first.

The displayed order is therefore an average-load ranking, not a peak-load ranking.

A port with the highest `max_power_mW` may appear lower in the table if its average remains relatively small.

If peak load is more important for a particular investigation, the query can instead use:

```sql
ORDER BY max_power_mW DESC
```

Alternatively, the Grafana table can be sorted interactively by the maximum-value column.

---

## Power Is Not Energy Consumption

Despite the current panel name, the query does not calculate accumulated energy consumption.

Power and energy are different quantities:

```text
Power
└─ Rate of energy use at a given time

Energy
└─ Power accumulated over time
```

The current query calculates:

* average recorded power
* maximum recorded power

It does not integrate power over time.

An energy-consumption calculation would require the time interval between samples to be considered, for example:

```text
Energy ≈ Σ(power × time interval)
```

Therefore, this panel should be interpreted as a power-level comparison rather than a total energy-consumption ranking.

The existing title is retained as the current dashboard name, but the README uses more precise terminology when describing the query output.

---

## Relationship with the Time-Series Panels

### PORT_X / MAIN_LINE Power Monitor

```text
Time-series panel
└─ When and how did the electrical values change?
```

### Power Consumption & MAX Ranking

```text
Ranking panel
├─ Which port had the highest average recorded power?
└─ Which port had the highest recorded peak?
```

The ranking panel identifies channels that may deserve closer attention.

The time-series panel can then be used to examine when the relevant values occurred and whether they were sustained or temporary.

---

## Operational Interpretation

A high average or maximum value does not by itself indicate a fault.

Possible explanations may include:

* an intentionally active actuator
* a branch supplying a larger load
* a temporary startup current
* a mechanical load increase
* a stalled or obstructed mechanism
* a different demonstration sequence
* a different number of active modules
* measurement or data-quality issues

The panel does not determine which explanation applies.

Its role is to identify relative differences that can be investigated using:

* PORT_X / MAIN_LINE time-series panels
* I2C Address State Timeline
* Latest Events
* Errors / ESTOP
* Recent Sessions metadata
* the physical system configuration
* the demonstration sequence

Correlation between a high power value and another recorded event does not by itself prove causation.

---

## Missing and Uneven Data

The SQL includes only stored snapshots that match the selected time range.

If one port has fewer valid records than another, its average and maximum are calculated from the available rows only.

The current query does not display:

* sample count
* missing-record count
* acquisition-failure count
* data-coverage percentage
* time-weighted coverage

This means that two rows may be based on different numbers of samples.

For the current bounded proof-of-concept sessions, the comparison is intended as a practical summary rather than a formal metrology result.

---

## Result Precision

The query applies:

```sql
ROUND(..., 2)
```

to both aggregated values.

The displayed results therefore use two decimal places.

Rounding is applied after the average or maximum has been calculated.

It affects the displayed value but not which source samples are included.

Rows are ordered using the rounded alias:

```sql
ORDER BY avg_power_mW DESC
```

Ports with very similar unrounded averages may therefore appear equal after display rounding.

This precision is sufficient for the current dashboard presentation.

---

## Performance Characteristics

The query groups all matching power snapshots by `port_id`.

Query cost increases with:

* selected time-range length
* number of stored snapshots
* power-monitor polling frequency
* number of monitored channels
* amount of historical data retained

Because the result contains only one row per `port_id`, the final table remains small even when many source snapshots are processed.

The primary cost is reading and aggregating the matching snapshot records.

The current implementation is intended for bounded tabletop demonstrations and selected historical periods rather than unrestricted long-duration aggregation.

---

## Grafana Panel Settings

Recommended panel type:

* Table

Recommended configuration:

* Use the Grafana dashboard range to select the analysis period.
* Apply milliwatt units to `avg_power_mW` and `max_power_mW`.
* Display `MAIN_LINE` as an overall reference rather than an ordinary branch competitor.
* Keep the default table sorted by `avg_power_mW` when typical load is the primary concern.
* Sort by `max_power_mW` when peak load is more important.
* Use value mappings, thresholds, or data bars only when their interpretation is clearly defined.
* Compare suspicious rows with the corresponding PORT_X / MAIN_LINE time-series panel.
* Use a bounded time range to avoid mixing unrelated monitoring runs.

---

## Current Scope and Limitations

The current panel assumes:

* snapshots stored in `power_port_snapshots`
* calculated power stored in milliwatts
* timestamps stored as Unix epoch seconds
* Grafana time-range filtering
* grouping by `port_id`
* arithmetic sample averages
* maximum values selected from recorded samples
* average-power-based default sorting
* branch identifiers from `PORT_1` to `PORT_8`
* a main-line identifier of `MAIN_LINE`
* bounded prototype-scale monitoring periods

The panel does not:

* calculate accumulated energy consumption
* calculate a duration-weighted average
* identify when the maximum occurred
* display sample counts
* assess data completeness
* directly filter by `$session_id`
* distinguish expected load from abnormal load
* detect electrical faults
* determine the cause of a high value
* issue control commands

These constraints are intentional for the current proof-of-concept scope.

---

## Possible Future Extensions

Possible extensions include:

* calculating energy consumption from power and elapsed time
* displaying the timestamp of each maximum value
* showing sample counts per port
* showing data-coverage indicators
* calculating duration-weighted average power
* separating MAIN_LINE from the branch ranking
* filtering directly by monitoring-session ID
* adding a selectable average or maximum ranking mode
* linking each row to its corresponding time-series panel
* comparing two monitoring sessions
* identifying configurable threshold exceedances

These extensions are not required for the current tabletop prototype.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* Recent Sessions → [`../Recent Sessions/`](../Recent%20Sessions/)
* PORT_X / MAIN_LINE Power Monitor → [`../PORT_X MAIN_LINE/`](../PORT_X%20MAIN_LINE/)
* Statistics → [`../Statistics/`](../Statistics/)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Latest Events → [`../Latest Events/`](../Latest%20Events/)
* Errors / ESTOP → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Power Monitor Board → [`../../../../Hardware/Power_Monitor_Board/`](../../../../Hardware/Power_Monitor_Board/)
* Software overview → [`../../../README.md`](../../../README.md)
