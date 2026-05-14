from __future__ import annotations

from dataclasses import dataclass, field
from typing import List, Optional

from node_logic import DisplayState


@dataclass
class StateTransitionLog:
    timestamp: float
    address: int
    module_name: str
    old_state: str
    new_state: str


@dataclass
class NodeViewModel:
    address: int
    module_type: int = 0x00
    module_name: str = "None"
    status_raw: Optional[int] = None
    status_flags: List[str] = field(default_factory=list)
    cmd_raw: Optional[int] = None
    cmd_name: str = "--"
    state: DisplayState = DisplayState.NO_DEVICE
    protocol_state: DisplayState = DisplayState.NO_DEVICE
    current_warn: bool = False
    warn_source: str = "--"
    ever_seen: bool = False
    error_count: int = 0
    last_seen_ts: Optional[float] = None
    setpoint0: Optional[int] = None
    target_time_ms: Optional[int] = None
    feedback0: Optional[int] = None
    current_run_id: Optional[str] = None
    current_run_start_ts: Optional[float] = None