# Rewind Documentation Wiki

Welcome to the official documentation and technical wiki for **Rewind**, a lightweight, local-first file recovery system designed for Linux desktop using modern C++20.

Rewind maintains automatic local history for user-selected directories, stores deduplicated file contents as immutable objects, tracks recovery metadata transactionally in SQLite, and enables safe, verified file-level restores without risking existing workspace data.

---

## Documentation Index

| Wiki Guide | File Link | Description |
|---|---|---|
| System Architecture | [ARCHITECTURE.md](ARCHITECTURE.md) | Modular monolith architecture, data flow pipeline, and component interface boundaries. |
| Database & Schema | [DATABASE.md](DATABASE.md) | SQLite persistence, table schemas, entity relationships, indexes, and transactional commit guarantees. |
| Storage & Integrity | [STORAGE_AND_INTEGRITY.md](STORAGE_AND_INTEGRITY.md) | Immutable content-addressed storage, cryptographic hashing, directory sharding, and crash durability. |
| CLI & API Reference | [CLI_AND_API.md](CLI_AND_API.md) | CLI commands (`init`, `root`, `snapshot`, `timeline`, `restore`, `verify`, `status`), options, and C++ library APIs. |
| Observer & Reconciliation | [OBSERVER_AND_RECONCILIATION.md](OBSERVER_AND_RECONCILIATION.md) | Linux `inotify` notification backend, event queue coalescing, and periodic reconciliation scans. |
| Restore Protocol & Safety | [RESTORE_AND_SAFETY.md](RESTORE_AND_SAFETY.md) | Multi-step safe restore protocol, pre-restore hash verification, safety copies, and atomic replacement. |
| Testing & Verification | [TESTING.md](TESTING.md) | Test strategy, GoogleTest suites, crash-injection matrix, sanitizers (ASan, UBSan, TSan), and benchmarks. |
| Deployment & Operations | [DEPLOYMENT.md](DEPLOYMENT.md) | Build presets, toolchain requirements, system dependencies, Linux packaging, and systemd service setup. |
| UML & Sequence Diagrams | [UML.md](UML.md) | Architecture diagrams, component relationships, commit protocol sequence, and ERD visual models. |

---

## Quick Reference

### Core Invariants
1. A committed snapshot references only fully written and verified objects.
2. Published objects are immutable and content-addressed.
3. Restore verifies temporary content before replacing a destination.
4. Existing destination content is preserved in a safety copy before replacement.
5. Garbage collection never deletes an object referenced by a committed snapshot.
6. Event loss triggers reconciliation or an explicit incomplete-history warning.
7. The local core works completely offline without accounts or cloud services.

### Build and Test Summary
```bash
# Configure debug build with CMake presets
cmake --preset debug

# Build all targets
cmake --build --preset debug

# Run test suite
ctest --preset debug --output-on-failure
```

> Note: Detailed guides are actively updated across project implementation phases.
