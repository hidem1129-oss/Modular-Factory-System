import time
import uuid

from PyQt5 import QtCore, QtWidgets
from typing import List
from UI.i_2_c_debugger_800480 import Ui_MainWindow
from db_logger import EventLogger, DB_PATH
from readers import SMBusRegisterReader, MockRegisterReader
from power_config import MAIN_POWER_PORT
from power_monitor import PowerMonitorService
from models import NodeViewModel, StateTransitionLog
from debugger_model import DebuggerModel

from node_logic import (
    DisplayState,
    STATE_COLORS,
)

from constants import (
    DEFAULT_POLLING_MS,
    POLLING_OPTIONS_MS,
    MAX_UI_EVENT_LINES,
    MAIN_CURRENT_LIMIT_MA,
    MAIN_POWER_LIMIT_W,
    DEFAULT_I2C_BUS,
    SESSION_PREFIX,
    COLOR_GRAY,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_RED,
)

CMD_NAME_MAP = {
    0x00: "STOP / NOP",
    0x01: "RUN",
    0x08: "ESTOP_RESET",
    0x10: "LATCH_APPLY",
}

class MainWindow(QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("I2C Debugger - Node & Power Monitor")

        self.current_polling_ms = DEFAULT_POLLING_MS
        self.reader_mode = "real"
        self.session_id = f"{SESSION_PREFIX}_{uuid.uuid4().hex[:12]}"
        self.selected_addr = 0x10
        self.is_closing = False

        self.event_lines: List[str] = []
        self.event_logger = EventLogger(DB_PATH)

        self.power_monitor = PowerMonitorService(
            event_logger=self.event_logger,
            session_id=self.session_id,
        )

        self.model = self._create_model(self.reader_mode)
        self.event_logger.start_monitor_session(
            session_id=self.session_id,
            reader_mode=self.reader_mode,
            polling_ms=self.current_polling_ms,
        )
        self.event_logger.commit()
        self.node_buttons = {
            0x10: self.ui.btn_node_10,
            0x11: self.ui.btn_node_11,
            0x12: self.ui.btn_node_12,
            0x13: self.ui.btn_node_13,
            0x14: self.ui.btn_node_14,
            0x15: self.ui.btn_node_15,
            0x16: self.ui.btn_node_16,
            0x17: self.ui.btn_node_17,
            0x18: self.ui.btn_node_18,
            0x19: self.ui.btn_node_19,
        }

        self.power_labels = {
            0: {
                "current": self.ui.label_port0_current_value,
                "voltage": self.ui.label_port0_voltage_value,
                "power": self.ui.label_port0_power_value,
            },
            1: {
                "current": self.ui.label_port1_current_value,
                "voltage": self.ui.label_port1_voltage_value,
                "power": self.ui.label_port1_power_value,
            },
            2: {
                "current": self.ui.label_port2_current_value,
                "voltage": self.ui.label_port2_voltage_value,
                "power": self.ui.label_port2_power_value,
            },
            3: {
                "current": self.ui.label_port3_current_value,
                "voltage": self.ui.label_port3_voltage_value,
                "power": self.ui.label_port3_power_value,
            },
            4: {
                "current": self.ui.label_port4_current_value,
                "voltage": self.ui.label_port4_voltage_value,
                "power": self.ui.label_port4_power_value,
            },
            5: {
                "current": self.ui.label_port5_current_value,
                "voltage": self.ui.label_port5_voltage_value,
                "power": self.ui.label_port5_power_value,
            },
            6: {
                "current": self.ui.label_port6_current_value,
                "voltage": self.ui.label_port6_voltage_value,
                "power": self.ui.label_port6_power_value,
            },
            7: {
                "current": self.ui.label_port7_current_value,
                "voltage": self.ui.label_port7_voltage_value,
                "power": self.ui.label_port7_power_value,
            },
        }

        self.main_labels = {
            "current": self.ui.label_main_current_value,
            "voltage": self.ui.label_main_voltage_value,
            "power": self.ui.label_main_power_value,
            "current_usage": self.ui.label_main_current_usage_value,
            "power_usage": self.ui.label_main_power_usage_value,
        }

        self.capacity_bars = {
            "current": self.ui.progressBar_current,
            "power": self.ui.progressBar_power,
        }

        self.latest_power_data = {}

        for addr, btn in self.node_buttons.items():
            btn.clicked.connect(lambda _, a=addr: self.on_node_selected(a))

        self.ui.comboBox.clear()
        self.ui.comboBox.addItems([f"{ms} ms" for ms in POLLING_OPTIONS_MS])
        self.ui.comboBox.setCurrentText(f"{DEFAULT_POLLING_MS} ms")
        self.ui.comboBox.currentTextChanged.connect(self.change_polling_interval)

        self.ui.comboBox_mode.clear()
        self.ui.comboBox_mode.addItems(["mock", "real"])
        self.ui.comboBox_mode.setCurrentText("real")
        self.ui.comboBox_mode.currentTextChanged.connect(self.change_reader_mode)

        self.reload_event_log_from_db()

        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_ui)
        self.timer.start(self.current_polling_ms)

        self.update_ui()

    def _create_model(self, mode: str) -> DebuggerModel:
        if mode == "real":
            try:
                reader = SMBusRegisterReader(bus_no=DEFAULT_I2C_BUS)
            except Exception as e:
                QtWidgets.QMessageBox.warning(
                    self,
                    "SMBus Error",
                    f"real mode に切り替え失敗:\n{e}\nmock に戻します"
                )
                self.ui.comboBox_mode.blockSignals(True)
                self.ui.comboBox_mode.setCurrentText("mock")
                self.ui.comboBox_mode.blockSignals(False)
                self.reader_mode = "mock"
                return DebuggerModel(MockRegisterReader())

            self.reader_mode = "real"
            return DebuggerModel(reader)

        self.reader_mode = "mock"
        return DebuggerModel(MockRegisterReader())

    def _replace_model(self, mode: str) -> None:
        old_model = self.model
        self.model = self._create_model(mode)
        old_model.close()

    def state_style(self, state: DisplayState, selected: bool = False) -> str:
        colors = STATE_COLORS[state]
        bg = colors["bg"]
        fg = colors["fg"]
        border = "3px solid #222222" if selected else "1px solid #444444"
        return (
            f"background-color: {bg};"
            f"color: {fg};"
            f"border: {border};"
            f"text-align: left;"
            f"padding-left: 8px;"
        )

    def update_single_power_port(
        self,
        idx: int,
        data: dict[str, float | None],
    ) -> None:
        labels = self.power_labels.get(idx)
        if labels is None:
            return

        current_mA = data.get("current_mA")
        voltage_V = data.get("bus_voltage_V")
        power_mW = data.get("power_mW")

        labels["current"].setText(
            f"{current_mA:.1f} mA" if current_mA is not None else "-- mA"
        )
        labels["voltage"].setText(
            f"{voltage_V:.3f} V" if voltage_V is not None else "-- V"
        )
        labels["power"].setText(
            f"{power_mW / 1000.0:.3f} W" if power_mW is not None else "-- W"
        )

        title_label = getattr(self.ui, f"port{idx}_label", None)
        if title_label is not None:
            self.set_port_title_color_by_current(title_label, current_mA)


    def update_main_line_summary(
        self,
        *,
        total_current_mA: float,
        total_power_mW: float,
        main_voltage_V: float | None,
    ) -> None:
        total_power_W = total_power_mW / 1000.0

        current_pct = (total_current_mA / MAIN_CURRENT_LIMIT_MA) * 100.0
        power_pct = (total_power_W / MAIN_POWER_LIMIT_W) * 100.0

        self.main_labels["current"].setText(f"{total_current_mA:.1f} mA")
        self.main_labels["voltage"].setText(
            f"{main_voltage_V:.3f} V" if main_voltage_V is not None else "-- V"
        )
        self.main_labels["power"].setText(f"{total_power_W:.3f} W")

        self.main_labels["current_usage"].setText(
            f"{total_current_mA:.1f} mA / {MAIN_CURRENT_LIMIT_MA:.0f}mA / {current_pct:.1f} %"
        )
        self.main_labels["power_usage"].setText(
            f"{total_power_W:.3f} W / {MAIN_POWER_LIMIT_W:.0f}W / {power_pct:.1f} %"
        )

        main_usage_pct = max(current_pct, power_pct)
        self.set_main_line_color(main_usage_pct)

        self.update_capacity_bar("current", current_pct)
        self.update_capacity_bar("power", power_pct)

    def update_capacity_bar(self, key: str, usage_percent: float) -> None:
        clamped_value = self.clamp_progress_value(usage_percent)
        color = self.classify_main_usage_color(usage_percent)

        self.capacity_bars[key].setValue(clamped_value)
        self.set_progressbar_color(self.capacity_bars[key], color)

    def clamp_progress_value(self, value: float) -> int:
        return max(0, min(100, int(round(value))))

    def update_node_button(self, addr: int, node: NodeViewModel) -> None:
        btn = self.node_buttons[addr]
        btn.setText(f"0x{addr:02X}  {node.module_name}  {node.state.value}")
        btn.setStyleSheet(self.state_style(node.state, selected=(addr == self.selected_addr)))

    def on_node_selected(self, addr: int) -> None:
        self.selected_addr = addr
        self.refresh_selected_node()
        for a, node in self.model.nodes.items():
            self.update_node_button(a, node)

    def refresh_selected_node(self) -> None:
        node = self.model.nodes[self.selected_addr]

        self.ui.node_address.setText(f"0x{node.address:02X}")
        self.ui.node_device.setText(node.module_name)
        self.ui.node_state.setText(node.state.value)

        # 選択ノードのヘッダにも状態色を反映
        colors = STATE_COLORS[node.state]
        header_style = (
            f"background-color: {colors['bg']};"
            f"color: {colors['fg']};"
            f"border: 1px solid #444444;"
            f"padding: 2px;"
        )
        self.ui.node_address.setStyleSheet(header_style)
        self.ui.node_device.setStyleSheet(header_style)
        self.ui.node_state.setStyleSheet(header_style)

        if node.status_raw is None:
            self.ui.node_status_bit.setText("--")
            self.ui.node_status_detail.setText("--")
        else:
            self.ui.node_status_bit.setText(f"0x{node.status_raw:02X}")
            self.ui.node_status_detail.setText("\n".join(f"- {f}" for f in node.status_flags))

        if node.cmd_raw is None:
            self.ui.node_cmd_bit.setText("--")
            self.ui.node_cmd_detail.setText("--")
        else:
            self.ui.node_cmd_bit.setText(f"0x{node.cmd_raw:02X}")
            self.ui.node_cmd_detail.setText(f"- {node.cmd_name}")

        if node.last_seen_ts is None:
            self.ui.node_time_value.setText("--")
        else:
            self.ui.node_time_value.setText(
                time.strftime("%H:%M:%S", time.localtime(node.last_seen_ts))
            )

    def append_event_log(self, line: str) -> None:
        self.event_lines.insert(0, line)
        self.event_lines = self.event_lines[:MAX_UI_EVENT_LINES]
        self.ui.plainTextEdit_log.setPlainText("\n\n".join(self.event_lines))

    def reload_event_log_from_db(self) -> None:
        rows = self.event_logger.fetch_recent_events(limit=MAX_UI_EVENT_LINES)
        self.event_lines = [
            f"[{row['ts_text']}] {row['address_hex']} {row['module_name']}\n{row['old_state']} -> {row['new_state']}"
            for row in rows
        ]
        self.ui.plainTextEdit_log.setPlainText("\n\n".join(self.event_lines))


    def handle_single_transition(self, log: StateTransitionLog) -> None:
        node = self.model.nodes[log.address]

        self.event_logger.log_event_transition(
            timestamp=log.timestamp,
            address=log.address,
            module_name=log.module_name,
            old_state=log.old_state,
            new_state=log.new_state,
            status_raw=node.status_raw,
            cmd_raw=node.cmd_raw,
            cmd_name=node.cmd_name,
            run_id=node.current_run_id,
            session_id=self.session_id,
        )

        self.event_logger.log_state_segment(
            timestamp=log.timestamp,
            address=log.address,
            module_name=log.module_name,
            new_state=log.new_state,
            session_id=self.session_id,
        )

        if log.old_state == "READY" and log.new_state == "BUSY":
            run_id = self.make_run_id(node, log.timestamp)
            node.current_run_id = run_id
            node.current_run_start_ts = log.timestamp
            self.event_logger.start_run_session(
                run_id=run_id,
                session_id=self.session_id,
                node=node,
                start_ts=log.timestamp,
            )

        elif log.old_state == "BUSY" and log.new_state == "READY":
            if node.current_run_id is not None:
                self.event_logger.finish_run_session(
                    run_id=node.current_run_id,
                    end_ts=log.timestamp,
                    end_state=log.new_state,
                    feedback0=node.feedback0,
                    result="SUCCESS",
                )
                node.current_run_id = None
                node.current_run_start_ts = None

        ts = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(log.timestamp))
        line = (
            f"[{ts}] 0x{log.address:02X} \n{log.module_name}\n"
            f"{log.old_state} -> {log.new_state}"
        )
        self.append_event_log(line)

    def handle_transitions(self, transitions: list[StateTransitionLog]) -> None:
        for log in transitions:
            self.handle_single_transition(log)

    def save_all_node_snapshots(self) -> None:
        ts = time.time()
        for node in self.model.nodes.values():
            self.event_logger.log_snapshot(
                timestamp=ts,
                node=node,
                setpoint0=node.setpoint0,
                target_time_ms=node.target_time_ms,
                feedback0=node.feedback0,
                run_id=node.current_run_id,
                session_id=self.session_id,
            )

    def handle_session_close(self, end_ts: float) -> None:
        for node in self.model.nodes.values():
            if node.state == DisplayState.SESSION_ENDED:
                continue

            transition = StateTransitionLog(
                timestamp=end_ts,
                address=node.address,
                module_name=node.module_name,
                old_state=node.state.value,
                new_state=DisplayState.SESSION_ENDED.value,
            )

            self.event_logger.log_event_transition(
                timestamp=transition.timestamp,
                address=transition.address,
                module_name=transition.module_name,
                old_state=transition.old_state,
                new_state=transition.new_state,
                status_raw=node.status_raw,
                cmd_raw=node.cmd_raw,
                cmd_name=node.cmd_name,
                run_id=node.current_run_id,
                session_id=self.session_id,
            )

            self.event_logger.log_state_segment(
                timestamp=transition.timestamp,
                address=transition.address,
                module_name=transition.module_name,
                new_state=transition.new_state,
                session_id=self.session_id,
            )

            node.state = DisplayState.SESSION_ENDED

            self.event_logger.log_snapshot(
                timestamp=end_ts,
                node=node,
                setpoint0=node.setpoint0,
                target_time_ms=node.target_time_ms,
                feedback0=node.feedback0,
                run_id=node.current_run_id,
                session_id=self.session_id,
            )

    def collect_power_snapshot(self) -> dict[str, dict[str, float | None]]:
        return self.power_monitor.collect_snapshot(self.model.reader)

    def update_ui(self) -> None:
        if getattr(self, "is_closing", False):
            return

        transitions = self.model.poll_once()

        for addr, node in self.model.nodes.items():
            self.update_node_button(addr, node)

        self.refresh_selected_node()
        self.handle_transitions(transitions)
        self.save_all_node_snapshots()


        self.statusBar().showMessage(
            f"mode={self.reader_mode} / bus=i2c-{DEFAULT_I2C_BUS} / polling={self.current_polling_ms} ms"
        )

        power_snapshot = self.collect_power_snapshot()
        if power_snapshot:
            self.latest_power_data = power_snapshot
            self.update_power_monitor(power_snapshot)

    def change_polling_interval(self, text: str) -> None:
        value = int(text.split()[0])
        self.current_polling_ms = value
        self.timer.stop()
        self.timer.start(value)

    def change_reader_mode(self, text: str) -> None:
        self.timer.stop()
        self._replace_model(text)
        self.reload_event_log_from_db()
        self.timer.start(self.current_polling_ms)
        self.update_ui()

    def closeEvent(self, event):
        self.is_closing = True
        self.timer.stop()
        end_ts = time.time() + 1.0

        self.handle_session_close(end_ts)

        self.event_logger.finish_monitor_session(
            session_id=self.session_id,
            result="CLOSED",
            note="Main window closed",
        )

        self.event_logger.commit()
        self.model.close()
        self.event_logger.close()
        super().closeEvent(event)

    def update_power_monitor(self, power_data: dict[str, dict[str, float | None]]) -> None:
        # 枝線 PORT_1〜PORT_8 をUIへ表示
        for ui_idx, port_no in enumerate(range(1, 9)):
            port_id = f"PORT_{port_no}"
            data = power_data.get(port_id, {})
            self.update_single_power_port(ui_idx, data)

        # MAIN_LINE はU13の実測値を直接使う
        main_data = power_data.get(MAIN_POWER_PORT.port_id, {})

        main_current_mA = main_data.get("current_mA")
        main_power_mW = main_data.get("power_mW")
        main_voltage_V = main_data.get("bus_voltage_V")

        self.update_main_line_summary(
            total_current_mA=main_current_mA if main_current_mA is not None else 0.0,
            total_power_mW=main_power_mW if main_power_mW is not None else 0.0,
            main_voltage_V=main_voltage_V,
        )

    def classify_port_current_color(self, current_mA: float | None) -> str:
        if current_mA is None:
            return COLOR_GRAY  # 濃灰
        if current_mA <= 10.0:
            return COLOR_GRAY  # 0～15mA: 灰
        if current_mA <= 300.0:
            return COLOR_GREEN  # 15～300mA: 緑
        if current_mA <= 500.0:
            return COLOR_YELLOW  # 301～500mA: 黄
        return COLOR_RED  # 501mA以上: 赤

    def classify_main_usage_color(self, usage_percent: float) -> str:
        if usage_percent <= 20.0:
            return COLOR_GRAY  # 濃灰
        if usage_percent <= 70.0:
            return COLOR_GREEN  # 緑
        if usage_percent <= 90.0:
            return COLOR_YELLOW  # 黄
        return COLOR_RED  # 赤

    def set_main_line_color(self, usage_percent: float) -> None:
        color = self.classify_main_usage_color(usage_percent)

        # タイトル
        self.ui.port_main_label.setStyleSheet(f"""
            color: {color};
            font-weight: bold;
        """)

    def set_progressbar_color(self, bar, color: str) -> None:
        bar.setStyleSheet(f"""
            QProgressBar {{
                border: 1px solid #888;
                background-color: #EAEAEA;
                text-align: center;
            }}

            QProgressBar::chunk {{
                background-color: {color};
            }}
        """)

    def set_port_title_color_by_current(self, label_widget, current_mA: float | None) -> None:
        color = self.classify_port_current_color(current_mA)
        label_widget.setStyleSheet(f"""
            color: {color};
            font-weight: bold;
        """)

    def make_run_id(self, node: NodeViewModel, ts: float) -> str:
        return f"0x{node.address:02X}_{int(ts * 1000)}"

