# include

This directory contains public headers shared across all node implementations.

---

## Purpose

The `include` layer defines the **public interface of the firmware core**.

It provides the APIs that node-specific implementations use to interact with the core system.

---

## Responsibilities

- Expose core functionality to node-specific code
- Define callback interfaces
- Provide access to shared data (registers, state, etc.)
- Abstract internal implementation details

---

## Design Concept

The `include` layer acts as the **boundary between common logic and node-specific logic**.

Node-specific implementations should:

- include headers from this directory
- use only the exposed APIs
- avoid direct access to internal core logic

---

## Usage

Node-specific code (e.g., `servo_node`) interacts with the system through these headers.

### Example responsibilities:

- reading register values
- publishing feedback values
- accessing system state
- implementing callbacks

---

## Key Files

- `node_core.h` : core API (state, register access, control functions)
- `node_app.h` : callback interface definition

---

## Important Rule

> Node-specific code must depend on `include/`, not directly on `core/`.

This ensures:

- clean separation of responsibilities
- maintainability
- portability of node implementations
