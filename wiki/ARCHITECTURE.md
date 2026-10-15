# System Architecture

> **Status:** Active Baseline (Step 1 Foundation & Common Subsystem Operational)  
> **Target:** Linux Desktop, C++20  
> **Source Directory:** `src/`, `include/rewind/`

## 1. Overview and Design Philosophy

Rewind is structured as a modular monolith in C++20. High-level responsibilities are decoupled across distinct subsystems to enforce clear ownership, explicit lifetimes, zero raw owning pointers, and high testability.

### Root Namespace Convention (`rewindfs`)
To eliminate redeclaration collisions with the glibc POSIX C function `extern void rewind(FILE *stream);` declared at the global scope in `<stdio.h>`, all core C++ entities are declared under the root namespace `rewindfs` (e.g., `rewindfs::common`, `rewindfs::storage`, `rewindfs::metadata`).

## 2. High-Level Data Flow

```text
Filesystem events → Event queue → Coalescer ─┐
                                             ├→ Snapshot coordinator
Periodic reconciliation scan ────────────────┘
                                                    ↓
                                  Immutable content-addressed store
                                                    ↕
                                  SQLite metadata and snapshot manifests
                                                    ↓
                                      Timeline / Search / Restore planner
                                                    ↓
                                      Verified restore + safety copy
```

## 3. Subsystem Breakdown

- **`common`:** Shared types across all subsystems: `rewindfs::common::RewindError`, domain `ErrorCode`, monadic `Result<T>` and `Result<void>`, and semantic versioning primitives. (Implemented in Step 1)
- **`observer`:** Linux filesystem notification backend (`inotify`), event translation, queue management, and quiet-period coalescing.
- **`filesystem`:** Safe directory traversal, permission handling, file identity detection, and symlink policy enforcement.
- **`snapshot`:** Orchestrates scans, content hashing, manifest generation, and coordinates transactional commits.
- **`storage`:** Content-addressed immutable object store, cryptographic hashing (BLAKE3 / SHA-256), directory sharding, and integrity checks.
- **`metadata`:** SQLite database wrapper, schema migration management, and repository interfaces.
- **`restore`:** Multi-step safe restore engine: plan generation, conflict checking, temporary file write, hash verification, safety copy creation, and atomic replacement.
- **`retention`:** Retention policy evaluation, dry-run pruning, and reachability-based garbage collection.
- **`timeline`:** Change classification, diff generation, and timeline query engine.
- **`cli`:** Command-line user interface and terminal presentations.
- **`platform`:** Linux platform primitives and OS abstraction boundaries.

## 4. Key Architectural Trade-Offs

- Whole-file content-addressing vs. chunking (starting with whole-file objects for simplicity and measuring before chunking).
- Inotify event queue limits vs. periodic reconciliation scan.
- Transactional commit boundary ordering (writing immutable objects before committing SQLite snapshot manifest).

---
*Detailed component specifications and interface contracts will be documented as each subsystem is developed.*
