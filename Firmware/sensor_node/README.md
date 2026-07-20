# sensor_node

This directory contains the device-specific firmware implementation for the analog photo-reflector sensor node used in the Modular Factory System.

The current implementation is tuned to the photo-reflector circuit selected for this project rather than being a fully generic sensor abstraction.

The shared register interface and host-side orchestration can still be reused with other analog sensors when their raw values, polarity, thresholds, and units are converted or calibrated appropriately.

---

## Purpose

The sensor node samples the selected analog photo-reflector circuit and publishes:

- raw ADC value
- filtered ADC value
- detection and transition flags

It is intended for tasks such as:

- object-presence detection
- workpiece arrival detection
- conveyor timing triggers
- orchestration branch conditions
- sensor-value logging

The firmware currently embeds photo-reflector-specific assumptions, including:

- analog input through Pico ADC
- detection when the filtered value falls below a threshold
- hysteresis-based release
- low/high ADC saturation checks
- a continuously enabled emitter LED output
- default values derived from the existing photo-sensor experiment

---

## Reuse Boundary

The current firmware should be understood as:

```text
common sensor-node interface
        +
project-specific photo-reflector interpretation
```

The register and feedback structure can serve as a reusable analog-sensor foundation.

However, direct reuse with another sensor may require changes to:

- ADC channel and GPIO
- signal polarity
- threshold direction
- hysteresis behavior
- filtering parameters
- saturation limits
- emitter or excitation control
- physical-unit conversion

Alternatively, host-side orchestration can convert the published raw and filtered values into application-specific units or decisions.

This allows the firmware to remain useful as a common acquisition node even when the physical sensor or calibration model changes.

---

## Directory Structure

| Path | Responsibility |
|---|---|
| [`src/`](./src/) | Photo-reflector sampling, filtering, detection logic, feedback publication, and profile metadata |
| [`README.md`](./README.md) | Sensor behavior, parameter mapping, feedback mapping, reuse boundary, and limitations |

---

## Common Firmware Relationship

```text
photo-reflector circuit
        ↓ analog voltage
sensor_node
        ↓ callbacks and public API
common/core
        ↓ I²C registers
host orchestration
        ↓ calibration / conversion / decisions
application behavior
```

The common firmware owns:

- I²C register access
- staged-to-active parameter transfer
- READY, BUSY, and ESTOP lifecycle
- command results
- completion and update signaling
- `DATA_READY`
- `UPDATE_CNT`

The sensor node owns:

- ADC acquisition
- oversampling
- filtering
- threshold polarity
- hysteresis
- state-flag generation
- feedback meaning
- photo-emitter output behavior

---

## Current Hardware Interface

| Signal | GPIO / channel | Purpose |
|---|---|---|
| Photo-reflector analog output | GPIO26 / ADC0 | 12-bit ADC input |
| Photo-reflector emitter LED control | GPIO2 | Held high continuously |

Current ADC assumptions:

```text
ADC range: 0–4095
oversampling: 16 samples per published raw value
```

The emitter LED output is initialized high and remains continuously enabled in the current implementation.

---

## Photo-Reflector-Specific Detection Polarity

The selected circuit is interpreted as:

```text
workpiece detected when filtered value <= threshold
```

This polarity is not universal for all photo-reflectors or analog sensors.

Another circuit may produce:

- increasing voltage when an object is present
- decreasing voltage when an object is present
- a non-linear response
- distance-dependent rather than binary behavior

For another sensor, the polarity and conversion logic must be confirmed rather than assumed.

---

## Supported Operation Modes

| `OP_MODE` | Support | Behavior |
|---|---|---|
| `OP_MODE_DEFAULT` | Supported | Falls back to `OP_MODE_SPEED` |
| `OP_MODE_SPEED` | Supported | Exponential moving average filtering |
| `OP_MODE_TIME` | Supported | Simple moving average filtering |
| `OP_MODE_CONTINUOUS` | Not supported | Rejected |
| `OP_MODE_ONE_SHOT` | Not supported | Rejected |

The mode names come from the common interface.

For this sensor node:

- SPEED means faster-response EMA filtering
- TIME means window-based SMA filtering

They do not represent motor speed or operation duration.

---

## Command and Parameter Mapping

| Register | Sensor-node meaning | Current support |
|---|---|---|
| `OP_MODE` | Selects EMA or SMA filtering | Used |
| `SETPOINT0` | Detection threshold | Used |
| `SETPOINT1` | Reserved | Not used |
| `TARGET_TIME` | SMA window length in samples / 10 ms ticks | Used in TIME mode |
| `HOLD_TIME` | Reserved | Not used |
| `LIMIT0` | EMA coefficient in permille | Used in SPEED mode |
| `LIMIT1` | Detection hysteresis width | Used |
| `CMD_DIR` | No sensor meaning | Not used |
| `CMD_MODE` | Reserved | Not used |
| `CMD_USE_SOFTLIMIT` | Reserved | Not used |

---

## Detection Threshold and Hysteresis

The detection threshold is:

```text
threshold_on = SETPOINT0
```

When no object is currently detected:

```text
filtered_value <= threshold_on
        ↓
detected = true
```

The release threshold is:

```text
threshold_off = SETPOINT0 + LIMIT1
```

clamped to the ADC maximum.

When an object is currently detected:

```text
filtered_value >= threshold_off
        ↓
detected = false
```

This hysteresis reduces rapid toggling near the threshold.

---

## SPEED Mode: EMA Filtering

In `OP_MODE_SPEED`, the node applies an exponential moving average.

`LIMIT0` is interpreted as an EMA coefficient in permille:

```text
1–1000 = 0.1%–100.0%
```

Normalization:

- `0` becomes `1`
- values above `1000` are clamped to `1000`

A larger coefficient follows new samples more quickly.

A smaller coefficient smooths noise more strongly but responds more slowly.

The implementation uses Q15 fixed-point arithmetic internally.

---

## TIME Mode: SMA Filtering

In `OP_MODE_TIME`, the node applies a simple moving average.

`TARGET_TIME` is interpreted as the window length.

Current behavior:

- `TARGET_TIME = 0` is rejected
- minimum effective window is 1
- maximum window is 64
- samples are processed through the cooperative 10 ms callback

Approximate averaging interval:

```text
window length × 10 ms
```

Example:

```text
TARGET_TIME = 8
≈ 80 ms moving-average window
```

This timing is approximate rather than hard real-time.

---

## ADC Sampling

Each periodic sensor update reads 16 ADC samples and publishes their arithmetic average as the raw value.

Conceptually:

```text
raw_value =
    sum(16 ADC conversions) / 16
```

This initial oversampling occurs before the selected EMA or SMA filter.

Therefore the processing chain is:

```text
ADC conversions
      ↓ 16-sample average
raw_value
      ↓ EMA or SMA
filtered_value
      ↓ threshold + hysteresis
sensor_state
```

---

## Feedback Mapping

| Register | Meaning |
|---|---|
| `FB_VALUE0` | Oversampled raw ADC value |
| `FB_VALUE1` | Filtered ADC value |
| `FB_VALUE2` | Sensor-state bit field |

Feedback is published through:

```c
core_publish_fb_u16(
    raw_value,
    filtered_value,
    sensor_state
);
```

The values are ADC-domain values, not calibrated distance, reflectance, voltage, or physical units.

---

## Sensor-State Bits

`FB_VALUE2` currently uses:

| Bit | Meaning |
|---:|---|
| 0 | Detection level is active |
| 1 | Rising event: detection changed from inactive to active |
| 2 | Falling event: detection changed from active to inactive |
| 3 | Raw ADC value is near a saturation boundary |
| 4–15 | Reserved |

### Event duration

Rising and falling bits are generated from the previous and current detection level.

They are transient results of the current update, not permanently latched event history.

Host software should poll or monitor update changes if it must observe every transition.

---

## Saturation Detection

The current photo-reflector experiment uses fixed raw ADC saturation thresholds:

```text
low saturation:  raw <= 5
high saturation: raw >= 4090
```

When either condition is true, bit 3 of `FB_VALUE2` is set.

These values are project-specific diagnostics.

They may not be suitable for another sensor, analog front end, supply voltage, or ADC reference.

---

## Current Default Values

The runtime and profile contain defaults derived from the existing photo-sensor experiment.

Current profile defaults include:

| Parameter | Default | Meaning |
|---|---:|---|
| `LIMIT0` | 80 | 8.0% EMA coefficient |
| `TARGET_TIME` | 8 | Approximately 80 ms SMA window |

The sensor implementation also initializes internal fallback values before active configuration is loaded:

| Parameter | Internal initial value |
|---|---:|
| `SETPOINT0` | 3800 |
| `TARGET_TIME` | 8 |
| `LIMIT0` | 250 |
| `LIMIT1` | 120 |

Host-controlled operation should still use the normal write and `LATCH_APPLY` sequence.

These internal values should not be treated as a universal calibration for all units or installations.

---

## LATCH_APPLY Behavior

When `on_latch_apply` runs, the sensor node:

1. reads active values through the common API
2. resolves the operation mode
3. validates the configuration
4. normalizes the filter parameters
5. resets mode-specific filter state
6. publishes the current state

Current getters used:

```c
core_get_op_mode()
core_get_setpoint0()
core_get_target_time()
core_get_limit0()
core_get_limit1()
```

The configuration is validated again during `on_run_start`.

---

## RUN Behavior

The common RUN lifecycle controls periodic measurement.

### RUN start

On an accepted RUN rising edge:

- ADC initialization is confirmed
- active configuration is loaded
- mode and parameters are validated
- periodic sensor processing becomes active
- current feedback is published

### RUN active

During each 10 ms callback:

- one oversampled raw value is acquired
- EMA or SMA filtering is applied
- detection state is updated
- feedback is published

### RUN stop

A host RUN falling edge stops periodic updates and publishes the current state.

### E-STOP

E-STOP stops periodic sensor processing and publishes the current state.

The current firmware does not turn off the photo-emitter LED during E-STOP.

---

## Host-Side Conversion and Calibration

The orchestration layer may convert ADC-domain values into more useful application representations.

Examples include:

```text
raw ADC
    ↓ offset and gain correction
normalized reflectance
    ↓ threshold or lookup table
object present / absent
```

or:

```text
filtered ADC
    ↓ calibration curve
estimated distance
```

Possible host-side responsibilities:

- per-unit threshold calibration
- inversion of signal polarity
- conversion to percentage
- conversion to voltage
- lookup-table linearization
- sensor-specific physical-unit conversion
- environmental compensation
- combining multiple sensors
- application-specific alarms

This separation lets the firmware provide stable acquisition and transport while the orchestration layer owns application context.

However, firmware-level threshold and state flags remain tuned to the selected photo-reflector circuit unless the embedded interpretation is changed.

---

## Example Host Sequence

Example EMA configuration:

```text
1. Write OP_MODE = OP_MODE_SPEED
2. Write SETPOINT0 = calibrated detection threshold
3. Write LIMIT0 = desired EMA coefficient
4. Write LIMIT1 = hysteresis width
5. Send LATCH_APPLY
6. Confirm LATCHED
7. Set RUN
8. Read FB_VALUE0, FB_VALUE1, and FB_VALUE2
```

Example SMA configuration:

```text
1. Write OP_MODE = OP_MODE_TIME
2. Write SETPOINT0 = calibrated detection threshold
3. Write TARGET_TIME = averaging window
4. Write LIMIT1 = hysteresis width
5. Send LATCH_APPLY
6. Set RUN
7. Monitor DATA_READY or UPDATE_CNT
```

---

## Example Uses

- workpiece presence detection
- conveyor entry and exit detection
- process timing triggers
- orchestration branch conditions
- event logging
- analog-sensor acquisition tests
- filter and threshold evaluation

These are examples rather than restrictions on the register interface.

---

## Current Limitations

- tuned to the selected analog photo-reflector circuit
- fixed ADC input on GPIO26 / ADC0
- fixed emitter LED output on GPIO2
- emitter LED is always on after initialization
- detection polarity is fixed to low-value detection
- saturation thresholds are fixed
- ADC values are not converted to voltage or physical units
- no per-unit calibration storage
- no automatic threshold calibration
- no sensor-disconnect diagnosis
- rising and falling events are not latched history
- filter updates depend on the approximate 10 ms callback
- SMA window is limited to 64
- unsupported common operation modes are rejected
- E-STOP does not disable the emitter LED

---

## Extension Points

Possible extensions include:

- configurable ADC channel
- configurable emitter control
- high-value or low-value detection polarity
- host-selectable threshold direction
- per-unit calibration coefficients
- voltage conversion
- physical-unit conversion
- automatic baseline calibration
- adaptive thresholds
- configurable saturation limits
- latched event counters
- sensor-disconnect detection
- emitter pulsing and synchronous sampling
- additional filter types
- profile-specific unit metadata

Any change to host-visible behavior should also update the profile metadata, Register Map documentation, and this README.

---

## Implementation Note

The comments in `node_profile_sensor.c` should be checked against `node_app_sensor.c` when behavior changes.

The current implementation uses:

```text
filtered_value <= SETPOINT0 → detected
LIMIT1 → hysteresis width
```

The application implementation is authoritative for the current detection polarity and hysteresis behavior.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| ADC, filtering, detection, and feedback behavior | [`src/node_app_sensor.c`](./src/node_app_sensor.c) |
| Sensor profile metadata and defaults | [`src/node_profile_sensor.c`](./src/node_profile_sensor.c) |
| Common public API | [`../common/include/node_core.h`](../common/include/node_core.h) |
| Common runtime behavior | [`../common/core/node_core.c`](../common/core/node_core.c) |
| Register contract | [`../../Docs/Register_Map/`](../../Docs/Register_Map/) |

---

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- Common firmware overview → [`../common/README.md`](../common/README.md)
- Node Extension API → [`../common/docs/Node_Extension_API.md`](../common/docs/Node_Extension_API.md)
- State model → [`../common/docs/State_Model.md`](../common/docs/State_Model.md)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- Motor node → [`../motor_node/`](../motor_node/)
- Servo node → [`../servo_node/`](../servo_node/)
