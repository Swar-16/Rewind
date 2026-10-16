# Rewind — Step-by-Step Implementation Guide for Person 1

> **Role:** Systems, Storage & Linux Platform Engineer (Person 1)  
> **Status:** Active Guide · **Version:** 1.0 · **Date:** 2026-10-13  
> **Partner Guide:** [step_by_step_person_2.md](step_by_step_person_2.md)  
> **Master Guide:** [step_by_step_guide.md](step_by_step_guide.md)  
> **Architecture Blueprint:** [docs/full_implementation_plan.md](../full_implementation_plan.md)

---

## 1. Role Scope & Invariants

You are **Person 1**, responsible for the core systems, low-level OS primitives, cryptographic storage, safe filesystem traversal, inotify observation, and Linux platform integration.

### Core Invariants You Must Enforce
1. Raw object files must be flushed to disk (`fdatasync`/`fsync`) before publishing them as valid objects.
2. Temporary staging files must never collide or escape sandbox boundaries (`PathSanitizer`).
3. Never traverse symlinks outside the protected root boundary.
4. Process termination during storage writes must never corrupt existing immutable objects.
5. Inotify buffer overflow (`IN_Q_OVERFLOW`) must be detected and propagated to trigger reconciliation.
6. Every step must be executed in a dedicated branch (`feat/step-XX-person1`), tested independently, and merged with Person 2 at the step completion gate.

---

## 2. Step-by-Step Work Breakdown

### Step 1: Build Foundation, Toolchains & CI Infrastructure [✅ Complete]
- **Branch:** `feat/step-01-person1` (Merged into `main` via PR #1)
- **Status:** ✅ Complete (Commit `7123462`)
- **Assigned Files:**
  - `CMakeLists.txt` (Root configuration)
  - `CMakePresets.json` (Debug, Release, ASan, TSan presets)
  - `cmake/CompilerWarnings.cmake` (Strict `-Wall -Wextra -Wpedantic -Werror`)
  - `cmake/Sanitizers.cmake` (ASan, UBSan, TSan setup)
  - `cmake/Dependencies.cmake` (FetchContent / FindPackage for GTest and SQLite3)
  - `.clang-format` (LLVM style rules)
  - `.clang-tidy` (Modern C++ linter checks)
  - `.github/workflows/ci.yml` (Ubuntu CI matrix: GCC 13, Clang 17)
- **What:** Configure the modern C++20 build framework and automated quality gates.
- **Why:** Establish a zero-warning, reproducible build environment before writing domain logic.
- **How:**
  1. Define CMake standard `CMAKE_CXX_STANDARD 20` and `CMAKE_CXX_STANDARD_REQUIRED ON`.
  2. Structure library targets: `rewind_core` (object library or static library) and executable `rewind_cli`.
  3. Wire GoogleTest via `FetchContent` or `find_package(GTest REQUIRED)` with `CTest`.
  4. Write GitHub Actions workflow running on pull requests to `main` and `dev`.
- **Verification Commands:**
  ```bash
  cmake --preset debug
  cmake --build --preset debug
  ctest --preset debug --output-on-failure
  ```
- **Sync with Person 2:** ✅ Integrated with Person 2's `common/` types (`feat/step-01-person2`); joint build and test suite passed across all presets.
- **Implementation Report:** [Implementations/step_01_build_foundation.md](../../Implementations/step_01_build_foundation.md)

---

### Step 2: Content-Addressed Immutable Object Store & BLAKE3 Hasher
- **Branch:** `feat/step-02-person1`
- **Assigned Files:**
  - `include/rewind/storage/hasher.hpp`
  - `src/storage/hasher.cpp`
  - `include/rewind/storage/object_store.hpp`
  - `src/storage/object_store.cpp`
  - `include/rewind/storage/storage_layout.hpp`
  - `src/storage/storage_layout.cpp`
  - `tests/unit/test_hasher.cpp`
  - `tests/unit/test_object_store.cpp`
- **What:** Implement streaming cryptographic hashing and atomic, immutable content-addressed storage.
- **Why:** Guarantee content deduplication, bit-rot detection, and crash-resilient file persistence.
- **How:**
  1. **`Hasher` Class:** Provide streaming interface `init()`, `update(std::span<const uint8_t>)`, and `finalize() -> std::string` (returning hex-encoded BLAKE3 or SHA-256 hash).
  2. **`ObjectStore` Class:**
     - `write_object(std::istream& stream) -> Result<ObjectId, RewindError>`:
       - Generates UUID/unique filename in `.rewind/tmp/`.
       - Streams bytes from input stream while simultaneously updating `Hasher`.
       - Calls `fdatasync()` on file descriptor before close.
       - Publishes file into `.rewind/objects/ab/cd...` using `renameat2` (`RENAME_NOREPLACE`).
       - If destination object already exists (duplicate), unlinks temporary file and returns hash successfully.
     - `read_object(const ObjectId& id) -> Result<std::ifstream, RewindError>`:
       - Opens object file for reading.
     - `verify_object(const ObjectId& id) -> bool`:
       - Re-reads and re-hashes object to catch storage corruption.
- **Verification Commands:**
  ```bash
  cmake --build --preset debug --target test_hasher test_object_store
  ./build/debug/tests/unit/test_hasher
  ./build/debug/tests/unit/test_object_store
  ```
- **Sync with Person 2:** Merge with Person 2's SQLite database PR on `dev`.

---

### Step 3: Snapshot Commit Coordinator & Storage Durability Flush
- **Branch:** `feat/step-03-person1`
- **Assigned Files:**
  - `include/rewind/snapshot/commit_protocol.hpp`
  - `src/snapshot/commit_protocol.cpp`
  - `include/rewind/snapshot/snapshot_coordinator.hpp`
  - `src/snapshot/snapshot_coordinator.cpp`
  - `tests/unit/test_commit_protocol.cpp`
- **What:** Orchestrate the strict two-phase commit ordering between disk objects and metadata manifests.
- **Why:** Invariant #1: A committed snapshot must never reference missing, incomplete, or unverified objects.
- **How:**
  1. Define commit states: `Phase 1: SYNC_OBJECTS`, `Phase 2: COMMIT_METADATA`, `Phase 3: PUBLISHED`.
  2. Ensure all newly detected files have their object payloads completely written and synced to disk before calling Person 2's metadata repository transaction.
  3. Implement startup crash recovery: scan `.rewind/tmp/` on boot and unlink abandoned staging files.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_commit_protocol" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's repository implementation; run joint snapshot test.

---

### Step 4: Filesystem Scanner, Path Sanitizer & Traversal Policy
- **Branch:** `feat/step-04-person1`
- **Assigned Files:**
  - `include/rewind/filesystem/path_sanitizer.hpp`
  - `src/filesystem/path_sanitizer.cpp`
  - `include/rewind/filesystem/traversal_policy.hpp`
  - `include/rewind/filesystem/scanner.hpp`
  - `src/filesystem/scanner.cpp`
  - `tests/unit/test_path_sanitizer.cpp`
  - `tests/unit/test_scanner.cpp`
- **What:** Safely traverse monitored directories, detect modified files, and enforce security policies.
- **Why:** Prevent directory traversal attacks, infinite symlink loops, and scanning unauthorized paths.
- **How:**
  1. `PathSanitizer::resolve_relative(root, untrusted_path)`: verifies canonical path remains strictly within root boundary.
  2. `TraversalPolicy`: configure exclusions (`.git`, `.rewind`, temporary editor files `*.tmp`, `*~`), max file size limit, and symlink behavior.
  3. `FilesystemScanner::scan_directory(root)`:
     - Uses `std::filesystem::recursive_directory_iterator` with `symlink_options::no_follow`.
     - Records file size, permissions, `mtime`, and detects concurrent writes during stat.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_scanner|test_path_sanitizer" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's CLI commands (`rewind root add`, `rewind snapshot create`).

---

### Step 5: Platform File Operations & Restore Content Verification
- **Branch:** `feat/step-05-person1`
- **Assigned Files:**
  - `include/rewind/platform/file_ops.hpp`
  - `src/platform/linux/file_ops.cpp`
  - `include/rewind/restore/restore_executor.hpp`
  - `src/restore/restore_executor.cpp`
  - `tests/unit/test_restore_executor.cpp`
- **What:** Execute verified byte staging and atomic destination replacement.
- **Why:** Invariant #3: Restore verifies historical content before publishing it as the restored file.
- **How:**
  1. `LinuxFileOps::atomic_replace(source_staged, target_dest)`: wraps `renameat2()` with POSIX atomic semantics.
  2. `RestoreExecutor::execute(plan)`:
     - Streams object from `ObjectStore` into staging file `target_dest + ".rewind_stage"`.
     - Re-computes BLAKE3 hash on staging file.
     - Compares calculated hash with expected snapshot hash; aborts with `RewindError::HashMismatch` if corrupt.
     - Preserves original permissions and file mode.
     - Atomically swaps staged file into final destination path.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_restore_executor" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's restore planner and safety copy manager; verify full restore flow.

---

### Step 6: Linux Inotify Observer & Event Coalescing Queue
- **Branch:** `feat/step-06-person1`
- **Assigned Files:**
  - `include/rewind/observer/event.hpp`
  - `include/rewind/observer/inotify_watcher.hpp`
  - `src/observer/inotify_watcher.cpp`
  - `include/rewind/observer/coalescer.hpp`
  - `src/observer/coalescer.cpp`
  - `tests/unit/test_coalescer.cpp`
  - `tests/unit/test_inotify_watcher.cpp`
- **What:** Listen for kernel filesystem change events with debouncing and queue bounds.
- **Why:** Capture file modifications transparently without high CPU polling while handling rapid bursts.
- **How:**
  1. `InotifyWatcher`: calls `inotify_init1(IN_NONBLOCK | IN_CLOEXEC)` and adds watches recursively (`IN_CLOSE_WRITE`, `IN_CREATE`, `IN_DELETE`, `IN_MOVED_FROM`, `IN_MOVED_TO`).
  2. `EventCoalescer`:
     - Bounded ring buffer holding up to 10,000 raw events.
     - Applies quiet-window timer (default: 500ms): file events for the same path are collapsed into the final state.
     - Detects `IN_Q_OVERFLOW` and flags buffer degradation.
- **Verification Commands:**
  ```bash
  cmake --preset tsan && cmake --build --preset tsan
  ctest --preset tsan -R "test_coalescer|test_inotify" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's reconciliation scanner and app coordinator integration.

---

### Step 7: Text Diff Generator & File Revision Comparison
- **Branch:** `feat/step-07-person1`
- **Assigned Files:**
  - `include/rewind/timeline/diff_engine.hpp`
  - `src/timeline/diff_engine.cpp`
  - `tests/unit/test_diff_engine.cpp`
- **What:** Generate unified diffs comparing two file versions from the object store.
- **Why:** Allow users to inspect exact changes before confirming a restore.
- **How:**
  1. Implement bounded Myers diff algorithm for text files (< 2 MiB).
  2. For binary files or files exceeding 2 MiB, return metadata difference (size change, hash difference).
  3. Format diff output with terminal ANSI color codes (+green, -red, @cyan).
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_diff_engine" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's CLI `rewind diff` and `rewind timeline` commands.

---

### Step 8: Reachability-Based Garbage Collector Engine
- **Branch:** `feat/step-08-person1`
- **Assigned Files:**
  - `include/rewind/retention/garbage_collector.hpp`
  - `src/retention/garbage_collector.cpp`
  - `tests/unit/test_garbage_collector.cpp`
- **What:** Safely prune unreferenced object files using mark-and-sweep reachability.
- **Why:** Reclaim disk space without violating Invariant #5 (never delete objects referenced by committed snapshots).
- **How:**
  1. `GarbageCollector::analyze_reachability(active_hashes)`: marks all active object IDs.
  2. Sweeps `.rewind/objects/` tree and identifies orphaned files.
  3. Implements grace period (only deletes orphans older than 24 hours).
  4. Supports dry-run reporting (calculates reclaimable bytes without unlinking).
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_garbage_collector" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's retention policy rules and `rewind gc` CLI.

---

### Step 9: Security Hardening & Edge-Case Test Suites
- **Branch:** `feat/step-09-person1`
- **Assigned Files:**
  - `tests/security/test_symlink_traversal.cpp`
  - `tests/security/test_path_escape.cpp`
  - `tests/security/test_permissions.cpp`
- **What:** Rigorously stress test Rewind against malicious inputs and adversarial directory structures.
- **Why:** Ensure security boundaries and unprivileged execution safety.
- **How:**
  1. Create symlinks pointing to `/etc/passwd` and circular directory links; verify scanner ignores them.
  2. Feed malicious relative paths (`../../../sensitive`); verify `PathSanitizer` rejects them.
  3. Test read-only permissions and disk permission denials.
- **Verification Commands:**
  ```bash
  ctest --preset debug -R "test_symlink|test_path_escape|test_permissions" --output-on-failure
  ```
- **Sync with Person 2:** Merge with Person 2's crash injection test harness and benchmarks.

---

### Step 10: Systemd Service, Packaging & Final Release Quality
- **Branch:** `feat/step-10-person1`
- **Assigned Files:**
  - `packaging/linux/rewind.service`
  - `packaging/linux/control`
  - CPack packaging configuration in `CMakeLists.txt`
- **What:** Package Rewind for distribution as Debian (`.deb`) and RPM (`.rpm`) packages with systemd background daemon supervision.
- **Why:** Enable single-command installation and native Linux desktop background autostart.
- **How:**
  1. Configure `CPack` with package metadata, licenses, and architecture (`amd64`, `arm64`).
  2. Author `rewind.service` systemd user unit invoking `rewind daemon`.
  3. Verify clean package installation on fresh Linux environments.
- **Verification Commands:**
  ```bash
  cpack -G DEB
  cpack -G RPM
  dpkg -c rewind-*.deb
  ```
- **Sync with Person 2:** Merge final wiki documentation and release notes; perform complete end-to-end smoke test.
