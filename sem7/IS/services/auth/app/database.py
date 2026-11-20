import os
import sqlite3
import time
from pathlib import Path


class AuthDatabase:
    def __init__(self) -> None:
        self.path = Path(os.getenv("AUTH_DB_PATH", "/data/auth.sqlite3"))

    def _connect(self) -> sqlite3.Connection:
        connection = sqlite3.connect(self.path, timeout=5)
        connection.row_factory = sqlite3.Row
        connection.execute("PRAGMA foreign_keys = ON")
        return connection

    def initialize(self) -> None:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        with self._connect() as connection:
            connection.execute("PRAGMA journal_mode = WAL")
            connection.executescript(
                """
                CREATE TABLE IF NOT EXISTS users (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    username TEXT NOT NULL UNIQUE,
                    password_hash TEXT NOT NULL,
                    created_at INTEGER NOT NULL
                );

                CREATE TABLE IF NOT EXISTS sessions (
                    token_hash TEXT PRIMARY KEY,
                    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                    csrf_hash TEXT NOT NULL,
                    created_at INTEGER NOT NULL,
                    expires_at INTEGER NOT NULL
                );

                CREATE INDEX IF NOT EXISTS sessions_expires_at_idx
                    ON sessions (expires_at);

                CREATE TABLE IF NOT EXISTS login_challenges (
                    token_hash TEXT PRIMARY KEY,
                    expires_at INTEGER NOT NULL
                );

                CREATE TABLE IF NOT EXISTS login_failures (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    client_key TEXT NOT NULL,
                    username TEXT NOT NULL,
                    attempted_at INTEGER NOT NULL
                );

                CREATE INDEX IF NOT EXISTS login_failures_lookup_idx
                    ON login_failures (client_key, username, attempted_at);
                """
            )

    def has_users(self) -> bool:
        with self._connect() as connection:
            row = connection.execute("SELECT EXISTS (SELECT 1 FROM users)").fetchone()
            return bool(row[0])

    def create_initial_user(self, username: str, password_hash: str) -> None:
        with self._connect() as connection:
            connection.execute(
                """
                INSERT INTO users (username, password_hash, created_at)
                VALUES (?, ?, ?)
                """,
                (username, password_hash, int(time.time())),
            )

    def get_user(self, username: str) -> sqlite3.Row | None:
        with self._connect() as connection:
            return connection.execute(
                "SELECT id, username, password_hash FROM users WHERE username = ?",
                (username,),
            ).fetchone()

    def create_login_challenge(self, token_hash: str, expires_at: int) -> None:
        now = int(time.time())
        with self._connect() as connection:
            connection.execute("DELETE FROM login_challenges WHERE expires_at <= ?", (now,))
            connection.execute(
                "INSERT INTO login_challenges (token_hash, expires_at) VALUES (?, ?)",
                (token_hash, expires_at),
            )

    def validate_login_challenge(self, token_hash: str) -> bool:
        now = int(time.time())
        with self._connect() as connection:
            connection.execute("DELETE FROM login_challenges WHERE expires_at <= ?", (now,))
            row = connection.execute(
                """
                SELECT EXISTS (
                    SELECT 1 FROM login_challenges
                WHERE token_hash = ? AND expires_at > ?
                )
                """,
                (token_hash, now),
            ).fetchone()
            return bool(row[0])

    def delete_login_challenge(self, token_hash: str) -> None:
        with self._connect() as connection:
            connection.execute(
                "DELETE FROM login_challenges WHERE token_hash = ?",
                (token_hash,),
            )

    def count_recent_failures(
        self,
        client_key: str,
        username: str,
        since: int,
    ) -> int:
        with self._connect() as connection:
            connection.execute("DELETE FROM login_failures WHERE attempted_at < ?", (since,))
            row = connection.execute(
                """
                SELECT count(*)
                FROM login_failures
                WHERE client_key = ? AND username = ? AND attempted_at >= ?
                """,
                (client_key, username, since),
            ).fetchone()
            return int(row[0])

    def record_login_failure(self, client_key: str, username: str) -> None:
        with self._connect() as connection:
            connection.execute(
                """
                INSERT INTO login_failures (client_key, username, attempted_at)
                VALUES (?, ?, ?)
                """,
                (client_key, username, int(time.time())),
            )

    def clear_login_failures(self, client_key: str, username: str) -> None:
        with self._connect() as connection:
            connection.execute(
                "DELETE FROM login_failures WHERE client_key = ? AND username = ?",
                (client_key, username),
            )

    def create_session(
        self,
        token_hash: str,
        user_id: int,
        csrf_hash: str,
        expires_at: int,
    ) -> None:
        now = int(time.time())
        with self._connect() as connection:
            connection.execute("DELETE FROM sessions WHERE expires_at <= ?", (now,))
            connection.execute(
                """
                INSERT INTO sessions (
                    token_hash, user_id, csrf_hash, created_at, expires_at
                ) VALUES (?, ?, ?, ?, ?)
                """,
                (token_hash, user_id, csrf_hash, now, expires_at),
            )

    def get_session(self, token_hash: str) -> sqlite3.Row | None:
        now = int(time.time())
        with self._connect() as connection:
            connection.execute("DELETE FROM sessions WHERE expires_at <= ?", (now,))
            return connection.execute(
                """
                SELECT s.token_hash, s.csrf_hash, s.expires_at, u.id AS user_id, u.username
                FROM sessions AS s
                JOIN users AS u ON u.id = s.user_id
                WHERE s.token_hash = ? AND s.expires_at > ?
                """,
                (token_hash, now),
            ).fetchone()

    def delete_session(self, token_hash: str) -> None:
        with self._connect() as connection:
            connection.execute("DELETE FROM sessions WHERE token_hash = ?", (token_hash,))
