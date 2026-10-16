# Rewind: Local-First Workspace History and Recovery

> **A lightweight, local-first time machine for the files that matter.**
>
> Rewind quietly maintains history for user-selected folders on Linux. When a mistake removes, replaces, or damages your work, you can inspect what changed and restore the version you need. No manual commits required. No cloud account needed. No risk of damaging current work.

**Target Platform:** Linux Desktop (x86_64, aarch64)  
**Implementation:** Modern C++20, CMake 3.22+, SQLite 3  
**Status:** Step 1 Completed (C++20 Build Foundation & Common Subsystem Operational)

---

## The Story

At 11:47 p.m., someone runs a cleanup command with a bad path. A semester project disappears, a developer finds a script has overwritten a configuration file, or an uncommitted source file vanishes.

The immediate reaction is simple: *I only need the version from ten minutes ago.*

Existing backup tools often require complex remote configuration, periodic manual commits, or full operating system rollbacks. Rewind is built around a single, testable promise:

> **If Rewind has recorded a file version, the user can find it and restore it without damaging the last valid recovery point or silently overwriting current work.**

---

## 1. Key Architectural Highlights

- **Modular Monolith in Modern C++20:** Engineered with strict RAII, explicit ownership semantics, value types, and zero raw owning pointers. Decoupled domain interfaces ensure core operations remain independent of CLI or UI layers.
- **Content-Addressed Immutable Store:** File content is saved as immutable objects addressed by cryptographic hashes (BLAKE3 / SHA-256) inside a two-level sharded directory structure (`.rewind/objects/ab/cd...`), ensuring natural deduplication.
- **Transactional SQLite Metadata:** High-performance local metadata tracking using SQLite 3 in Write-Ahead Logging (WAL) mode. Snapshots, file manifests, and change events are committed within atomic database transactions.
- **Crash-Resilient Two-Phase Commit:** Strict commit ordering guarantees that object data is flushed and synced to disk before any metadata manifest references it. A process crash never produces a committed snapshot pointing to missing or partial data.
- **Linux Inotify Observer & Reconciliation:** Kernel filesystem notifications (`inotify`) are buffered in a bounded queue and coalesced across quiet periods. A periodic reconciliation scanner runs in the background to detect missed events and queue overflows.
- **Multi-Step Safe Restore Protocol:** Every restore writes candidate bytes to a temporary location, verifies its content hash, and creates an automated safety copy of any existing destination file before performing an atomic replacement.
- **100% Local-First & Privacy-Preserving:** Operates fully offline without network requests, accounts, or telemetry. Log files never include file content by default.

---

## 2. Quick Start Guide

### 2.1 Prerequisites
- Linux OS (Kernel 5.10+ recommended)
- GCC 13+ or Clang 17+ (C++20 support required)
- CMake 3.22+ & Ninja or Make
- SQLite 3.40+ development headers (`libsqlite3-dev`)

### 2.2 Build from Source
```bash
# Configure debug build with CMake preset
cmake --preset debug

# Build all targets
cmake --build --preset debug

# Or configure and build optimized release binary
cmake --preset release
cmake --build --preset release
```

### 2.3 CLI Usage Preview
```bash
# Initialize local repository
./build/release/bin/rewind init --store-path ~/.local/share/rewind

# Add a folder to continuous monitoring
./build/release/bin/rewind root add ~/projects/my_app

# Check current status and monitored roots
./build/release/bin/rewind status

# Trigger a manual snapshot
./build/release/bin/rewind snapshot create

# Inspect historical timeline for a file
./build/release/bin/rewind timeline ~/projects/my_app/main.cpp

# Safely restore an earlier version
./build/release/bin/rewind restore ~/projects/my_app/main.cpp --snapshot 42
```

---

## 3. Verification & Testing

Rewind includes comprehensive test suites across unit logic, system integration, crash boundaries, and security rules:

```bash
# Run complete test suite via CTest
ctest --preset debug --output-on-failure

# Build and run with AddressSanitizer and UndefinedBehaviorSanitizer
cmake --preset asan
cmake --build --preset asan
ctest --preset asan --output-on-failure

# Build and run with ThreadSanitizer
cmake --preset tsan
cmake --build --preset tsan
ctest --preset tsan --output-on-failure
```

---

## 4. Documentation & Wiki Index

Comprehensive technical documentation is maintained under the `wiki/` directory and synchronized automatically to the GitHub Wiki:

| Document | Location | Description |
|---|---|---|
| **Wiki Home** | [wiki/Home.md](wiki/Home.md) | Official documentation hub and navigation index. |
| **System Architecture** | [wiki/ARCHITECTURE.md](wiki/ARCHITECTURE.md) | Decoupled subsystems, modular design, and data pipeline. |
| **Database & Schema** | [wiki/DATABASE.md](wiki/DATABASE.md) | SQLite 3 schemas, entity relationships, indexes, and transactions. |
| **Storage & Integrity** | [wiki/STORAGE_AND_INTEGRITY.md](wiki/STORAGE_AND_INTEGRITY.md) | Content-addressed storage, hashing, and corruption prevention. |
| **CLI & API Reference** | [wiki/CLI_AND_API.md](wiki/CLI_AND_API.md) | CLI commands, options, exit codes, and C++ library APIs. |
| **Observer & Reconciliation** | [wiki/OBSERVER_AND_RECONCILIATION.md](wiki/OBSERVER_AND_RECONCILIATION.md) | Inotify watch management, coalescing, and scan reconciliation. |
| **Restore & Safety** | [wiki/RESTORE_AND_SAFETY.md](wiki/RESTORE_AND_SAFETY.md) | Verified restore flow, conflict policies, and safety copies. |
| **Testing Strategy** | [wiki/TESTING.md](wiki/TESTING.md) | Unit tests, integration suites, crash-injection matrix, and benchmarks. |
| **Deployment & Operations** | [wiki/DEPLOYMENT.md](wiki/DEPLOYMENT.md) | Toolchain requirements, CMake presets, Linux packaging, and systemd daemon. |
| **UML & Diagrams** | [wiki/UML.md](wiki/UML.md) | Architecture models, sequence diagrams, and schema ERD. |
| **Master Implementation Plan** | [docs/full_implementation_plan.md](docs/full_implementation_plan.md) | Complete end-to-end blueprint and implementation roadmap. |
| **Product Requirements (PRD)** | [docs/product/PRD.md](docs/product/PRD.md) | Foundational product specification, target users, and phases. |

---

## 5. Strategic Roadmap

- **Phase 0–1:** Problem validation, C++20 build foundation, CI, sanitizers, and baseline CLI.
- **Phase 2–3:** Content-addressed object store, SQLite persistence, and transactional snapshot commit.
- **Phase 4–5:** Safe directory scanning, manual snapshots, and verified multi-step restore safety.
- **Phase 6–7:** Linux `inotify` observer, reconciliation scanning, timeline inspection, and usable CLI.
- **Phase 8–9:** Retention policies, garbage collection, benchmarks, and release hardening.
- **Phase 10–11:** Optional encrypted peer-to-peer device replication and multi-platform expansion.
