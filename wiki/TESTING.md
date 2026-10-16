# Testing Strategy & Verification

> **Status:** Active Baseline (Step 1 Verified)  
> **Framework:** GoogleTest, CTest, Clang Sanitizers  
> **Source Directory:** `tests/`

## 1. Testing Philosophy

Rewind operates on user data and file persistence. Testing rigorously exercises failure paths, crash recovery boundaries, and edge cases, not just happy-path flows. All tests compile cleanly under `-Wall -Wextra -Wpedantic -Werror`.

## 2. Test Suite Categories

- **Unit Tests (`tests/unit/`):**
  - `tests/unit/test_common.cpp` (Step 1): 15 unit tests validating semantic version format, `ErrorCode` mapping, POSIX errno conversions, `RewindError` formatting, move semantics, and monadic `Result<T>` / `Result<void>` chaining.
- **Integration Tests (`tests/integration/`):** Full end-to-end flows: root configuration, snapshot capture, modification, timeline queries, and safe restore (scheduled for subsequent steps).
- **Crash & Fault Injection Tests (`tests/crash/`):** Simulated process termination at persistence boundaries:
  - Crash during temporary object write.
  - Crash after object publication but before SQLite commit.
  - Crash during SQLite manifest transaction.
  - Crash during multi-step restore replacement.
- **Security Tests (`tests/security/`):** Symlink loop handling, path traversal attacks (`../`), permissions violations, and unprivileged user isolation.
- **Benchmarks (`tools/benchmark_runner/`):** Performance metrics for snapshot capture latency, storage deduplication ratio, memory overhead, and restore throughput.

## 3. Presets & Sanitizer Verification Matrix

All presets are verified on Linux (WSL2 Ubuntu 24.04, GCC 13.3.0):

| Preset | Purpose | Status | Execution Command |
|---|---|---|---|
| **`debug`** | Fast development build with full debug symbols and warnings | ✅ 16/16 Passed | `cmake --preset debug && cmake --build --preset debug && ctest --preset debug` |
| **`asan`** | AddressSanitizer + UndefinedBehaviorSanitizer (leak & bounds detection) | ✅ 16/16 Passed | `cmake --preset asan && cmake --build --preset asan && ctest --preset asan` |
| **`tsan`** | ThreadSanitizer (data-race detection across concurrent paths) | ✅ 16/16 Passed | `setarch x86_64 -R cmake --build --preset tsan && setarch x86_64 -R ctest --preset tsan` |
| **`release`** | Optimized release binary verification (`-O3`) | ✅ 16/16 Passed | `cmake --preset release && cmake --build --preset release && ctest --preset release` |

---
*Crash test fixtures, fault injection runners, and performance benchmarks will be progressively integrated.*
