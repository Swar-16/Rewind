# CLI & API Reference

> **Status:** Active Baseline  
> **Interface:** Command-line executable (`rewind`) and C++20 Domain Library  
> **Source Directory:** `src/`, `include/rewind/`

## 1. CLI Commands & Baseline Flags

The Rewind CLI provides a command-line interface for managing protected folders, browsing historical versions, and safely restoring files.

### 1.1 Baseline Global Flags (Implemented in Step 1)

| Flag            | Description                                                             | Output Preview                                                                       |
| --------------- | ----------------------------------------------------------------------- | ------------------------------------------------------------------------------------ |
| `-v, --version` | Display version, product name, and target platform information          | `Rewind v0.1.0 — Local-first transparent file recovery system (Linux x86_64, C++20)` |
| `-h, --help`    | Display command usage synopsis, global options, and planned subcommands | Full command synopsis and option summary                                             |

### 1.2 Subcommand Matrix (Planned Implementation Steps)

| Command           | Subcommands / Flags                        | Description                                                     | Status           |
| ----------------- | ------------------------------------------ | --------------------------------------------------------------- | ---------------- |
| `rewind init`     | `--store-path <dir>`                       | Initialize the Rewind repository and metadata database.         | Planned (Step 4) |
| `rewind root`     | `add <path>`, `list`, `remove <id>`        | Manage monitored directory roots.                               | Planned (Step 4) |
| `rewind snapshot` | `create [root_id]`, `list`, `inspect <id>` | Trigger manual snapshots and inspect recovery points.           | Planned (Step 4) |
| `rewind timeline` | `[path]`, `--from <date>`, `--to <date>`   | View chronological change history for paths.                    | Planned (Step 7) |
| `rewind diff`     | `<version_a> <version_b>`                  | Compare differences between file revisions.                     | Planned (Step 7) |
| `rewind restore`  | `<path> --snapshot <id> [--dest <path>]`   | Safely restore a file or folder revision.                       | Planned (Step 5) |
| `rewind verify`   | `[--all] [--fix]`                          | Verify data store object hashes and database consistency.       | Planned (Step 9) |
| `rewind status`   | `--json`                                   | Display monitoring status, storage usage, and last scan health. | Planned (Step 7) |
| `rewind gc`       | `[--dry-run]`                              | Perform garbage collection on unreferenced objects.             | Planned (Step 8) |

---

## 2. Core C++ Library API (`rewindfs::common`)

All core subsystems operate under the root namespace `rewindfs` (isolated from glibc `<stdio.h>` `::rewind`).

### 2.1 Monadic Error & Return Primitives

- **`rewindfs::common::ErrorCode`:** Strongly typed enumeration covering POSIX and domain error codes:
  `Success = 0`, `InvalidArgument`, `FileNotFound`, `PathNotFound`, `AlreadyExists`, `PermissionDenied`, `DiskFull`, `DatabaseError`, `MigrationFailed`, `HashMismatch`, `CorruptedObject`, `ObjectNotFound`, `InvalidState`, `IOError`, `NotImplemented`, `InternalError`.
- **`rewindfs::common::RewindError`:** Value-type representing error states with human-readable description, optional path context, and POSIX errno factory mapping (`RewindError::from_errno(err_num, context, path)`).
- **`rewindfs::common::Result<T>`:** Zero-overhead monadic return type:
  - `is_ok()`, `is_err()`, `value()`, `error()`, `value_or(default_val)`.
  - Monadic chaining: `and_then(Func&&)` and `transform(Func&&)`.
- **`rewindfs::common::Result<void>`:** Void specialization for operations with side-effects that return success or error.

### 2.2 Version Primitives

- `VERSION_MAJOR = 0`, `VERSION_MINOR = 1`, `VERSION_PATCH = 0`.
- `VERSION_STRING = "0.1.0"`, `PRODUCT_NAME = "Rewind"`.
- `get_version_banner()`: Produces standardized semantic version banner.

---
*Detailed component specifications and interface contracts will be documented as each subsystem is developed.*
