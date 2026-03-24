# common

This directory contains shared firmware components used across all Raspberry Pi Pico nodes.

---

## Purpose

The `common` layer provides the reusable foundation of the firmware architecture.

It defines how all nodes behave in a consistent and unified way.

---

## Responsibilities

The `common` layer is responsible for:

- Shared node behavior (state handling, control flow)
- Register-based communication handling
- Public APIs for node-specific implementations
- Platform and build configuration
- Common design documentation

---

## Directory Structure

- `core/` : shared control logic and state handling
- `include/` : public headers used by node-specific implementations
- `platform/` : build/runtime configuration and Pico-specific entry points
- `docs/` : common design notes and architecture documents

---

## Relationship to node-specific firmware

Node-specific firmware such as `servo_node` or `sensor_node` is built on top of this shared layer.

In other words:

- `common/` defines the reusable firmware core
- each node directory defines device-specific behavior
- node-specific code extends the shared system, rather than replacing it

---

## Design Concept

This architecture separates:

- **what is common** → implemented in `common/`
- **what is device-specific** → implemented in each node directory

This allows:

- easy addition of new node types
- consistent behavior across nodes
- reusable firmware components
