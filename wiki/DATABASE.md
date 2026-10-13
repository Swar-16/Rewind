# Database & Persistence Schema

> **Status:** Outline / Planned  
> **Engine:** SQLite 3 (WAL mode)  
> **Source Directory:** `migrations/sqlite/`, `src/metadata/`, `include/rewind/metadata/`

## 1. Overview

Rewind uses SQLite 3 as its transactional metadata repository. SQLite stores protected root configurations, snapshot manifests, file entries, event logs, and restore audits.

## 2. SQLite Configuration & Guarantees

- **Journal Mode:** Write-Ahead Logging (`PRAGMA journal_mode = WAL;`) for concurrent read access during snapshot creation.
- **Synchronous Mode:** Normal or Full depending on durability configuration (`PRAGMA synchronous = NORMAL;`).
- **Foreign Keys:** Enforced at runtime (`PRAGMA foreign_keys = ON;`).
- **Busy Timeout:** Configured to handle lock contention gracefully (`PRAGMA busy_timeout = 5000;`).

## 3. Schema Entities & Tables

- `protected_root`: Tracked directories, recursive policies, enabled flags, and last scan status.
- `snapshot`: Atomic recovery points, root IDs, timestamps, status (in-progress, committed, failed), file counts, and logical byte totals.
- `file_entry`: Manifest items linking a path in a snapshot to an immutable content object hash, including permissions and modification times.
- `object`: Content-addressed objects, hashes, payload size, storage format version, and integrity verification timestamps.
- `event`: Filesystem change events, event types (create, modify, delete, rename), timestamps, and source confidence.
- `restore_operation`: Audit log of restore actions, destination paths, safety copy references, and execution status.

## 4. Migration Architecture

Versioned SQL migrations managed under `migrations/sqlite/` with an internal `schema_migrations` tracking table to ensure reproducible schema evolution across upgrades.

---
*Exact DDL statements, indexes, and query performance profiles will be documented upon completing Phase 3.*
