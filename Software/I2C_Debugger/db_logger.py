import time
import os
import sqlite3

from typing import Any, List, Optional
from pathlib import Path

DB_DIR = Path("/var/lib/i2c_debugger")
DB_PATH = DB_DIR / "i2c_debugger_events.sqlite3"
DB_DIR.mkdir(parents=True, exist_ok=True)
os.chmod(DB_DIR, 0o2775)

if DB_PATH.exists():
    os.chmod(DB_PATH, 0o664)

class EventLogger:
    def __init__(self, db_path: Path) -> None:
        self.db_path = db_path
        self.db_path.parent.mkdir(parents=True, exist_ok=True)
        self.conn = sqlite3.connect(str(db_path), timeout=3.0)
        self.conn.row_factory = sqlite3.Row
        self.conn.execute("PRAGMA journal_mode=WAL;")
        self.conn.execute("PRAGMA synchronous=NORMAL;")
        self.conn.execute("PRAGMA busy_timeout=3000;")
        self.conn.row_factory = sqlite3.Row
        self._setup()

    def _setup(self) -> None:
        SQL = """
            CREATE TABLE IF NOT EXISTS event_logs (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                ts_epoch        REAL NOT NULL,
                ts_text         TEXT NOT NULL,
                address         INTEGER NOT NULL,
                address_hex     TEXT NOT NULL,
                module_name     TEXT,
                old_state       TEXT,
                new_state       TEXT,
                event_type      TEXT NOT NULL,
                status_raw      INTEGER,
                cmd_raw         INTEGER,
                cmd_name        TEXT,
                message         TEXT,
                run_id          TEXT,
                session_id      TEXT
            );

            CREATE TABLE IF NOT EXISTS node_snapshots (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                ts_epoch        REAL NOT NULL,
                ts_text         TEXT NOT NULL,
                address         INTEGER NOT NULL,
                address_hex     TEXT NOT NULL,
                module_name     TEXT,
                state           TEXT,
                status_raw      INTEGER,
                cmd_raw         INTEGER,
                cmd_name        TEXT,
                setpoint0       INTEGER,
                target_time_ms  INTEGER,
                feedback0       INTEGER,
                run_id          TEXT,
                session_id      TEXT
            );

            CREATE TABLE IF NOT EXISTS run_sessions (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                run_id          TEXT NOT NULL UNIQUE,
                session_id      TEXT,
                address         INTEGER NOT NULL,
                address_hex     TEXT NOT NULL,
                module_name     TEXT,
                start_ts_epoch  REAL NOT NULL,
                end_ts_epoch    REAL,
                duration_ms     INTEGER,
                setpoint0       INTEGER,
                target_time_ms  INTEGER,
                feedback0       INTEGER,
                result          TEXT,
                end_state       TEXT
            );

            CREATE TABLE IF NOT EXISTS state_segments (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                session_id TEXT,
                address INTEGER NOT NULL,
                address_hex TEXT NOT NULL,
                module_name TEXT,
                state TEXT NOT NULL,
                start_ts REAL NOT NULL,
                end_ts REAL
            );

            CREATE TABLE IF NOT EXISTS monitor_sessions (
                id                INTEGER PRIMARY KEY AUTOINCREMENT,
                session_id        TEXT NOT NULL UNIQUE,
                started_ts_epoch  REAL NOT NULL,
                started_ts_text   TEXT NOT NULL,
                ended_ts_epoch    REAL,
                ended_ts_text     TEXT,
                reader_mode       TEXT,
                polling_ms        INTEGER,
                result            TEXT,
                note              TEXT
            );

            CREATE TABLE IF NOT EXISTS power_port_snapshots (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                ts_epoch        REAL NOT NULL,
                ts_text         TEXT NOT NULL,

                port_id         TEXT NOT NULL,
                tca_addr        INTEGER NOT NULL,
                tca_addr_hex    TEXT NOT NULL,
                tca_channel     INTEGER NOT NULL,
                ina_addr        INTEGER NOT NULL,
                ina_addr_hex    TEXT NOT NULL,

                bus_voltage_V   REAL,
                shunt_raw       INTEGER,
                current_mA      REAL,
                power_mW        REAL,

                session_id      TEXT
            );

            CREATE INDEX IF NOT EXISTS idx_monitor_sessions_started
            ON monitor_sessions(started_ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_event_logs_ts
            ON event_logs(ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_event_logs_addr_ts
            ON event_logs(address, ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_node_snapshots_ts
            ON node_snapshots(ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_node_snapshots_addr_ts
            ON node_snapshots(address, ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_run_sessions_addr_start
            ON run_sessions(address, start_ts_epoch);

            CREATE INDEX IF NOT EXISTS idx_segments_address
            ON state_segments(address_hex);

            CREATE INDEX IF NOT EXISTS idx_segments_time
            ON state_segments(start_ts, end_ts);

            """

        cur = self.conn.cursor()
        cur.executescript(SQL)

        self._ensure_column("event_logs", "session_id", "TEXT")
        self._ensure_column("node_snapshots", "session_id", "TEXT")
        self._ensure_column("run_sessions", "session_id", "TEXT")
        self._ensure_column("state_segments", "session_id", "TEXT")
        self._ensure_column("node_snapshots", "protocol_state", "TEXT")
        self._ensure_column("node_snapshots", "current_warn", "INTEGER")
        self._ensure_column("node_snapshots", "warn_source", "TEXT")
        self._ensure_column("power_port_snapshots", "shunt_raw", "INTEGER")
        self._ensure_column("power_port_snapshots", "power_mW", "REAL")

        self.conn.commit()
        print(f"DB initialized: {self.db_path}")

    def _ensure_column(self, table_name: str, column_name: str, column_def: str) -> None:
        cur = self.conn.cursor()
        cur.execute(f"PRAGMA table_info({table_name})")
        existing = {row["name"] for row in cur.fetchall()}
        if column_name not in existing:
            cur.execute(f"ALTER TABLE {table_name} ADD COLUMN {column_name} {column_def}")

    def start_monitor_session(
            self,
            *,
            session_id: str,
            reader_mode: str,
            polling_ms: int,
    ) -> None:
        ts = time.time()
        ts_text = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ts))
        self.conn.execute(
            """
            INSERT INTO monitor_sessions (
                session_id,
                started_ts_epoch,
                started_ts_text,
                reader_mode,
                polling_ms,
                result
            ) VALUES (?, ?, ?, ?, ?, ?)
            """,
            (session_id, ts, ts_text, reader_mode, polling_ms, "RUNNING"),
        )
        self.conn.commit()

    def finish_monitor_session(
            self,
            *,
            session_id: str,
            result: str = "CLOSED",
            note: Optional[str] = None,
    ) -> None:
        ts = time.time()
        ts_text = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ts))
        self.conn.execute(
            """
            UPDATE monitor_sessions
            SET ended_ts_epoch = ?,
                ended_ts_text = ?,
                result = ?,
                note = ?
            WHERE session_id = ?
            """,
            (ts, ts_text, result, note, session_id),
        )
        self.conn.commit()

    def log_state_segment(
            self,
            timestamp: float,
            address: int,
            module_name: str,
            new_state: str,
            session_id: Optional[str] = None,
    ) -> None:
        address_hex = f"0x{address:02X}"

        self.conn.execute(
            """
            UPDATE state_segments
            SET end_ts = ?
            WHERE address = ?
              AND end_ts IS NULL
            """,
            (timestamp, address),
        )

        self.conn.execute(
            """
            INSERT INTO state_segments (
                session_id,
                address,
                address_hex,
                module_name,
                state,
                start_ts,
                end_ts
            ) VALUES (?, ?, ?, ?, ?, ?, NULL)
            """,
            (
                session_id,
                address,
                address_hex,
                module_name,
                new_state,
                timestamp,
            ),
        )

    def log_event_transition(
            self,
            *,
            timestamp: float,
            address: int,
            module_name: str,
            old_state: str,
            new_state: str,
            status_raw: Optional[int],
            cmd_raw: Optional[int],
            cmd_name: str,
            run_id: Optional[str],
            session_id: Optional[str] = None,
    ) -> None:
        ts_text = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(timestamp))
        self.conn.execute(
            """
            INSERT INTO event_logs (
                ts_epoch, ts_text,
                address, address_hex, module_name,
                old_state, new_state,
                event_type,
                status_raw,
                cmd_raw, cmd_name,
                message,
                run_id,
                session_id
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                timestamp,
                ts_text,
                address,
                f"0x{address:02X}",
                module_name,
                old_state,
                new_state,
                "STATE_CHANGE",
                status_raw,
                cmd_raw,
                cmd_name,
                f"{old_state} -> {new_state}",
                run_id,
                session_id,
            ),
        )

    def fetch_recent_events(self, limit: int = 100) -> List[sqlite3.Row]:
        cur = self.conn.cursor()
        cur.execute(
            """
            SELECT ts_text, address_hex, module_name, old_state, new_state
            FROM event_logs
            ORDER BY id DESC
            LIMIT ?
            """,
            (limit,),
        )
        return cur.fetchall()

    def log_power_port_snapshot(
            self,
            *,
            ts_epoch: float,
            port_id: str,
            tca_addr: int,
            tca_channel: int,
            ina_addr: int,
            bus_voltage_V: Optional[float],
            shunt_raw: Optional[int],
            current_mA: Optional[float],
            power_mW: Optional[float],
            session_id: Optional[str] = None,
    ) -> None:
        ts_text = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ts_epoch))
        self.conn.execute(
            """
            INSERT INTO power_port_snapshots (
                ts_epoch, ts_text,
                port_id,
                tca_addr, tca_addr_hex,
                tca_channel,
                ina_addr, ina_addr_hex,
                bus_voltage_V, shunt_raw, current_mA,power_mW,
                session_id
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                ts_epoch,
                ts_text,
                port_id,
                tca_addr,
                f"0x{tca_addr:02X}",
                tca_channel,
                ina_addr,
                f"0x{ina_addr:02X}",
                bus_voltage_V,
                shunt_raw,
                current_mA,
                power_mW,
                session_id,
            ),
        )

    def log_snapshot(
            self,
            timestamp: float,
            node: Any,
            *,
            setpoint0: Optional[int] = None,
            target_time_ms: Optional[int] = None,
            feedback0: Optional[int] = None,
            run_id: Optional[str] = None,
            session_id: Optional[str] = None,
    ) -> None:
        ts_text = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(timestamp))
        self.conn.execute(
            """
            INSERT INTO node_snapshots (
                ts_epoch, ts_text,
                address, address_hex, module_name,
                state, protocol_state, current_warn, warn_source, status_raw,
                cmd_raw, cmd_name,
                setpoint0, target_time_ms, feedback0,
                run_id, session_id
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                timestamp,
                ts_text,
                node.address,
                f"0x{node.address:02X}",
                node.module_name,
                node.state.value,
                node.protocol_state.value,
                1 if node.current_warn else 0,
                node.warn_source,
                node.status_raw,
                node.cmd_raw,
                node.cmd_name,
                setpoint0,
                target_time_ms,
                feedback0,
                run_id,
                session_id,
            ),
        )

    def start_run_session(
            self,
            run_id: str,
            session_id: Optional[str],
            node: Any,
            start_ts: float,
    ) -> None:
        self.conn.execute(
            """
            INSERT INTO run_sessions (
                run_id,
                session_id,
                address, address_hex, module_name,
                start_ts_epoch,
                setpoint0, target_time_ms,
                result, end_state
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                run_id,
                session_id,
                node.address,
                f"0x{node.address:02X}",
                node.module_name,
                start_ts,
                node.setpoint0,
                node.target_time_ms,
                "RUNNING",
                None,
            ),
        )
        self.conn.commit()

    def finish_run_session(
            self,
            run_id: str,
            end_ts: float,
            end_state: str,
            feedback0: Optional[int],
            result: str = "SUCCESS",
    ) -> None:
        cur = self.conn.cursor()
        cur.execute(
            "SELECT start_ts_epoch FROM run_sessions WHERE run_id = ?",
            (run_id,),
        )
        row = cur.fetchone()
        if row is None:
            return

        start_ts = row["start_ts_epoch"]
        duration_ms = int((end_ts - start_ts) * 1000)

        self.conn.execute(
            """
            UPDATE run_sessions
            SET end_ts_epoch = ?,
                duration_ms = ?,
                result = ?,
                end_state = ?,
                feedback0 = ?
            WHERE run_id = ?
            """,
            (
                end_ts,
                duration_ms,
                result,
                end_state,
                feedback0,
                run_id,
            ),
        )
        self.conn.commit()

    def commit(self):
        self.conn.commit()

    def close(self) -> None:
        self.conn.close()