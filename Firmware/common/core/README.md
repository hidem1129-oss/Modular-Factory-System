# core

This directory contains the shared control logic that defines how each node behaves.

---

## Purpose

The `core` layer implements the **common execution model** for all nodes.

It is responsible for:

- node state management
- command handling
- register processing
- execution flow control

---

## Responsibilities

The core layer handles:

- State transitions (IDLE / RUNNING / ESTOP)
- Command acceptance / rejection
- Register updates and synchronization
- DATA_READY and UPDATE_CNT handling
- Latch and staging mechanisms
- Invocation of node-specific callbacks

---

## Execution Model

Each node follows a unified execution flow defined by the core:

1. Host writes to registers (via I²C)
2. Core processes register updates
3. Commands are validated and accepted/rejected
4. State transitions occur (e.g., IDLE → RUNNING)
5. Node-specific logic is executed via callbacks
6. Results are written back to registers

This ensures consistent behavior across all node types.

---

## Relationship to node-specific code

The core layer does not implement device-specific behavior.

Instead, it delegates execution to node-specific implementations via callbacks.

### Example:

- `servo_node` → controls servo movement
- `sensor_node` → reads sensor values
- `motor_node` → drives motors

---

## Callback Mechanism

Node-specific logic is injected through callback structures.

The core calls these callbacks at appropriate timing points:

- initialization
- periodic updates
- command execution
- state transitions

This allows:

- separation of common and device-specific logic
- reuse of the same execution model across nodes

---

## Design Concept

The core layer acts as a **runtime engine for nodes**.

It provides:

- a consistent lifecycle
- a unified command interface
- predictable state behavior

Node-specific code only defines *what to do*,  
while the core defines *how and when it runs*.

---

## Key Files

- `node_core.c` : main control logic and execution flow
- `node_app.c` : callback dispatch and node-specific binding
- `node_profile.c` : node configuration selection
