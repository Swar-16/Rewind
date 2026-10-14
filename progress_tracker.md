# Rewind — Engineering Progress Tracker

> **Document Type:** Project Milestone and Verification Dashboard  
> **Target Platform:** Linux Desktop (x86_64, aarch64) · **Language Standard:** C++20  
> **Master Blueprint:** [docs/full_implementation_plan.md](docs/full_implementation_plan.md)  
> **Execution Guides:** [step_by_step_guide.md](docs/product/step_by_step_guide.md) · [step_by_step_person_1.md](docs/product/step_by_step_person_1.md) · [step_by_step_person_2.md](docs/product/step_by_step_person_2.md)

---

## 1. Master Step Progress Matrix

| Step | Milestone Name | Person 1 (Systems & Storage) | Person 2 (Metadata & Apps) | Step Merge Gate | Status |
|---|---|---|---|---|---|
| **Step 1** | Build Foundation & Toolchains | 🟢 Completed | 🟡 Pending Merge | 🟡 In Progress | 🟡 In Progress |
| **Step 2** | Core Storage & Database Setup | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 3** | Snapshot Coordinator & Metadata | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 4** | Filesystem Scanner & Manual CLI | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 5** | Verified Restore Safety Engine | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 6** | Observer, Coalescing & Scan | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 7** | Timeline Queries, Diffing & CLI | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 8** | Retention Policy & Safe GC | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 9** | Security Hardening & Benchmarks | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |
| **Step 10**| Packaging, Systemd & Release | ❌ Not Started | ❌ Not Started | ❌ Not Started | ❌ Not Started |

---

## 2. Step 1 Detailed Status & Deliverables

### Person 1 Deliverables (Branch: `foundation` / `feat/step-01-person1`)
- [x] Root `CMakeLists.txt` (C++20, modular includes, dynamic library target) — ✅ Passed
- [x] `CMakePresets.json` (Debug, Release, ASan, TSan presets) — ✅ Passed
- [x] `cmake/CompilerWarnings.cmake` (Strict `-Wall -Wextra -Wpedantic -Werror`) — ✅ Passed
- [x] `cmake/Sanitizers.cmake` (ASan, UBSan, TSan runtime checks) — ✅ Passed
- [x] `cmake/Dependencies.cmake` (GTest FetchContent fallback, SQLite3) — ✅ Passed
- [x] `.clang-format` (LLVM modern C++20 formatting) — ✅ Passed
- [x] `.clang-tidy` (Modern C++ linter checks) — ✅ Passed
- [x] `.github/workflows/ci.yml` (Ubuntu CI matrix: GCC 13, Clang 17) — ✅ Passed

### Person 2 Deliverables (Branch: `feat/step-01-person2`)
- [ ] `include/rewind/common/error.hpp` and `src/common/error.cpp`
- [ ] `include/rewind/common/result.hpp`
- [ ] `include/rewind/common/version.hpp`
- [ ] `src/main.cpp` (`--version`, `--help`)
- [ ] `tests/unit/test_common.cpp`

### Merge Gate Verification Protocol
1. Merge Person 1 and Person 2 branches into `dev`.
2. Execute validation commands:
   ```bash
   cmake --preset debug
   cmake --build --preset debug
   ctest --preset debug --output-on-failure
   ```
3. Verify clean compilation with `-Wall -Wextra -Wpedantic -Werror`.
4. Tag Step 1 completed upon verification pass.
