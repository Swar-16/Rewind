# Restore Protocol & Safety Architecture

> **Status:** Outline / Planned  
> **Source Directory:** `src/restore/`, `include/rewind/restore/`

## 1. Safety Guarantees

Rewind enforces the fundamental invariant:
> **A restore must never silently destroy the only available copy of existing destination content.**

## 2. Restore Execution Protocol

```text
[Select Snapshot & File]
         │
         ▼
[Resolve Target Destination & Verify Path Boundaries]
         │
         ▼
[Write Candidate Content to Staging File]
         │
         ▼
[Verify SHA-256 / BLAKE3 Hash of Staging Content]
         │
         ▼
[Detect Destination Conflict]
 ├── If target exists and user approved replacement:
 │   └── Create timestamped Safety Copy in safety store (.rewind/safety/...)
 └── If target does not exist: proceed
         │
         ▼
[Atomic Replacement of Destination File]
         │
         ▼
[Log Restore Operation to SQLite Audit Table]
```

## 3. Conflict Resolution Modes

- **Fail / Abort:** Abort immediately if destination file already exists.
- **Alternate Destination:** Write restored content to a user-specified alternative folder.
- **Safe Overwrite:** Create a verified safety copy of the existing destination, then atomically replace.

## 4. Undo and Safety Copy Recovery

- Pre-restore copies are retained in `.rewind/safety/` with full provenance recorded in SQLite `restore_operation`.
- Users can roll back a restore operation via `rewind restore --undo <restore_op_id>`.

---
*Sequence flows, safety copy retention rules, and edge-case handling will be detailed in Phase 5.*
