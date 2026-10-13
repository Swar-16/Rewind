# Testing Strategy & Verification

> **Status:** Outline / Planned  
> **Framework:** GoogleTest, CTest, Clang Sanitizers  
> **Source Directory:** `tests/`

## 1. Testing Philosophy

Rewind operates on user data and file persistence. Testing must rigorously exercise failure paths, crash recovery boundaries, and edge cases, not just happy-path flows.

## 2. Test Suite Categories

- **Unit Tests (`tests/unit/`):** Isolated verification of components (hashing, path sanitization, coalescer debouncing, manifest serialization).
- **Integration Tests (`tests/integration/`):** Full end-to-end flows: root configuration, snapshot capture, modification, timeline queries, and safe restore.
- **Crash & Fault Injection Tests (`tests/crash/`):** Simulated process termination at persistence boundaries:
  - Crash during temporary object write.
  - Crash after object publication but before SQLite commit.
  - Crash during SQLite manifest transaction.
  - Crash during multi-step restore replacement.
- **Security Tests (`tests/security/`):** Symlink loop handling, path traversal attacks (`../`), permissions violations, and unprivileged user isolation.
- **Benchmarks (`tools/benchmark_runner/`):** Performance metrics for snapshot capture latency, storage deduplication ratio, memory overhead, and restore throughput.

## 3. Sanitizers & Analysis

- AddressSanitizer (ASan) + UndefinedBehaviorSanitizer (UBSan)
- ThreadSanitizer (TSan) for multi-threaded observer and worker verification
- Clang-Tidy static analysis and strict compiler warning baselines (`-Wall -Wextra -Wpedantic -Werror`)

---
*Test test execution commands, fixtures, and crash test results will be added as test suites are built.*
