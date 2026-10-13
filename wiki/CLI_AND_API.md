# CLI & API Reference

> **Status:** Outline / Planned  
> **Interface:** Command-line executable (`rewind`) and C++20 Domain Library  
> **Source Directory:** `src/cli/`, `include/rewind/app/`

## 1. CLI Commands Overview

The Rewind CLI provides an intuitive command-line interface for managing protected folders, browsing historical versions, and safely restoring files.

| Command | Subcommands / Flags | Description |
|---|---|---|
| `rewind init` | `--store-path <dir>` | Initialize the Rewind repository and metadata database. |
| `rewind root` | `add <path>`, `list`, `remove <id>` | Manage monitored directory roots. |
| `rewind snapshot` | `create [root_id]`, `list`, `inspect <id>` | Trigger manual snapshots and inspect recovery points. |
| `rewind timeline` | `[path]`, `--from <date>`, `--to <date>` | View chronological change history for paths. |
| `rewind diff` | `<version_a> <version_b>` | Compare differences between file revisions. |
| `rewind restore` | `<path> --snapshot <id> [--dest <path>]` | Safely restore a file or folder revision. |
| `rewind verify` | `[--all] [--fix]` | Verify data store object hashes and database consistency. |
| `rewind status` | `--json` | Display monitoring status, storage usage, and last scan health. |
| `rewind gc` | `[--dry-run]` | Perform garbage collection on unreferenced objects. |

## 2. Core C++ Library API

- `AppCoordinator`: Top-level orchestrator for user actions.
- `SnapshotCoordinator`: Orchestrates scanning, object publication, and snapshot commitment.
- `RestorePlanner` & `RestoreExecutor`: Generates and executes verified restore plans.
- `StorageEngine`: Direct interface to immutable content objects.
- `MetadataRepository`: Interface to SQLite entities and queries.

---
*Command options, output formats, exit codes, and API signatures will be documented as CLI and library components are developed.*
