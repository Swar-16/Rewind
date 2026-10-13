# Rewind — Product Requirements Document (PRD)

> **Purpose:** Define what Rewind must do, who it serves, what it will not promise, and how each phase will be accepted.
>
> **Status:** Initial product baseline · **Version:** 0.1 · **Date:** 2026-10-13  
> **Initial platform:** Linux desktop · **Implementation language:** C++20  
> **Working name:** Rewind (name/trademark availability not verified)

## 1. Executive summary

Rewind is a lightweight, local-first file recovery system for developers, students, researchers, writers, and creators. It keeps automatic history for user-selected folders, stores file content as immutable objects, records recovery metadata in SQLite, and supports safe file-level restore.

The first release focuses on one narrow promise: **when Rewind has recorded a file version, a user can find and restore it without damaging the last valid recovery point.**

Rewind is not a full operating-system restore tool, a replacement for Git, or a guarantee against hardware failure. Its local-first core must work without an account or network connection. Optional replication to a second device is a later phase.

## 2. Product vision and story

At 11:47 p.m., a person runs a cleanup command, overwrites a project file, or deletes a folder by mistake. They do not want a lecture about backup strategy. They want the version from ten minutes ago.

Rewind aims to make that recovery simple. It records history for folders that users choose, shows what changed, lets users inspect prior versions, and restores selected content safely.

**North-star statement:** People should not need to predict every mistake before they can recover from one.

The promise has limits. A local copy cannot protect against every disk failure, theft, or attacker. Rewind must show when history is incomplete and must never claim to have captured a version that it did not capture.

## 3. Problem statement

Current recovery workflows can require users to configure backup destinations, create manual commits, search archives, or understand system-level restore tools. The existence of a backup does not always mean a person can quickly find and safely restore one useful file version.

Core user questions:
1. What changed?
2. When did it change?
3. Can I inspect the earlier content?
4. Can I restore it without overwriting newer work?

## 4. Target users

| Segment | Job to be done | Initial importance |
|---|---|---|
| Technical students | Recover assignments, source code, and project files | Primary |
| Developers | Recover uncommitted work, local configuration, and generated files worth keeping | Primary |
| Researchers | Recover notebooks, datasets, and experiment files | Secondary |
| Writers and creators | Recover earlier drafts and project assets | Secondary |
| Small teams | Inspect and recover local workspace changes without operating a large backup service | Later validation |

The first design partner group should be technical students and developers. Do not assume that all users want command-line tools.

## 5. Competitive research and positioning

Existing tools already offer parts of this capability. The product must not claim that snapshots, deduplication, encryption, or restore are new.

| Existing solution | Strengths | Product hypothesis to validate |
|---|---|---|
| Apple Time Machine | Integrated backup and local snapshots | Can Rewind offer a more direct workspace timeline for its target users? |
| Windows File History | Earlier file versions and configurable backup | Is setup and single-file recovery more understandable in Rewind's proposed flow? |
| Windows recovery / System Restore | Supported system recovery paths | Can Rewind make file-level workspace recovery easier to inspect? |
| Git | History, diffs, branching, collaboration | Is automatic history useful for files and work that were not committed or tracked? |
| BorgBackup | Mature deduplication, compression, encryption, archive tooling | Do some users prefer a simpler automatic recovery experience over a configurable backup workflow? |
| restic | Encrypted, deduplicated snapshots and multiple storage backends | Is a focused workspace timeline a useful complement to a general backup tool? |

References:
- Apple local snapshots: https://support.apple.com/en-in/102154
- Apple restore guide: https://support.apple.com/en-us/guide/mac-help/mh11422/mac
- Microsoft File History: https://support.microsoft.com/en-us/windows/backup-and-restore-with-file-history
- Microsoft recovery options: https://support.microsoft.com/en-us/windows/recovery-options-in-windows
- BorgBackup: https://borgbackup.readthedocs.io/
- restic: https://restic.readthedocs.io/

These sources describe product capabilities; they do not prove user dissatisfaction or a unique market gap.

### Research validation plan
1. Directly test each available competitor.
2. Time setup, find-version, preview, and restore tasks.
3. Test delete, overwrite, rename, large changes, and low-storage conditions where safe.
4. Interview 10–15 users and observe at least five recovery tasks.
5. Record the task, steps, time, failure points, and user feedback.
6. Update the positioning from evidence; do not publish unsupported claims.

## 6. Goals and non-goals

### MVP goals
- Monitor one or more user-selected directories on Linux.
- Record regular-file changes and reconcile missed events.
- Store immutable content objects addressed by a cryptographic hash.
- Store snapshot metadata transactionally in SQLite.
- Provide a timeline and file-version lookup.
- Restore a selected version with a verified temporary write and safety copy.
- Survive process crashes without invalidating prior committed snapshots.
- Report incomplete history, errors, storage use, and last successful scan.
- Work offline without an account or required cloud service.
- Include automated tests, a threat model, and repeatable benchmarks.

### Non-goals
- Full operating-system rollback or bootable recovery media.
- Replacing Git or becoming a collaboration platform.
- Required cloud storage or user accounts.
- Guaranteed recovery from a failed disk or compromised host.
- Supporting all operating systems in the first release.
- Custom cryptography or a custom database engine.
- A guarantee of zero data loss in every possible environment.

## 7. User stories and acceptance criteria

### US-01: Restore an earlier file version
**As a user,** I can find a prior file version and restore it safely.

Acceptance:
- History shows recorded versions with timestamps and status.
- Text files can be previewed within defined size limits.
- Restore writes to a temporary file and verifies its hash before replacement.
- Existing destination content is preserved before replacement.
- Rewind never overwrites an existing file without clear user consent.
- A failure before replacement leaves the destination unchanged.
- The result is recorded and shown to the user.

### US-02: Recover a deleted file
Acceptance:
- The timeline shows the last recorded version even if the live path is absent.
- The user can select an alternate restore destination.
- An existing destination triggers a clear conflict flow.
- The operation result is visible in history.

### US-03: Understand what changed
Acceptance:
- Timeline supports added, modified, deleted, and rename events when the evidence is sufficient.
- Events show path, time, and source (`notification`, `scan`, or user action).
- Scan-inferred changes are not falsely presented as exact event history.
- Users can filter by path, date, and event type.
- Any known gap in history is visible.

### US-04: Trust a recovery point after a crash
Acceptance:
- A committed snapshot never references a partial object.
- A crash during commit leaves the old committed state or a new complete state.
- Startup detects abandoned temporary data.
- Integrity checks report missing or corrupt objects.
- Controlled crash-injection tests cover commit boundaries.

### US-05: Understand storage use
Acceptance:
- Logical data size and physical store size are shown separately.
- Retention rules are visible.
- Garbage collection never deletes content referenced by a committed snapshot.
- Low disk space and failed writes create a visible warning.

### US-06: Keep data local
Acceptance:
- Core operation requires no network.
- No account is required.
- Network replication is disabled until explicitly configured.
- Logs do not contain file content by default.
- Documentation explains which file metadata is stored.

## 8. Functional requirements

### P0 — MVP
1. Protected-root configuration and validation.
2. Linux file notification backend behind a platform interface.
3. Bounded event queue and event coalescing.
4. Periodic reconciliation scan.
5. Safe directory traversal and explicit symlink policy.
6. Immutable content-addressed object store.
7. SQLite metadata schema and migrations.
8. Snapshot state machine and commit protocol.
9. Snapshot listing, manifest inspection, and integrity verification.
10. Timeline for recorded versions and known change events.
11. Restore plan, alternate destination, safety copy, temporary write, hash verification, safe replacement.
12. Visible status, errors, last scan, last committed snapshot, and storage use.
13. Unit, integration, crash, security, and benchmark tests.

### P1 — Beta
- Text diff preview and binary file details.
- Search and timeline filters.
- Directory restore with a preview plan.
- Age/count retention rules.
- Dry-run garbage collection.
- Resource limits and user-visible exclusions.
- Simple desktop or local UI after CLI usability tests.
- Configuration export/import and documented recovery procedures.

### P2 — Advanced
- Optional encrypted device-to-device replication.
- Resumable transfer and integrity verification.
- Key rotation and device revocation.
- Content-defined chunking if benchmarks justify it.
- Additional OS backends with platform-specific tests.
- Portable export/import format and packaging.

## 9. UX requirements

The first screen must answer: **“Is my work protected, and can I get it back?”**

Show:
- Protected folders and monitoring status.
- Last successful scan and last complete recovery point.
- Known history gaps and errors.
- File history and restore entry point.
- Logical data size, physical storage use, and retention state.

Restore flow:
1. Choose a file or recovery point.
2. Select the recorded version.
3. Preview text or inspect binary file details.
4. Select destination.
5. Review overwrite warnings.
6. Confirm restore.
7. Verify content.
8. Report success or explain a safe next step.
9. Offer undo when a previous destination version is available.

Use plain language. Do not show “protected” before the first complete snapshot. Do not hide background failures. Keep keyboard navigation and readable contrast in scope.

## 10. Architecture and data flow

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

Later: optional replication sends verified objects to a trusted peer.
```

Core modules:
- `observer`: platform events and watch lifecycle.
- `filesystem`: safe traversal, file identity, metadata.
- `snapshot`: scan result to committed recovery point.
- `storage`: immutable objects and integrity checks.
- `metadata`: SQLite schema, migrations, and repositories.
- `restore`: plan, conflict check, safety copy, verification, replacement.
- `retention`: policy and safe garbage collection.
- `timeline`: history queries and change classification.
- `app`: use cases and coordination.
- `cli`: first user interface.
- `platform`: OS-specific operations.

### Invariants
1. A committed snapshot references only fully written and verified objects.
2. Published objects are immutable.
3. Restore verifies temporary content before replacing a destination.
4. Existing destination content is preserved when replacement is approved.
5. Garbage collection never deletes an object referenced by a committed snapshot.
6. Event loss triggers reconciliation or an incomplete-history warning.
7. The local core remains usable when networking is disabled.
8. Peer failure does not invalidate a local committed snapshot.
9. Metadata and object changes follow a documented commit order.
10. Success has a testable definition.

## 11. Data model

Initial SQLite entities:
- `protected_root`: root ID, path, enabled flag, policy, last scan state.
- `snapshot`: ID, root ID, start/end time, status, manifest hash, file count, logical bytes, error.
- `file_entry`: snapshot ID, relative path, type, size, metadata, object reference.
- `object`: content hash, size, stored size, format version, verification status.
- `event`: path, event kind, time, source, confidence/status, snapshot reference.
- `restore_operation`: source snapshot/path, destination, times, status, safety-copy reference.

Use relative paths inside manifests. Do not treat modification time or file size alone as proof that content is unchanged. Define support for hard links, symbolic links, sparse files, permissions, extended attributes, and special files before claiming support.

## 12. Repository structure

```text
rewind/
├── README.md
├── PRD.md
├── LICENSE
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── SECURITY.md
├── CHANGELOG.md
├── CMakeLists.txt
├── CMakePresets.json
├── .clang-format
├── .clang-tidy
├── cmake/
├── docs/
│   ├── architecture/
│   │   ├── overview.md
│   │   ├── invariants.md
│   │   ├── snapshot-commit-protocol.md
│   │   ├── restore-protocol.md
│   │   ├── retention-and-gc.md
│   │   └── decisions/
│   ├── research/
│   │   ├── competitive-research.md
│   │   └── competitor-test-results.md
│   ├── security/
│   │   ├── threat-model.md
│   │   ├── privacy.md
│   │   └── recovery-limits.md
│   ├── testing/
│   │   ├── test-strategy.md
│   │   ├── crash-matrix.md
│   │   └── benchmark-method.md
│   └── ux/
│       ├── user-stories.md
│       └── flows.md
├── include/rewind/
│   ├── app/  
│   ├── config/  
│   ├── domain/
│   ├── filesystem/  
│   ├── observer/  
│   ├── snapshot/
│   ├── storage/
│   ├── metadata/
│   ├── restore/
│   ├── retention/
│   ├── timeline/
│   └── platform/
├── src/
│   ├── app/
│   ├── config/
│   ├── domain/
│   ├── filesystem/
│   ├── observer/linux/
│   ├── snapshot/
│   ├── storage/
│   ├── metadata/
│   ├── restore/
│   ├── retention/
│   ├── timeline/
│   ├── platform/linux/
│   ├── cli/
│   └── main.cpp
├── migrations/sqlite/
├── tests/
│   ├── unit/
│   ├── integration/
│   ├── crash/
│   ├── security/
│   ├── fixtures/
│   └── support/
├── tools/
│   ├── workload_generator/
│   ├── benchmark_runner/
│   ├── crash_injector/
│   └── store_inspector/
├── examples/
├── packaging/linux/
└── .github/workflows/
```

Create directories when they have a real first file; do not add empty structure for appearance. Keep the first version a modular monolith. Keep the core independent of the UI. Add replication and other platform backends only after local recovery is reliable.

## 13. Phase-wise implementation plan

### Phase 0 — Validate the problem
Work: interview 10–15 users; observe five recovery tasks; test competitor workflows; prototype timeline and restore flow; define representative benchmark workloads.  
Deliverables: interview notes, tested competitor matrix, top user jobs, UX flow, revised metrics.  
Exit: at least five users describe a recent consequential recovery problem; prototype testing reveals actionable issues; the proposed workflow has a clear evidence-based reason to exist.

### Phase 1 — Build foundation
Work: CMake, C++20 baseline, warnings, format/lint, tests, CI, sanitizers, error model, configuration, first ADRs.  
Deliverables: reproducible clean build, minimal CLI, contribution guide, CI.  
Exit: fresh checkout builds and tests; CI catches failure; default logs contain no file content.

### Phase 2 — Implement object store
Work: trusted hash library, object format, temporary write, durability policy, verification, atomic publication where supported, duplicate handling, locking, disk-full and corruption tests.  
Deliverables: immutable store, inspector, object-format documentation.  
Exit: repeated bytes reuse identity; partial writes are never published as valid; corruption is detected.

### Phase 3 — Metadata and snapshot commit
Work: SQLite schema/migrations; snapshot state machine; manifest; verified-object precondition; transactional commit; startup cleanup; crash injection.  
Deliverables: metadata repository, coordinator, commit protocol, crash matrix.  
Exit: each snapshot is committed and usable or incomplete and clearly marked; no committed snapshot points to partial content.

### Phase 4 — Safe scanning and manual snapshots
Work: safe directory traversal; handle permissions and changing files; hash and store content; manifest creation; CLI snapshot/list/verify; fixture comparison.  
Deliverables: explicit snapshot command, scanner, verification.  
Exit: supported test trees restore equivalently; scan errors are visible; no write escapes allowed paths.

### Phase 5 — Restore safety
Work: restore planning; conflict handling; alternate destination; safety copy; temporary write and hash verification; atomic replacement; audit record; traversal and symlink tests.  
Deliverables: restore executor and documented safety model.  
Exit: failed pre-replacement steps leave destination unchanged; no silent overwrite; restored bytes match expected hash.

### Phase 6 — Automatic observation
Work: Linux notification backend; event translation and coalescing; overflow detection; watch restart; periodic reconciliation; quiet-period scheduling; bounded queues; rapid-change tests.  
Deliverables: observer, reconciliation scanner, history-completeness status.  
Exit: lost events are detected through reconciliation; history is not reported as current without a successful scan; idle resource use is measured.

### Phase 7 — Timeline and usable interface
Work: CLI status/root/history/diff/restore/verify; timeline; path/date filters; bounded text preview; binary metadata; error guidance; usability tests; choose desktop toolkit only after testing.  
Deliverables: usable CLI, timeline flow, demo.  
Exit: a new user can protect a folder, find a version, and restore without developer help.

### Phase 8 — Retention and efficiency
Work: age/count retention; dry-run cleanup; reachability-based garbage collection; crash-safe cleanup; integrity sweeps; benchmark whole-file objects, chunking, and compression; visible exclusions.  
Deliverables: retention engine, cleanup plan, benchmark report.  
Exit: GC never deletes referenced objects in tests; crash recovery works; performance claims are reproducible.

### Phase 9 — Security and release hardening
Work: path review, permissions, fuzzing, sanitizers, corrupted-store tests, recovery limits, export guidance, versioned formats, signed artifacts where practical, beta feedback.  
Deliverables: security checklist, beta, known limits, release checklist.  
Exit: no known critical data-loss defect remains open; upgrades and limits are documented.

### Phase 10 — Optional device replication
Work: peer identity, standard crypto/transport, bounded versioned protocol, authentication, verified durable acknowledgements, idempotent retry, reconnect/backpressure, resumable transfer, key rotation, network fault tests.  
Deliverables: optional peer replication, pairing, status, security review.  
Exit: local history works offline; receiver acknowledges only verified stored objects; interrupted transfers resume safely; no data leaves before opt-in.

### Phase 11 — Platform expansion
Work: validate demand; abstract notifications and safe replacement; implement one OS at a time; platform-specific integration tests; package and document differences.  
Deliverables: supported-platform matrix, install/upgrade guide, compatibility policy.  
Exit: each platform passes its own restore, crash, and security tests; unsupported behavior is clearly stated.

## 14. Performance targets and measurement

Initial targets—not achieved results:
- Core idle memory target: under 50 MiB on a named reference Linux machine.
- Idle CPU target: near 0% average over a 10-minute idle test, excluding scheduled reconciliation.
- No successful restore may produce bytes that fail the expected hash check.
- Unchanged content should not be written again as a new object.
- Publish p50/p95/p99 latency, throughput, CPU time, peak memory, bytes read/written, and physical-to-logical storage ratio.

Each benchmark must record hardware, OS/kernel, file system, compiler, dependencies, workload seed, file count/size distribution, changed-file count, cache conditions, baseline settings, run count, and raw results.

## 15. Security and privacy

Threats include accidental deletion, process crash, corrupt objects, path traversal, symlink attacks, unprivileged local tampering within the host account model, and content leakage through logs. Do not claim full protection from root-level attackers, compromised accounts, disk failure with only one copy, theft of an unencrypted device, or files that vanish before capture.

Controls:
- Standard cryptographic hashes and maintained libraries.
- No custom encryption.
- Strict path validation and restrictive store permissions.
- Verify content on read and restore.
- Keep content out of logs by default.
- Define flush and atomic-replacement guarantees.
- Test disk-full, permission, and corruption failures.
- Network disabled by default.
- Before replication: peer authentication, encrypted transport, bounded messages, integrity checks, idempotency, key rotation, and recovery procedures.

## 16. Success metrics

User: time to verified restore, completion without help, restore errors, understanding of history status, user confidence.  
Reliability: invalid committed snapshots, integrity failures, crash-test failures, undetected event gaps, bad restores, unsafe garbage collection.  
Efficiency: idle/active memory and CPU, scan time, bytes read/written, store ratio, restore throughput.  
Trust: setup time, understanding of local-only behavior, export success, response to data-loss defects.

The target for known integrity failures in tested conditions is zero. This is not a proof that failures are impossible in every environment.

## 17. Key risks

| Risk | Response |
|---|---|
| Weak differentiation | Validate workflows and user demand before strong claims |
| Lost filesystem events | Reconciliation and explicit incomplete-history state |
| Wrong commit ordering | Written protocol and crash injection |
| Restore overwrites newer work | Preview, safety copy, temporary write, verification |
| Storage growth | Visible budgets, retention, safe GC |
| Complexity from chunking | Begin with whole-file objects; measure before adding chunks |
| Single-device failure | State local-store limit; add optional replication later |
| Security scope grows too early | Keep networking out of MVP |
| Cross-platform differences | Platform-specific implementations and tests |
| Unsupported performance claims | Reproducible benchmarks with disclosed conditions |

## 18. Portfolio plan

Show a real recovery task, then explain the system behind it:
1. A 60–90 second demo of a deletion or overwrite and verified restore.
2. Architecture and snapshot commit protocol.
3. ADRs explaining SQLite, immutable objects, and event-plus-scan design.
4. Crash-injection tests and results.
5. Reproducible benchmarks and raw data.
6. Threat model and recovery limits.
7. Competitor workflow study with hands-on evidence.
8. User-test summary with permission and personal data removed.

Resume bullet template (fill only with verified results):

> Built **Rewind**, a C++20 local-first file recovery system with immutable content storage, transactional snapshots, safe restore, and filesystem-event reconciliation; measured **[verified result]** on **[defined workload]**.

## 19. Definition of done

A feature is done when behavior and failure cases are documented; normal and error paths are tested; logs avoid file content; errors explain the next safe step; resource use is measured where relevant; security impact is reviewed; and the feature works from a clean build. A phase is done only when its exit conditions pass.

## 20. First implementation milestone

- Create repository and CMake project.
- Add minimal C++20 executable and `--version`.
- Add unit tests, format checks, strict warnings, and CI.
- Write `docs/architecture/invariants.md`.
- Add ADR for Linux-first scope.
- Test writing and reading known bytes.
- Test a trusted content hash.
- Add a CLI command to show the store path.

The first milestone is a small tested core—not a dashboard.

## 21. Writing and standards note

This PRD uses plain language, direct instructions, short sections, and consistent terms inspired by ASD-STE100. It is not certified or formally verified as compliant with the complete controlled-language standard.

## 22. References

- Apple Time Machine local snapshots: https://support.apple.com/en-in/102154
- Apple restore guide: https://support.apple.com/en-us/guide/mac-help/mh11422/mac
- Microsoft File History: https://support.microsoft.com/en-us/windows/backup-and-restore-with-file-history
- Microsoft recovery options: https://support.microsoft.com/en-us/windows/recovery-options-in-windows
- BorgBackup: https://borgbackup.readthedocs.io/
- restic: https://restic.readthedocs.io/
