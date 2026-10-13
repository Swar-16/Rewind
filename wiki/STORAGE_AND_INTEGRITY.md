# Storage Engine & Data Integrity

> **Status:** Outline / Planned  
> **Hash Algorithm:** BLAKE3 / SHA-256  
> **Source Directory:** `src/storage/`, `include/rewind/storage/`

## 1. Storage Layout & Object Model

Rewind persists file content as immutable, content-addressed objects inside a local directory tree:
```text
.rewind/
├── objects/
│   ├── ab/
│   │   └── cd1234... (immutable object file)
│   └── ef/
│       └── 567890...
├── tmp/          (temporary write area for uncommitted objects)
├── safety/       (pre-restore safety copies of modified files)
└── metadata.db   (SQLite database in WAL mode)
```

## 2. Object Commit Protocol

1. Stream incoming file bytes to a uniquely named temporary file in `tmp/`.
2. Compute the cryptographic hash (BLAKE3 or SHA-256) and record byte counts on the fly.
3. Flush and synchronize the temporary file to disk (`fdatasync` / `fsync`).
4. Atomically publish the file into its sharded location in `objects/ab/` using POSIX atomic rename (`renameat2` with `RENAME_NOREPLACE` where available).
5. If the target object already exists, confirm content identity and delete the temporary file without error.

## 3. Integrity Verification & Corruption Detection

- Read verification: Re-hash objects on restore or explicit verification passes to catch bit-rot or tampering.
- Background sweeps: Periodic verification audits that cross-reference all objects against SQLite snapshot manifests.
- Crash recovery: Orphaned files in `tmp/` are detected and pruned during system startup.

---
*Implementation specifics, format headers, and benchmark figures will be documented in Phase 2.*
