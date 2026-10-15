# Rewind — Step-by-Step Implementation Guide for Person 2

> **Role:** Metadata, Applications, CLI & Reliability Engineer (Person 2)  
> **Status:** Active Guide · **Version:** 1.0 · **Date:** 2026-10-13  
> **Partner Guide:** [step_by_step_person_1.md](step_by_step_person_1.md)  
> **Master Guide:** [step_by_step_guide.md](step_by_step_guide.md)  
> **Architecture Blueprint:** [docs/full_implementation_plan.md](../full_implementation_plan.md)

---

## 1. Role Scope & Invariants

You are **Person 2**, responsible for SQLite persistence, relational schemas, database migrations, CLI interface parsing, restore planning, safety copy tracking, timeline querying, retention policies, and crash-resilience testing.

### Core Invariants You Must Enforce
1. SQLite must operate with Write-Ahead Logging (`PRAGMA journal_mode = WAL;`) and enforced foreign keys (`PRAGMA foreign_keys = ON;`).
2. Never commit metadata that references unverified or unwritten object files.
3. Every restore operation that overwrites an existing file must create and record a safety copy in `.rewind/safety/`.
4. Restore audits must be immutably recorded in the `restore_operation` table with full undo capability.
5. Missing or dropped observer events must be detected through reconciliation and accurately reported to the user as `INCOMPLETE` history.
6. Every step must be executed in a dedicated branch (`feat/step-XX-person2`), tested independently, and merged with Person 1 at the step completion gate.

---

## 2. Step-by-Step Work Breakdown

### Step 1: Common Infrastructure, Error Model & Baseline CLI [✅ Complete]
- **Branch:** `feat/step-01-person2`
- **Assigned Files:**
  - `include/rewind/common/error.hpp`
  - `src/common/error.cpp`
  - `include/rewind/common/result.hpp`
  - `include/rewind/common/version.hpp`
  - `src/main.cpp`
  - `tests/unit/test_common.cpp`
- **What:** Define the core error types, monadic return types, version constants, and executable entrypoint.
- **Why:** Provide the foundational types used across all subsystems without throwing raw exceptions across boundaries.
- **How:**
  1. `enum class ErrorCode`: `Success`, `FileNotFound`, `PermissionDenied`, `DiskFull`, `DatabaseError`, `HashMismatch`, `InvalidPath`, `ObjectNotFound`, `ConflictError`.
  2. `RewindError`: encapsulates `ErrorCode`, descriptive message, and optional path context.
  3. `Result<T, RewindError>`: lightweight `std::expected`-style monadic type with `.is_ok()`, `.is_err()`, `.value()`, and `.error()`.
  4. `src/main.cpp`: parses `--version` (prints `rewind v0.1.0`) and `--help` usage text.
- **Verification Commands:**
  ```bash
  cmake --preset debug
  cmake --build --preset debug --target rewind_cli test_common
  ./build/debug/bin/rewind --version
  ./build/debug/tests/unit/test_common
  ```
- **Sync with Person 1:** Merge with Person 1's CMake/CI infrastructure on `dev`.

---

### Step 2: SQLite Connection Wrapper & Migration Engine
- **Branch:** `feat/step-02-person2`
- **Assigned Files:**
  - `include/rewind/metadata/database.hpp`
  - `src/metadata/database.cpp`
  - `include/rewind/metadata/migration_runner.hpp`
  - `src/metadata/migration_runner.cpp`
  - `migrations/sqlite/001_initial_schema.sql`
  - `tests/unit/test_database.cpp`
  - `tests/unit/test_migrations.cpp`
- **What:** Implement the transactional SQLite database manager and versioned migration runner.
- **Why:** Ensure reproducible, versioned schema evolution and reliable ACID persistence.
- **How:**
  1. **`Database` Class:**
     - Opens SQLite connection with `sqlite3_open_v2`.
     - Executes pragmas: `PRAGMA journal_mode = WAL;`, `PRAGMA synchronous = NORMAL;`, `PRAGMA foreign_keys = ON;`, `PRAGMA busy_timeout = 5000;`.
     - Provides transaction helpers: `begin_transaction()`, `commit()`, `rollback()`.
     - Provides parameterized statement execution helpers.
  2. **`MigrationRunner` Class:**
     - Creates `schema_migrations` tracking table.
     - Scans and applies unapplied `.sql` scripts in alphabetical order within a single transaction per migration.
  3. **`001_initial_schema.sql`:**
     - Creates `protected_root`, `snapshot`, `file_entry`, `object`, and `event` tables with appropriate indexes (`idx_file_entry_snapshot`, `idx_snapshot_root_time`).
- **Verification Commands:**
  ```bash
  cmake --build --preset debug --target test_database test_migrations
  ./build/debug/tests/unit/test_database
  ./build/debug/tests/unit/test_migrations
  ```
- **Sync with Person 1:** Merge with Person 1's `ObjectStore` implementation on `dev`.

---

### Step 3: Domain Models & SQLite Repository Implementations
- **Branch:** `feat/step-03-person2`
- **Assigned Files:**
  - `include/rewind/domain/protected_root.hpp`
  - `include/rewind/domain/snapshot.hpp`
  - `include/rewind/domain/file_entry.hpp`
  - `include/rewind/domain/object_id.hpp`
  - `include/rewind/metadata/repositories.hpp`
  - `src/metadata/repositories.cpp`
  - `tests/unit/test_repositories.cpp`
- **What:** Implement strongly-typed domain entities and database repository interfaces.
- **Why:** Separate raw SQL query execution from application domain business logic.
- **How:**
  1. Define domain structs:
     - `ProtectedRoot`: `id`, `path`, `is_enabled`, `scan_policy`, `last_scan_at`, `last_scan_status`.
     - `Snapshot`: `id`, `root_id`, `started_at`, `committed_at`, `status`, `manifest_hash`, `total_files`, `total_bytes`.
     - `FileEntry`: `id`, `snapshot_id`, `relative_path`, `object_hash`, `file_size`, `file_mode`, `modified_at`, `entry_type`.
  2. Implement Repositories:
     - `ProtectedRootRepository`: `add(path)`, `get_by_id(id)`, `list_all()`, `remove(id)`.
     - `SnapshotRepository`: `create_pending(root_id)`, `commit(snapshot_id, manifest_hash, stats)`, `get_latest(root_id)`, `list_recent(limit)`.
     - `FileEntryRepository`: `bulk_insert(snapshot_id, entries)`, `find_by_snapshot(snapshot_id)`.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_repositories" --output-on-failure
  ```
- **Sync with Person 1:** Merge with Person 1's `SnapshotCoordinator` on `dev`.

---

### Step 4: CLI Subcommands & Interactive Terminal Presentations
- **Branch:** `feat/step-04-person2`
- **Assigned Files:**
  - `src/cli/cli_parser.hpp`
  - `src/cli/cli_parser.cpp`
  - `src/cli/commands.hpp`
  - `src/cli/commands.cpp`
  - `tests/unit/test_cli.cpp`
- **What:** Build the command-line argument parser and implement core user commands.
- **Why:** Provide the primary developer and user interface for managing Rewind.
- **How:**
  1. Implement robust command parsing without heavy external dependencies:
     - Subcommands: `init`, `root`, `snapshot`, `status`.
  2. Command implementations:
     - `cmd_init(store_path)`: creates directory tree, initializes SQLite DB, runs migrations.
     - `cmd_root(action, args)`: adds, lists, or removes protected directory roots.
     - `cmd_snapshot_create(root_id)`: triggers manual snapshot and prints formatted summary.
     - `cmd_snapshot_list(root_id)`: displays ASCII table of recorded snapshots with IDs, timestamps, file counts, and sizes.
  3. Format output cleanly with clear error messages and exit codes.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_cli" --output-on-failure
  ./build/debug/bin/rewind root list
  ```
- **Sync with Person 1:** Merge with Person 1's filesystem scanner; execute manual snapshot test end-to-end.

---

### Step 5: Restore Planner, Conflict Resolver & Undo Audit
- **Branch:** `feat/step-05-person2`
- **Assigned Files:**
  - `include/rewind/restore/restore_planner.hpp`
  - `src/restore/restore_planner.cpp`
  - `include/rewind/restore/conflict_resolver.hpp`
  - `src/restore/conflict_resolver.cpp`
  - `migrations/sqlite/002_add_restore_audit.sql`
  - `tests/unit/test_restore_planner.cpp`
- **What:** Plan safe restores, detect existing destination conflicts, generate safety copies, and record audit entries.
- **Why:** Invariant #2: A restore must not silently destroy the only available copy of the current destination.
- **How:**
  1. `RestorePlanner::create_plan(snapshot_id, relative_path, override_destination)`:
     - Looks up target `file_entry` from metadata.
     - Determines target path on live filesystem.
     - Inspects destination: checks whether file exists, permissions, and conflict type.
  2. `ConflictResolver::create_safety_copy(dest_path)`:
     - Copies existing file to `.rewind/safety/YYYYMMDD_HHMMSS_<filename>`.
     - Logs operation in SQLite `restore_operation` table.
  3. CLI Commands:
     - `rewind restore <path> --snapshot <id> [--dest <path>]`: executes plan with safety copy.
     - `rewind restore --undo <restore_id>`: rolls back restore using recorded safety copy.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_restore_planner" --output-on-failure
  ```
- **Sync with Person 1:** Merge with Person 1's `RestoreExecutor` on `dev`; verify end-to-end restore.

---

### Step 6: Periodic Reconciliation Scanner & System Status
- **Branch:** `feat/step-06-person2`
- **Assigned Files:**
  - `include/rewind/observer/reconciliation_scanner.hpp`
  - `src/observer/reconciliation_scanner.cpp`
  - `include/rewind/app/app_coordinator.hpp`
  - `src/app/app_coordinator.cpp`
  - `tests/unit/test_reconciliation_scanner.cpp`
- **What:** Build background reconciliation scheduler and high-level application coordinator.
- **Why:** Invariant #6: Event loss triggers reconciliation or an incomplete-history warning.
- **How:**
  1. `ReconciliationScanner`:
     - Runs on scheduled intervals (every 15 minutes) or on demand.
     - Compares current filesystem state against latest committed snapshot manifest.
     - Emits synthetic change events for any undetected changes.
  2. History Confidence State:
     - Marks `history_status = INCOMPLETE` if an inotify queue overflow occurred since last scan.
     - Resets to `COMPLETE` upon successful completion of a full reconciliation pass.
  3. `rewind status` CLI: reports monitoring status, last snapshot, history completeness, and storage metrics.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_reconciliation" --output-on-failure
  ./build/debug/bin/rewind status
  ```
- **Sync with Person 1:** Merge with Person 1's `InotifyWatcher` and coalescer on `dev`.

---

### Step 7: Timeline Query Engine & Chronological Change Browser
- **Branch:** `feat/step-07-person2`
- **Assigned Files:**
  - `include/rewind/domain/event.hpp`
  - `include/rewind/timeline/timeline_query.hpp`
  - `src/timeline/timeline_query.cpp`
  - `tests/unit/test_timeline_query.cpp`
- **What:** Implement timeline query filters and user-facing timeline inspection subcommands.
- **Why:** Enable users to answer: "What changed? When did it change? Which snapshots contain it?"
- **How:**
  1. `TimelineQueryEngine`:
     - Queries history of a specific file or whole directory tree across snapshots.
     - Classifies changes: `CREATED`, `MODIFIED`, `DELETED`, `RENAMED`.
     - Filters by date range (`--from`, `--to`), event type, and keyword.
  2. CLI Command `rewind timeline`:
     - Displays formatted interactive chronological timeline in terminal.
     - Supports `--json` flag for machine-readable output.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_timeline" --output-on-failure
  ./build/debug/bin/rewind timeline
  ```
- **Sync with Person 1:** Merge with Person 1's text diff engine on `dev`.

---

### Step 8: Retention Policy Engine & Safe Snapshot Pruning
- **Branch:** `feat/step-08-person2`
- **Assigned Files:**
  - `include/rewind/retention/retention_policy.hpp`
  - `src/retention/retention_policy.cpp`
  - `tests/unit/test_retention_policy.cpp`
- **What:** Evaluate retention rules (age and count limits) and prune obsolete snapshot manifests.
- **Why:** Keep metadata and storage usage bounded over time while protecting critical recovery points.
- **How:**
  1. `RetentionPolicy` rules:
     - Keep all snapshots for 24 hours.
     - Keep 1 snapshot per day for 30 days.
     - Keep 1 snapshot per week thereafter.
     - Keep at least the latest N snapshots regardless of age.
  2. `prune_snapshots(policy)`: marks expired snapshots as `PURGED` in database transaction.
  3. CLI Command `rewind gc`: invokes retention policy pruning, then calls Person 1's `GarbageCollector`.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_retention_policy" --output-on-failure
  ```
- **Sync with Person 1:** Merge with Person 1's reachability garbage collector on `dev`.

---

### Step 9: Crash-Injection Test Harness & Performance Benchmarks
- **Branch:** `feat/step-09-person2`
- **Assigned Files:**
  - `tests/crash/test_commit_boundary_crash.cpp`
  - `tests/crash/test_restore_interruption.cpp`
  - `tools/benchmark_runner/benchmark_main.cpp`
- **What:** Implement simulated crash injection at persistence boundaries and measure system performance.
- **Why:** Prove Invariants #1, #3, and performance targets (<50 MiB memory, near 0% idle CPU).
- **How:**
  1. `test_commit_boundary_crash.cpp`:
     - Uses fork/exec or custom crash hooks to simulate SIGKILL:
       - Case A: Crash during temporary object write.
       - Case B: Crash after object flush, before SQLite transaction commit.
       - Case C: Crash during SQLite transaction.
     - Verifies that subsequent restart safely cleans up partial state and commits remain consistent.
  2. `benchmark_main.cpp`:
     - Measures 1,000 files capture time, deduplication ratio, peak memory, and restore latency.
- **Verification Commands:**
  ```bash
  cmake --build --preset release --target benchmark_runner
  ./build/release/tools/benchmark_runner/benchmark_runner
  ctest --preset debug -R "test_commit_boundary_crash|test_restore_interruption" --output-on-failure
  ```
- **Sync with Person 1:** Merge with Person 1's security test suite on `dev`.

---

### Step 10: Documentation Wiki Synchronization & Final Release Smoke Test
- **Branch:** `feat/step-10-person2`
- **Assigned Files:**
  - `examples/sample-config.toml`
  - Progressively populate `wiki/` files (`ARCHITECTURE.md`, `DATABASE.md`, `CLI_AND_API.md`, `STORAGE_AND_INTEGRITY.md`, `RESTORE_AND_SAFETY.md`, `OBSERVER_AND_RECONCILIATION.md`, `TESTING.md`, `DEPLOYMENT.md`, `UML.md`)
  - `CHANGELOG.md`
- **What:** Populate complete technical documentation in `wiki/`, generate example configurations, and conduct end-to-end release smoke tests.
- **Why:** Provide turnkey usability and accurate documentation reflecting all verified behavior.
- **How:**
  1. Update `wiki/*.md` with final API signatures, schemas, and CLI options developed across Steps 1–9.
  2. Create comprehensive `examples/sample-config.toml` demonstrating watch paths, retention rules, and exclusions.
  3. Run full workflow test: `init` -> `root add` -> modify files -> inspect `timeline` -> `diff` -> `restore` -> `restore --undo` -> `gc`.
- **Verification Commands:**
  ```bash
  cmake --preset release && cmake --build --preset release
  ctest --preset release --output-on-failure
  ```
- **Sync with Person 1:** Final merge into `main`, tag `v0.1.0` release.
