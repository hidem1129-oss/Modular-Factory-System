# Statistics

<img width="1231" height="441" alt="Grafana table comparing recorded maximum, minimum, and range values for current and calculated power across monitored channels" src="https://github.com/user-attachments/assets/d503331e-2af3-4771-af0b-dd471731f32f" />

*Grafana Table panel comparing the recorded maximum, minimum, and maximum-to-minimum range of current and calculated power for each monitored channel.*

---

## Overview

The panel is intended for identifying channels with a wide recorded value range during a demonstration, test run, or selected monitoring period.

For every `port_id`, it displays:

* maximum recorded current
* minimum recorded current
* maximum-to-minimum current difference
* maximum recorded calculated power
* minimum recorded calculated power
* maximum-to-minimum calculated-power difference

The panel is intended for identifying channels that showed a large recorded change range during a demonstration, test run, or selected monitoring period.

It summarizes the extent of the recorded values.

It does not show when the changes occurred, how often they occurred, or what caused them.

---

## Questions This Panel Answers

This panel helps answer questions such as:

* What was the highest recorded current for each channel?
* What was the lowest recorded current for each channel?
* How wide was the recorded current range?
* What was the highest recorded calculated-power value?
* What was the lowest recorded calculated-power value?
* Which branch showed the largest maximum-to-minimum difference?
* Did a channel remain within a narrow recorded range?
* Which channel may warrant closer inspection in the time-series view?
* How did branch-level ranges compare with the MAIN_LINE reference?

The panel provides a range-based summary across channels.

It does not determine whether a wide range is expected, abnormal, or operationally significant.

---

## Displayed Values

The query returns one row for each monitored `port_id`.

| Field            | Meaning                                                                     | Unit       |
| ---------------- | --------------------------------------------------------------------------- | ---------- |
| `port_id`        | Monitored branch-port or main-line identifier                               | —          |
| `max_current_mA` | Highest recorded current sample in the selected time range                  | Milliamps  |
| `min_current_mA` | Lowest recorded current sample in the selected time range                   | Milliamps  |
| `p2p_current_mA` | Difference between the highest and lowest recorded current samples          | Milliamps  |
| `max_power_mW`   | Highest recorded calculated-power sample in the selected time range         | Milliwatts |
| `min_power_mW`   | Lowest recorded calculated-power sample in the selected time range          | Milliwatts |
| `p2p_power_mW`   | Difference between the highest and lowest recorded calculated-power samples | Milliwatts |

The current values are acquired measurements.

The power values are not independently measured channels.

The I2C Debugger calculates `power_mW` from the acquired voltage and current values before storing each snapshot in SQLite.

This panel calculates summary statistics from those stored snapshots.

---

## Data Source

The query reads from:

```text
power_port_snapshots
```

The table contains timestamped electrical snapshots for:

* `PORT_1` to `PORT_8`
* `MAIN_LINE`

Each stored snapshot includes:

* measurement timestamp
* monitored channel identifier
* acquired bus voltage
* acquired current
* power calculated by the I2C Debugger

Grafana reads the stored records and performs the requested aggregation.

It does not directly communicate with the Power Monitor Board.

---

## Channel Comparison Model

The current monitoring model contains two observation levels.

### Branch Ports

```text
PORT_1–PORT_8
└─ Individual monitored branch power lines
```

The branch rows can be compared to identify differences in recorded current and calculated-power ranges.

### Main Line

```text
MAIN_LINE
└─ Overall monitored electrical behavior
```

`MAIN_LINE` is included as a system-level reference.

It should not always be interpreted as directly comparable to an individual branch because it represents a different observation level.

A large MAIN_LINE range does not by itself indicate that the main line is less stable than a branch.

The query also does not calculate or prove that:

```text
MAIN_LINE = PORT_1 + PORT_2 + ... + PORT_8
```

Any comparison should account for the actual monitoring arrangement, sampling timing, connected loads, and system configuration.

---

## SQL Processing Model

The query performs five main operations:

| Step                 | Responsibility                                                           |
| -------------------- | ------------------------------------------------------------------------ |
| Source selection     | Reads electrical snapshots from `power_port_snapshots`                   |
| Time-range filtering | Includes only snapshots inside the selected Grafana range                |
| Channel grouping     | Groups matching snapshots by `port_id`                                   |
| Range aggregation    | Calculates recorded maximum, minimum, and maximum-to-minimum differences |
| Result ordering      | Sorts the rows by `port_id`                                              |

The query produces one summary row per monitored channel.

It does not return the timestamp of any individual maximum or minimum value.

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

## Grafana Time-Range Filtering

The query uses Grafana’s selected dashboard time range:

```text
${__from:date:seconds}
${__to:date:seconds}
```

These expressions provide the dashboard start and end times as Unix epoch seconds.

Only snapshots whose `ts_epoch` values fall inside the selected period are included.

Changing the dashboard range may change:

* the recorded maximum
* the recorded minimum
* the maximum-to-minimum difference
* which channels appear to have the widest range

The results should therefore always be interpreted in relation to the selected analysis period.

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
Statistics
```

This query uses the resulting time range but does not directly filter by `$session_id`.

For the current proof-of-concept workflow, bounded time ranges are used to align the statistics with the intended demonstration or test run.

However, the SQL itself does not verify that every included snapshot belongs to one specific monitoring session.

If sessions overlap or occur very close together, records from more than one run may be included in the same aggregation.

---

## Maximum and Minimum Interpretation

The query calculates the maximum and minimum stored samples for each channel.

For current:

```sql
MAX(current_mA)
MIN(current_mA)
```

For calculated power:

```sql
MAX(power_mW)
MIN(power_mW)
```

These values represent:

```text
highest recorded sample
lowest recorded sample
```

They do not necessarily represent the absolute physical maximum or minimum that occurred.

Short-duration peaks or dips may not be captured if they occur between stored snapshots.

The results are therefore limited by:

* power-monitor polling frequency
* snapshot storage interval
* successful acquisition of voltage and current
* the selected Grafana time range
* the available stored records

---

## Recorded Range Interpretation

The query calculates the maximum-to-minimum difference for current:

```sql
MAX(current_mA) - MIN(current_mA)
```

and for calculated power:

```sql
MAX(power_mW) - MIN(power_mW)
```

For example, if the stored current samples are:

```text
100 mA
140 mA
350 mA
120 mA
```

then:

```text
max_current_mA = 350 mA
min_current_mA = 100 mA
p2p_current_mA = 250 mA
```

The `p2p` fields should therefore be interpreted as:

```text
recorded maximum-to-minimum range
```

They describe how far apart the highest and lowest recorded values were during the selected period.

---

## Meaning of `p2p`

The column names use `p2p` as a compact representation of the maximum-to-minimum difference.

In this panel, `p2p` does not imply that the data forms a periodic waveform.

The query does not identify individual positive and negative waveform peaks.

It only calculates:

```text
recorded maximum − recorded minimum
```

within the selected time range.

This is a range statistic calculated from discrete stored snapshots.

---

## A Wide Range Does Not Describe the Change Pattern

A large `p2p_current_mA` or `p2p_power_mW` means that the highest and lowest stored values were far apart.

It does not show how the values moved between those points.

The same maximum-to-minimum difference can result from different behaviors.

```text
Case A
└─ One brief high sample

Case B
└─ Repeated movement between low and high values

Case C
└─ One sustained transition from a low level to a high level

Case D
└─ Different operating phases within the selected period
```

This panel cannot distinguish these cases.

It does not calculate:

* change frequency
* rate of change
* oscillation frequency
* time spent near the maximum
* time spent near the minimum
* standard deviation
* instability
* transition count

The corresponding time-series panel should be used to inspect the shape and timing of the recorded values.

---

## Range Is Not the Same as Variability

A large maximum-to-minimum range does not necessarily mean that a channel was continuously unstable.

For example, one startup-current sample may create a large range even if the remaining samples are stable.

Likewise, a channel may fluctuate frequently inside a narrow range and still show a relatively small `p2p` value.

The panel therefore describes:

```text
extent of the recorded values
```

rather than a complete statistical measure of variability or stability.

A broader variability analysis would require additional statistics such as:

* standard deviation
* percentiles
* median
* interquartile range
* change frequency
* duration-weighted distributions

These are outside the current panel scope.

---

## Current and Calculated-Power Ranges

Current and calculated power are related but should not be interpreted as identical statistics.

```text
Current range
└─ Difference between the highest and lowest acquired current samples

Calculated-power range
└─ Difference between the highest and lowest stored voltage × current results
```

Because calculated power depends on both voltage and current, the power range does not necessarily scale only with the current range.

A voltage change may also affect the calculated-power values.

This panel displays both statistics so the recorded electrical range can be considered from more than one perspective.

---

## Relationship with Other Power Panels

### PORT_X / MAIN_LINE Power Monitor

```text
Time-series panel
└─ When and how did voltage, current, and calculated power change?
```

### Power Consumption & MAX Ranking

```text
Average and maximum panel
├─ Which branch had the highest average recorded power?
└─ Which branch produced the highest recorded power sample?
```

### Statistics

```text
Recorded-range panel
├─ What were the highest and lowest recorded values?
└─ How wide was the maximum-to-minimum range?
```

The Statistics panel can identify a channel with a wide recorded range.

The corresponding time-series panel can then show when the high and low values occurred and whether the difference resulted from a brief event or a sustained operational change.

---

## Operational Interpretation

A large recorded range does not by itself indicate a fault.

Possible explanations include:

* normal startup current
* actuator activation and deactivation
* different process phases
* changing mechanical load
* different numbers of active modules
* intentional operating-mode changes
* stalled or obstructed mechanisms
* voltage variation
* measurement or data-quality issues

The panel does not determine which explanation applies.

Its role is to identify channels whose recorded value ranges may warrant closer investigation using:

* PORT_X / MAIN_LINE time-series panels
* Power Consumption & MAX Ranking
* I2C Address State Timeline
* Latest Events
* Errors / ESTOP
* Recent Sessions metadata
* the physical system configuration
* the demonstration sequence

Correlation between a wide electrical range and another event does not by itself prove causation.

---

## Missing and Uneven Data

The SQL includes only stored snapshots that match the selected time range.

If one channel has fewer valid records than another, its maximum, minimum, and range are calculated from the available rows only.

The current query does not display:

* sample count
* missing-record count
* acquisition-failure count
* data-coverage percentage
* polling continuity
* time-weighted coverage

Two rows may therefore be based on different numbers or densities of samples.

A channel with sparse data may show a smaller recorded range simply because a short peak or dip was not captured.

For the current bounded proof-of-concept sessions, the table is intended as a practical comparison rather than a formal metrology result.

---

## Result Ordering

The query sorts rows using:

```sql
ORDER BY port_id
```

The table is therefore organized by monitored channel identifier rather than by the size of a statistical value.

This default ordering makes it easy to locate a specific branch or the main line.

When investigating wide recorded ranges, the Grafana table can instead be sorted interactively by:

```text
p2p_current_mA
```

or:

```text
p2p_power_mW
```

Sorting the table changes the presentation order but does not change the calculated values.

---

## Result Precision

The query applies:

```sql
ROUND(..., 2)
```

to all current and calculated-power statistics.

The displayed values therefore use two decimal places.

Rounding is applied after the maximum, minimum, or difference has been calculated.

It does not change which source samples are included.

This precision is sufficient for the current dashboard presentation and tabletop proof-of-concept scope.

---

## Performance Characteristics

The query groups all matching snapshots by `port_id`.

Query cost increases with:

* selected time-range length
* number of stored snapshots
* power-monitor polling frequency
* number of monitored channels
* amount of historical data retained

Because the result contains only one row per `port_id`, the final table remains small even when many source records are processed.

The primary query cost is reading and aggregating the matching snapshots.

The current implementation is intended for bounded demonstration sessions and selected historical periods rather than unrestricted long-duration aggregation.

---

## Grafana Panel Settings

Recommended panel type:

* Table

Recommended configuration:

* Use the Grafana dashboard range to select the analysis period.
* Apply milliamp units to the current columns.
* Apply milliwatt units to the calculated-power columns.
* Keep `port_id` ordering when locating a specific monitored channel.
* Sort by `p2p_current_mA` when comparing current ranges.
* Sort by `p2p_power_mW` when comparing calculated-power ranges.
* Display `MAIN_LINE` as an overall reference rather than an ordinary branch competitor.
* Compare a wide-range row with the corresponding PORT_X / MAIN_LINE time-series panel.
* Use a bounded time range to avoid mixing unrelated monitoring runs.
* Avoid treating the `p2p` columns as complete stability or waveform-quality metrics.

---

## Current Scope and Limitations

The current panel assumes:

* snapshots stored in `power_port_snapshots`
* current stored in milliamps
* calculated power stored in milliwatts
* timestamps stored as Unix epoch seconds
* Grafana time-range filtering
* grouping by `port_id`
* maximum and minimum selected from recorded samples
* `p2p` calculated as recorded maximum minus recorded minimum
* branch identifiers from `PORT_1` to `PORT_8`
* a main-line identifier of `MAIN_LINE`
* bounded prototype-scale monitoring periods

The panel does not:

* show when the maximum occurred
* show when the minimum occurred
* calculate an average value
* calculate standard deviation
* calculate percentiles or median
* calculate change frequency
* distinguish one transient from repeated changes
* assess statistical stability
* assess data completeness
* directly filter by `$session_id`
* distinguish expected behavior from abnormal behavior
* detect electrical faults
* determine the cause of a large range
* issue control commands

These constraints are intentional for the current proof-of-concept scope.

---

## Possible Future Extensions

Possible extensions include:

* displaying the timestamps of maximum and minimum values
* displaying sample counts per channel
* showing data-coverage indicators
* calculating standard deviation
* calculating median and percentile ranges
* counting threshold crossings
* measuring time spent above a configured value
* calculating rate-of-change statistics
* distinguishing transient and sustained changes
* separating MAIN_LINE from the branch comparison
* filtering directly by monitoring-session ID
* linking each row to the corresponding time-series panel
* comparing recorded ranges between two sessions
* identifying configurable range thresholds

These extensions are not required for the current tabletop prototype.

---

## Related Documents

* Grafana overview → [`../../README.md`](../../README.md)
* Recent Sessions → [`../Recent Sessions/`](../Recent%20Sessions/)
* PORT_X / MAIN_LINE Power Monitor → [`../PORT_X MAIN_LINE/`](../PORT_X%20MAIN_LINE/)
* Power Consumption & MAX Ranking → [`../Power_consumption_&_MAX _ranking/`](../Power_consumption_%26_MAX%20_ranking/)
* I2C Address State Timeline → [`../I2C Address State Timeline/`](../I2C%20Address%20State%20Timeline/)
* Latest Events → [`../Latest Events/`](../Latest%20Events/)
* Errors / ESTOP → [`../Errors  ESTOP/`](../Errors%20%20ESTOP/)
* I2C Debugger → [`../../../I2C_Debugger/`](../../../I2C_Debugger/)
* Power Monitor Board → [`../../../../Hardware/Power_Monitor_Board/`](../../../../Hardware/Power_Monitor_Board/)
* Software overview → [`../../../README.md`](../../../README.md)
