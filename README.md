# pgcli - PostgreSQL CLI Manager for macOS

A lightweight, native macOS C++ application for managing PostgreSQL databases with a CLI-first design.

## Features

- **Connection Management:** Connect to PostgreSQL databases and save connection profiles cleanly.
- **Query Execution:** Run raw SQL queries and view pretty-printed results in auto-formatting ASCII tables.
- **Schema Management:** Describe table structures easily with helper commands.
- **Data Export:** Export table results seamlessly into CSV formats.
- **Logging & Config:** Core structured logger and JSON-based connection profile persistence (`pgcli_config.json`).
- **Memory Safety:** Strictly adheres to RAII resource management for all `PGconn` and `PGresult` internals.

## Architecture Layers

- `src/cli` - Command Line generic parsing and formatting logic.
- `src/core` - Command Handler, Application routing context.
- `src/db` - PostgreSQL RAII Wrapper (`libpq`).
- `src/utils` - Thread-safe logging and Configuration parsing.

## Requirements
- Apple Silicon (M1/M2/M3) or Intel macOS
- Standard Clang toolchain
- CMake 3.14+
- `libpq` (Installed via `brew install libpq`)

## Build Instructions

To compile locally on macOS:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Overview

You can start the interactive CLI:
```bash
./pgcli
```

Or run a command instantly:
```bash
./pgcli connect postgresql://user@localhost/db query "SELECT * FROM users;"
```

### Available Core Commands:
- `help` / `\?`                  - Show help message
- `connect <conn_string>`      - Connect using a standard Postgres connection URI
- `connect profile <name>`     - Connect using a saved profile
- `disconnect` / `\q`            - Disconnect from database
- `profile list`               - List all saved connection profiles
- `profile add <name> <conn>`  - Save a connection profile securely in local JSON
- `query <SQL>` / `q <SQL>`      - Execute arbitrary SQL queries
- `list_tables` / `\dt`         - List all tables in current database
- `describe <table>` / `\d`    - Show detailed table schema
- `export <table> <file>`      - Export basic table data to CSV format
- `exit` / `quit`                - Exit the application
