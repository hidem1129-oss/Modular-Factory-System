import time
from typing import Dict, List

from readers import RegisterReader
from models import NodeViewModel, StateTransitionLog
from node_logic import (
    DisplayState,
    decode_status_flags,
    decode_cmd_name,
    determine_protocol_state,
    merge_display_state,
)
from constants import (
    ADDRESS_START,
    ADDRESS_END,
    MAX_CONSECUTIVE_ERRORS,
    MODULE_TYPE_NAMES,
)

class DebuggerModel:
    def __init__(self, reader: RegisterReader) -> None:
        self.reader = reader
        self.has_completed_first_poll = False
        self.nodes: Dict[int, NodeViewModel] = {
            address: NodeViewModel(address=address)
            for address in range(ADDRESS_START, ADDRESS_END + 1)
        }

    def close(self) -> None:
        close_fn = getattr(self.reader, "close", None)
        if callable(close_fn):
            close_fn()

    def evaluate_mock_current_warn(self, address: int, protocol_state: DisplayState) -> tuple[bool, str]:
        if address == 0x11 and protocol_state == DisplayState.BUSY:
            return True, "Current"
        return False, "Protocol" if protocol_state == DisplayState.WARN else "--"

    def poll_once(self) -> List[StateTransitionLog]:
        transitions: List[StateTransitionLog] = []

        for address, node in self.nodes.items():
            old_state = node.state.value
            old_module_name = node.module_name

            try:
                raw = self.reader.read_node(address)
                node.ever_seen = True
                node.error_count = 0
                node.last_seen_ts = time.time()

                node.module_type = raw.module_type
                node.module_name = MODULE_TYPE_NAMES.get(
                    raw.module_type, f"Type 0x{raw.module_type:02X}"
                )

                node.status_raw = raw.status_raw
                node.status_flags = decode_status_flags(raw.status_raw)
                node.cmd_raw = raw.cmd_raw
                node.cmd_name = decode_cmd_name(raw.cmd_raw)

                node.protocol_state = determine_protocol_state(raw.status_raw)
                node.current_warn, node.warn_source = self.evaluate_mock_current_warn(
                    address, node.protocol_state
                )
                node.state = merge_display_state(node.protocol_state, node.current_warn)

                node.setpoint0 = raw.setpoint0
                node.target_time_ms = raw.target_time_ms
                node.feedback0 = raw.feedback0

            except Exception:
                node.error_count += 1
                node.status_raw = None
                node.status_flags = []
                node.cmd_raw = None
                node.cmd_name = "--"
                node.current_warn = False
                node.warn_source = "--"

                if node.error_count < MAX_CONSECUTIVE_ERRORS:
                    node.protocol_state = DisplayState.UNKNOWN
                    node.state = DisplayState.UNKNOWN
                else:
                    lost_state = DisplayState.SIGNAL_LOST if node.ever_seen else DisplayState.NO_DEVICE
                    node.protocol_state = lost_state
                    node.state = lost_state
                    if not node.ever_seen:
                        node.module_name = "No Device"

            if self.has_completed_first_poll and node.state.value != old_state:
                transitions.append(
                    StateTransitionLog(
                        timestamp=time.time(),
                        address=address,
                        module_name=node.module_name if node.module_name else old_module_name,
                        old_state=old_state,
                        new_state=node.state.value,
                    )
                )

        self.has_completed_first_poll = True
        return transitions
