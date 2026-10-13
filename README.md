# Rewind --- Your Work Deserves an Undo Button

> **A small, local-first time machine for the files that matter.**
>
> Rewind quietly keeps a history of selected folders. When a mistake
> removes, replaces, or damages your work, you can inspect what changed
> and restore the version you need. No manual commits. No cloud account
> required. No need to remember that a backup should have been running.

**Project status:** Product and engineering plan\
**Initial target:** Linux desktop, C++20\
**Product category:** Local-first file recovery and workspace history\
**Working name:** Rewind --- name and trademark availability are not yet
verified.

------------------------------------------------------------------------

## The story

At 11:47 p.m., someone runs a command that should clean a temporary
folder.

The command has a bad path.

A semester project disappears. A photographer loses a folder of edited
images. A researcher overwrites a data file. A developer finds that a
script has replaced a useful version of a configuration file with an
empty one.

The first reaction is familiar: *I only need the version from ten
minutes ago.*

But the file is gone. The person did not make a Git commit. The backup
drive may not be connected. The cloud service may not have synced the
right version. A recovery tool may exist, but the person may not know
how to use it.

The loss is not only the file. It is the time, care, and thought that
went into it.

Rewind starts with a simple belief:

**People should not need to plan for every mistake before they can
recover from one.**

Rewind watches only the folders that a person chooses. It keeps earlier
file content in a local recovery store. It shows what changed. It lets
the person preview a version, restore selected files, and undo a restore
when needed. It aims to use storage well, explain its status in plain
language, and stay quiet when there is no work to do.

It is not a promise that every computer failure can be reversed. A local
copy cannot save data from every disk failure, theft, or attack. Rewind
will state those limits clearly. Later, a person can choose to send
encrypted recovery data to another device.

The goal is not to make people think about backups more often.

**The goal is to let people get back to what they were doing.**

------------------------------------------------------------------------

## 1. Product summary

Rewind is a lightweight, local-first file recovery system for developers
and other people who work with valuable files.

It creates a history of selected folders without requiring users to make
commits or start manual backup jobs. It stores file content as immutable
objects, records snapshot metadata in a transactional database, and
offers safe file-level restore.

The first version will focus on a narrow, testable promise:

> If Rewind has recorded a file version, the user can find it and
> restore it without damaging the last valid recovery point.

The first version will not claim to restore a whole operating system,
recover files that Rewind never observed, or protect against every form
of malware or hardware failure.

### Product principles

1.  **Recovery before cleverness.** The first job is to recover the
    right bytes.
2.  **Local first.** The core product works without an account, network
    access, or cloud service.
3.  **Automatic, but visible.** Users should not need to start each
    snapshot. They must be able to see what Rewind has and has not
    recorded.
4.  **Small by design.** Avoid a large runtime, a required server, and
    needless background work.
5.  **Safe by default.** Preview changes before restore. Never silently
    overwrite a newer file.
6.  **Honest status.** Show missed events, storage errors, and
    incomplete history. Do not label an incomplete snapshot as safe.
7.  **Open to inspection.** Document the data format, threat model,
    limits, and benchmark methods.
8.  **Measure before claiming.** Publish results from repeatable tests.
    Do not claim "fast," "tiny," or "best" without evidence.

### Product position

Rewind is not a replacement for every backup or version-control tool.

-   **Git** is for intentional source history, collaboration, branches,
    and review.
-   **Backup tools** protect data by making copies and keeping recovery
    points.
-   **System restore tools** can return supported parts of an operating
    system to an earlier state.
-   **Rewind** aims to make automatic, file-level recovery history easy
    to inspect and use for selected work folders.

A person can use Rewind with Git and with a separate backup tool. The
product must explain that these tools solve related but different
problems.

------------------------------------------------------------------------

## 2. The problem

People create valuable work in folders, but many tools require a person
to plan recovery in advance.

Common failure cases include:

-   A command deletes files from the wrong directory.
-   A script truncates or replaces a file.
-   A tool saves over a useful version.
-   A person deletes a file and empties the trash.
-   A project changes in many places, and the person does not know which
    files changed.
-   A developer has uncommitted work that Git cannot restore after
    deletion.
-   A backup exists, but finding one file in it takes too many steps.
-   A recovery action overwrites newer work or creates a second problem.
-   A disk fills up and the backup system stops recording new data.
-   A computer loses power while a recovery point is being written.

The product must help answer four questions:

1.  **What changed?**
2.  **When did it change?**
3.  **Can I see the earlier content?**
4.  **Can I restore it without making the situation worse?**

### Who benefits?

  -----------------------------------------------------------------------
  User group              Example need            Why it matters
  ----------------------- ----------------------- -----------------------
  Students                Recover a deleted       A mistake should not
                          assignment or project   erase hours of work

  Developers              Recover uncommitted     Git does not cover
                          source code or a broken every file or every
                          config                  local state

  Researchers             Recover an earlier      Small file changes can
                          dataset, notebook, or   affect a result
                          experiment file         

  Writers                 Recover a draft before  Earlier wording may be
                          an unwanted edit        hard to recreate

  Designers and creators  Recover a project file  A source file may not
                          or asset                be easy to rebuild

  Small teams             Understand a local      Simple recovery should
                          change without          not require a
                          deploying a full backup specialist
                          service                 
  -----------------------------------------------------------------------

The first target user is a developer or technical student who works in a
selected directory. This user group can test the product with real files
and can also help validate the engineering design. Rewind should not
assume that every later user is technical.

------------------------------------------------------------------------

## 3. Research findings and product gap

The research shows that many important features already exist. Rewind
must not claim that automatic snapshots, deduplication, encryption, or
file restore are new ideas.

### Existing solutions

  -----------------------------------------------------------------------
  Product or approach     Strong points           Friction or limit
                                                  relevant to Rewind
  ----------------------- ----------------------- -----------------------
  Apple Time Machine      Built-in backup and     It is part of the Apple
                          local snapshots; users  ecosystem. Rewind must
                          can restore earlier     not claim it is
                          files                   generally worse; the
                                                  opportunity is a
                                                  different,
                                                  cross-platform and
                                                  developer-workspace
                                                  workflow

  Windows File History    Can keep earlier        The user must configure
                          versions of files in    a backup destination
                          selected libraries and  and understand the
                          folders                 Windows workflow.
                                                  Rewind can test whether
                                                  a more direct workspace
                                                  timeline is easier

  Windows System Restore  Can restore supported   System-level recovery
  / point-in-time         system state in defined is not the same as
  recovery                recovery scenarios      browsing and restoring
                                                  one file from a
                                                  developer workspace

  Git                     Excellent history,      History depends on
                          branching, diff, and    repository use and
                          collaboration for       tracked content. It is
                          tracked files           not a transparent
                                                  backup for every file
                                                  in a workspace

  BorgBackup              Mature backup tool with Powerful features can
                          deduplication,          require users to
                          compression, and        understand backup
                          encryption options      concepts and commands.
                                                  Rewind can focus on a
                                                  simple, interactive
                                                  file-recovery flow

  restic                  Encrypted, deduplicated It is a backup tool
                          snapshots and support   with its own setup and
                          for several storage     operating model. Rewind
                          backends                can test a narrower
                                                  automatic history and
                                                  restore experience
  -----------------------------------------------------------------------

### Source links

-   Apple, **About Time Machine local snapshots**:
    <https://support.apple.com/en-in/102154>
-   Apple, **Restore items backed up with Time Machine**:
    <https://support.apple.com/en-us/guide/mac-help/mh11422/mac>
-   Microsoft Support, **Back up and restore with File History**:
    <https://support.microsoft.com/en-us/windows/backup-and-restore-with-file-history>
-   Microsoft Support, **Recovery options in Windows**:
    <https://support.microsoft.com/en-us/windows/recovery-options-in-windows>
-   BorgBackup documentation: <https://borgbackup.readthedocs.io/>
-   restic documentation: <https://restic.readthedocs.io/>

Links and features can change. Verify the current vendor documentation
before making a public comparison.

### What the research does and does not prove

The sources establish that existing products support parts of the
problem. They do **not** prove that users dislike those products, that
their workflows fail, or that Rewind has a unique market position.

The following are product hypotheses. We must test them with users and
direct product trials:

-   Users want a timeline for a chosen workspace, not only a backup
    archive.
-   Users value a clear view of changed, added, renamed, and deleted
    files.
-   Users want to preview a restore and undo that restore.
-   Users want automatic history without manual commits or a cloud
    account.
-   Users value a small, measurable background footprint.
-   Users want the product to say when history is incomplete.
-   Users want to move recovery data to a second device without making
    cloud storage mandatory.

### Research plan before strong claims

1.  Install or test each available competitor on a supported system.
2.  Record the exact steps to enable protection, find a prior version,
    preview it, and restore it.
3.  Test deletion, overwrite, rename, a large directory change, and a
    full destination disk where safe.
4.  Measure setup time, restore time, storage use, CPU use, and memory
    use on the same test workload.
5.  Interview at least 10 people across developers, students, writers,
    and creators.
6.  Ask users to complete recovery tasks. Do not ask only whether they
    "like the idea."
7.  Record failed tasks and user quotes with permission.
8.  Update the product gap table with observed evidence.
9.  Keep claims that have no evidence out of the landing page and
    README.

**Positioning rule:** Rewind will compete on a focused workflow and
measured behavior, not on claims that other products cannot do basic
backup tasks.

------------------------------------------------------------------------

## 4. Product goals and non-goals

### Goals for the first public prototype

-   Watch a user-selected directory on Linux.
-   Record changes to regular files.
-   Keep file content in an immutable local object store.
-   Store snapshot and object metadata in a transactional database.
-   Show a timeline of recorded changes.
-   Let a user inspect and restore one earlier file version.
-   Preserve the current file before a restore that may replace it.
-   Recover safely after a process crash.
-   Detect missed filesystem events through periodic reconciliation.
-   Avoid storing the same content repeatedly when its hash is
    unchanged.
-   Report storage use, history status, and errors.
-   Run without a cloud account or network connection.
-   Include automated tests, a threat model, and reproducible
    benchmarks.

### Goals for later releases

-   Restore a selected group of files from a snapshot.
-   Search history by path, date, file type, and change kind.
-   Use configurable retention rules and a safe garbage collector.
-   Offer a simple desktop interface and command-line interface.
-   Support encrypted replication to a second trusted device.
-   Add platform-specific file monitoring for other operating systems.
-   Offer a portable export and recovery procedure.
-   Improve accessibility and localization based on user tests.

### Non-goals for the first prototype

-   Full operating-system rollback.
-   A bootable recovery image.
-   A cloud account or required hosted service.
-   A collaborative team version-control system.
-   A replacement for Git.
-   A general file-sync product.
-   Guaranteed recovery from a disk that has failed.
-   Guaranteed protection from an attacker who controls the same user
    account or root access.
-   Support for every file system and operating system on day one.
-   Custom cryptographic algorithms.
-   A custom database engine built only to make the project look more
    complex.
-   A claim of zero data loss before that claim has been proved under a
    defined test model.

------------------------------------------------------------------------

## 5. User stories and acceptance conditions

### Story A --- Recover a file

**As a user,** I want to see earlier versions of a file so that I can
recover a useful version after an accidental change.

Acceptance conditions:

-   The user can select a file and see its recorded versions.
-   Each version shows a time and a clear status.
-   The user can preview text files before restore.
-   A binary file can be restored without text preview.
-   Rewind writes the selected version to a temporary file first.
-   Rewind verifies the restored content before it replaces the
    destination.
-   Rewind saves the current destination as a recovery point before
    replacement.
-   If any step fails, the original destination remains unchanged.
-   The interface reports the result and the path that was changed.

### Story B --- Recover a deleted file

**As a user,** I want to find a deleted file in history and restore it
to a safe location.

Acceptance conditions:

-   The history shows that the file was deleted or is now missing.
-   The user can choose a restore path.
-   Rewind does not silently overwrite an existing file.
-   The user can compare the proposed destination with the original
    path.
-   The result appears in the event log.

### Story C --- Understand a project change

**As a developer,** I want to see which files changed during a time
range.

Acceptance conditions:

-   The timeline lists added, modified, deleted, and renamed events when
    the system can determine them.
-   Events include time, path, and event confidence or source where
    needed.
-   The UI does not report an event as confirmed if Rewind only inferred
    it during a scan.
-   A filter can reduce the list by path and event type.
-   The timeline makes gaps in recorded history visible.

### Story D --- Trust recovery after a crash

**As a user,** I want a process crash to leave the last completed
recovery point usable.

Acceptance conditions:

-   A crash during object write does not make a committed snapshot point
    to incomplete content.
-   A crash during snapshot commit leaves either the prior committed
    state or the new complete state.
-   Startup checks for incomplete temporary objects and uncommitted
    work.
-   Rewind reports any integrity problem.
-   Automated tests kill the process at controlled write and commit
    points.

### Story E --- Know the storage cost

**As a user,** I want to know how much space history uses.

Acceptance conditions:

-   The UI shows logical file size and physical store size as separate
    values.
-   The UI explains that compression and deduplication affect physical
    size.
-   Retention rules are visible.
-   Rewind never deletes an object that a committed snapshot still
    needs.
-   When the disk is near full, Rewind reports the condition and follows
    a documented policy.

### Story F --- Use Rewind without sending data away

**As a privacy-conscious user,** I want local recovery to work without a
network connection.

Acceptance conditions:

-   The core process makes no network request.
-   The product has no required account.
-   Network replication is disabled until the user configures it.
-   Logs do not include file content.
-   The documentation states what metadata is stored locally.

------------------------------------------------------------------------

## 6. Product requirements

### P0 --- Required for the first working prototype

1.  **Folder selection**
    -   The user can add and remove protected roots.
    -   Rewind rejects unsafe or unsupported roots with a clear error.
    -   Rewind does not follow symbolic links outside the protected root
        by default.
    -   The configuration records the root path and policy.
2.  **File observation**
    -   Use Linux filesystem notification support for prompt updates.
    -   Treat notifications as hints, not as a complete source of truth.
    -   Coalesce repeated events for the same path.
    -   Periodically scan the protected root to detect missed events.
    -   Detect file changes while avoiding reads of unchanged content
        where possible.
    -   Start with regular files. Define explicit behavior for
        directories, symlinks, sockets, device files, and special files.
3.  **Content store**
    -   Store file content as immutable objects.
    -   Identify an object by a cryptographic content hash.
    -   Write new objects to a temporary path.
    -   Verify the object before making it available to a snapshot.
    -   Atomically publish a completed object where the file system
        supports the required operation.
    -   Never change the content of an object after publication.
4.  **Metadata**
    -   Use SQLite for the initial metadata database.
    -   Keep snapshots, file entries, object references, scan status,
        and event records in transactions.
    -   Add schema migrations from the start.
    -   Use foreign keys and integrity checks.
    -   Do not put full file content in the metadata database.
5.  **Snapshot commit**
    -   A snapshot has a unique ID, start time, completion time, root
        ID, and status.
    -   A snapshot is visible as complete only after all referenced
        objects are durable and verified according to the documented
        durability policy.
    -   An incomplete snapshot must not look like a valid recovery
        point.
    -   Snapshot creation must be idempotent or safely resumable where
        practical.
6.  **Timeline**
    -   Show completed recovery points.
    -   Show changed paths and the known event type.
    -   Show incomplete or failed scan status.
    -   Distinguish event time from the time a reconciliation scan found
        a change.
7.  **Restore**
    -   Restore to a user-selected path.
    -   Create a safety copy of the destination if it already exists.
    -   Write to a temporary file, verify it, then replace the
        destination.
    -   Check path traversal and symlink conditions.
    -   Do not overwrite without clear user consent.
    -   Record restore actions and failures.
8.  **Status**
    -   Show whether monitoring is active.
    -   Show the last successful scan and snapshot.
    -   Show store size, free disk space, and errors.
    -   Show if history may be incomplete.
9.  **Testing**
    -   Unit tests for hashing, object storage, metadata, and restore
        planning.
    -   Integration tests for snapshot creation and restore.
    -   Crash-injection tests for commit boundaries.
    -   Test fixtures for rename, delete, rapid writes, large files, and
        file replacement.
    -   A deterministic workload generator for benchmarks.

### P1 --- Required for a useful beta

-   Text diff preview.
-   Search and timeline filters.
-   Directory-level restore with a preview plan.
-   Retention policies based on age and count.
-   Safe garbage collection with dry-run output.
-   Configuration export and import.
-   Resource limits for CPU, memory, and scan rate.
-   Better handling for very large files and high-change directories.
-   A simple desktop interface or local UI.
-   Clear backup/export instructions.
-   Signed release artifacts and a documented update process.

### P2 --- Advanced capabilities

-   Optional encrypted replication to a second device.
-   Resume interrupted transfers.
-   Integrity verification across the replica.
-   Device pairing and key rotation.
-   Content-defined chunking for large files with small edits, after
    benchmarks justify it.
-   Multiple platform backends for filesystem events and atomic file
    operations.
-   Plugin interface for additional storage targets, only after the core
    interfaces stabilize.
-   Optional rules for common build and cache folders.
-   User-controlled retention profiles.

### Product performance targets

These are **initial targets**, not measured claims. Record a baseline
first and revise targets if the workload shows that they are not
realistic.

-   Idle memory: target under 50 MiB for the core process on the
    reference Linux test machine.
-   Idle CPU: target near 0% average over a 10-minute idle test,
    excluding scheduled reconciliation.
-   File restore: no corruption on any successful restore test.
-   Repeated unchanged snapshot: avoid writing duplicate object bytes
    when the content hash is unchanged.
-   Startup: recover the metadata state without a full content scan when
    the database and journal are clean.
-   Large workload: publish p50, p95, and p99 latency, throughput, peak
    memory, CPU time, and bytes stored.
-   Recovery: every committed snapshot used in the test must pass an
    integrity check after injected crashes.

Do not publish "under 50 MiB" or other targets as achieved until the
benchmark script has measured them on a named machine and software
version.

------------------------------------------------------------------------

## 7. UX rules

The first screen should answer: **"Is my work protected, and can I get
it back?"**

It should show:

-   Protected folders.
-   Current monitoring status.
-   Time of the last successful scan.
-   Time of the last complete recovery point.
-   Number of files in the latest recovery point.
-   Physical storage used.
-   Any warning that affects recovery.
-   A clear way to browse history and restore a file.

### Restore flow

1.  Select a file or a recovery point.
2.  Select the version to restore.
3.  Preview text content or view file details.
4.  Choose the destination.
5.  Review any overwrite warning.
6.  Confirm restore.
7.  Verify the written content.
8.  Show success or a useful error.
9.  Offer a way to undo the restore when a prior destination version
    exists.

### UX rules

-   Use direct words such as "Restore file" and "History is incomplete."
-   Do not use vague success messages such as "Operation completed" when
    the user needs to know what changed.
-   Never hide an error because the background process can retry it.
-   Do not imply that a folder is protected until the initial scan and
    first complete snapshot have succeeded.
-   Make exclusions visible. Do not silently ignore a file type.
-   Do not show "safe" when the store has not passed integrity checks.
-   Provide keyboard navigation and readable contrast.
-   Do not require a person to understand hashes, chunks, journals, or
    database transactions to recover a file.
-   Keep advanced diagnostics available for technical users.

------------------------------------------------------------------------

## 8. Safety, privacy, and security

Rewind stores copies of personal and work files. A bug or security issue
can cause real harm. Security is a product requirement, not a later
polish task.

### Threat model

Protect against:

-   Accidental deletion or overwrite.
-   Rewind process crashes and power loss during a write.
-   Corrupt object files.
-   Malformed paths and path traversal.
-   Symlink attacks that try to write outside a protected root.
-   A local unprivileged process that tries to alter the store, within
    the limits of the host account model.
-   Interrupted or replayed replication messages in a later network
    version.
-   Unintended exposure of file content through logs.

Do not claim full protection against:

-   An attacker with root or equivalent control over the machine.
-   A compromised user account that can modify the Rewind process and
    its files.
-   Disk failure when the only copy is on that disk.
-   Theft of an unencrypted device.
-   Files that change and disappear before Rewind can read them.
-   Unsupported file-system behavior or hardware that violates
    durability guarantees.

### Required controls

-   Use standard cryptographic hash functions from a trusted library.
-   Do not design a custom encryption scheme.
-   Use safe path APIs and reject path traversal.
-   Avoid following symlinks during restore unless a later, reviewed
    policy explicitly permits it.
-   Set restrictive permissions on the store and configuration.
-   Keep file content out of logs by default.
-   Treat file names and metadata as potentially sensitive.
-   Verify object size and hash when reading objects.
-   Use atomic replacement where supported, and document
    platform-specific limits.
-   Define when `fsync` or equivalent durability operations are
    required.
-   Test disk-full, permission-denied, and corrupted-store cases.
-   Document key storage and recovery before adding encryption.
-   For replication, authenticate peers, encrypt transport, validate all
    lengths, limit message sizes, and use idempotent operations.
-   Keep a security reporting policy in the repository before public
    beta.

### Privacy defaults

-   No analytics by default.
-   No cloud dependency.
-   No content upload.
-   No account requirement.
-   Local logs with a documented location and retention policy.
-   Clear consent before replication.
-   A way to inspect protected roots and retention settings.
-   No promise that file names and timestamps are harmless metadata.

------------------------------------------------------------------------

## 9. Architecture

### High-level data flow

``` text
                 ┌─────────────────────────────┐
                 │ User: CLI / Desktop UI      │
                 └──────────────┬──────────────┘
                                │
                 ┌──────────────▼──────────────┐
                 │ Application / Use Cases     │
                 │ browse, snapshot, restore   │
                 └───────┬─────────────┬───────┘
                         │             │
            ┌────────────▼───┐   ┌─────▼────────────┐
            │ Timeline /     │   │ Restore Planner  │
            │ Query Service  │   │ and Safety Copy  │
            └────────────┬───┘   └─────┬────────────┘
                         │             │
                 ┌───────▼─────────────▼───────┐
                 │ Metadata Repository         │
                 │ SQLite transactions         │
                 └──────────────┬──────────────┘
                                │ object references
                 ┌──────────────▼──────────────┐
                 │ Immutable Content Store     │
                 │ hash → verified bytes       │
                 └─────────────────────────────┘

Filesystem events ──► Event Queue ──► Coalescer ──► Scan / Snapshot Coordinator
                                                   │
                                      Reconciliation Scanner
```

### Core components

**Filesystem observer** - Receives platform-specific change
notifications. - Converts them to internal events. - Does not make a
snapshot directly. - Must handle overflow, invalidation, and watch
restart.

**Event queue and coalescer** - Combines bursts of events for the same
path. - Applies queue limits and backpressure. - Records when event loss
or overflow occurs. - Requests reconciliation when event history may be
incomplete.

**Reconciliation scanner** - Walks the protected root. - Uses file
identity and metadata as hints, not proof that content is unchanged. -
Hashes content when needed. - Finds changes missed by the notification
stream. - Records scan start, completion, and errors.

**Snapshot coordinator** - Turns a consistent scan result into a
recovery point. - Defines the commit protocol. - Makes a snapshot
visible only after all required data is available. - Keeps incomplete
work separate from committed snapshots.

**Content-addressed store** - Stores immutable objects under a
hash-derived identity. - Writes to temporary storage and verifies
content. - Exposes `put`, `get`, `contains`, `verify`, and later
`delete_unreferenced` operations. - Does not decide retention on its
own.

**Metadata repository** - Stores roots, snapshots, file entries, object
references, events, settings, and restore history. - Uses transactions
and schema migrations. - Enforces referential integrity.

**Timeline and search** - Queries snapshots and file versions. -
Supports path, date, and event filters. - Distinguishes observed events
from changes inferred by a scan.

**Restore planner** - Calculates what will change before it writes. -
Checks destinations, permissions, path traversal, and overwrite
conditions. - Creates a safety copy when needed. - Writes and verifies
temporary content before replacement.

**Retention and garbage collection** - Computes which objects are still
referenced by committed snapshots. - Offers a dry-run plan. - Deletes
only unreferenced objects. - Uses a safe sequence so a crash cannot
remove data needed by a committed snapshot.

**Optional replication service --- later phase** - Sends encrypted,
verified objects and metadata to a trusted peer. - Supports partial
reads/writes, reconnects, bounded queues, and idempotent retries. - Does
not block the local snapshot path while a peer is offline.

### Key invariants

1.  A committed snapshot never references an object that is not fully
    written and verified.
2.  Published objects are immutable.
3.  A restore does not replace the destination until the replacement
    content has been written and verified.
4.  A restore preserves the prior destination when the user has approved
    a replacement.
5.  Garbage collection never removes an object referenced by a committed
    snapshot.
6.  A detected event gap causes reconciliation or an explicit
    incomplete-history warning.
7.  The local core works with networking disabled.
8.  Failure to replicate does not invalidate a local committed snapshot.
9.  Metadata and object-store changes follow a documented commit order.
10. A successful operation has a testable definition and an observable
    result.

------------------------------------------------------------------------

## 10. Data model

Use SQLite for metadata in the first version. Keep the schema small and
explicit. Add migrations before the first public release.

### Initial entities

**`protected_root`** - `root_id` - `path` - `created_at` - `enabled` -
`policy_json` or normalized policy fields - `last_scan_status` -
`last_successful_scan_at`

**`snapshot`** - `snapshot_id` - `root_id` - `started_at` -
`completed_at` - `status` (`building`, `committed`, `failed`,
`incomplete`) - `manifest_hash` - `file_count` - `logical_bytes` -
`error_code` and safe error detail

**`file_entry`** - `snapshot_id` - `relative_path` - `entry_type` -
`size_bytes` - `mtime_ns` when available - `file_identity` when
available - `content_hash` or object reference - `mode` and supported
metadata - `link_target` only if a later policy supports links

**`object`** - `object_hash` - `size_bytes` - `stored_size_bytes` -
`storage_state` - `created_at` - `verified_at` - `format_version`

**`event`** - `event_id` - `root_id` - `path` - `event_kind` -
`observed_at` - `source` (`notification`, `scan`, `restore`,
`user_action`) - `confidence` or equivalent status - `snapshot_id` where
applicable

**`restore_operation`** - `restore_id` - `snapshot_id` - `source_path` -
`destination_path` - `started_at` - `completed_at` - `status` -
`safety_copy_reference` - `error_code`

### Data model decisions

-   Use relative paths inside a root to prevent the manifest from
    depending on a single absolute path.
-   Store time in a consistent UTC representation. Show local time in
    the UI.
-   Keep object identity separate from file path. Two paths may use the
    same content object.
-   Do not assume modification time alone proves content equality.
-   Do not assume file size alone proves content equality.
-   Define how hard links, symbolic links, sparse files, permissions,
    extended attributes, and special files work before claiming support
    for them.
-   Add database indexes only for real query patterns and benchmark
    them.
-   Use migrations with forward and rollback notes. Do not edit old
    migrations after they have been used by other users.

------------------------------------------------------------------------

## 11. Repository structure

Start with a small modular monolith. Do not split the project into
network services before there is a real need. Keep interfaces clear so
storage, filesystem observation, and user interfaces can grow without a
rewrite.

``` text
rewind/
├── README.md
├── LICENSE
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── SECURITY.md
├── CHANGELOG.md
├── CMakeLists.txt
├── CMakePresets.json
├── .clang-format
├── .clang-tidy
├── .editorconfig
├── .gitignore
├── cmake/
│   ├── CompilerWarnings.cmake
│   ├── Sanitizers.cmake
│   └── Dependencies.cmake
├── docs/
│   ├── product/
│   │   ├── PRD.md
│   │   ├── personas-and-jobs.md
│   │   ├── competitive-research.md
│   │   ├── user-interview-plan.md
│   │   └── ux-flows.md
│   ├── architecture/
│   │   ├── overview.md
│   │   ├── invariants.md
│   │   ├── data-model.md
│   │   ├── snapshot-commit-protocol.md
│   │   ├── restore-protocol.md
│   │   ├── retention-and-gc.md
│   │   └── decisions/
│   │       ├── 0001-linux-first.md
│   │       ├── 0002-sqlite-metadata.md
│   │       ├── 0003-immutable-objects.md
│   │       ├── 0004-event-plus-reconciliation.md
│   │       └── 0005-hash-before-chunking.md
│   ├── security/
│   │   ├── threat-model.md
│   │   ├── privacy.md
│   │   └── recovery-limits.md
│   ├── testing/
│   │   ├── test-strategy.md
│   │   ├── crash-matrix.md
│   │   └── benchmark-method.md
│   └── research/
│       └── competitor-test-results.md
├── include/
│   └── rewind/
│       ├── app/
│       ├── config/
│       ├── domain/
│       ├── filesystem/
│       ├── observer/
│       ├── snapshot/
│       ├── storage/
│       ├── metadata/
│       ├── restore/
│       ├── retention/
│       ├── timeline/
│       └── platform/
├── src/
│   ├── app/
│   ├── config/
│   ├── domain/
│   ├── filesystem/
│   ├── observer/
│   │   └── linux/
│   ├── snapshot/
│   ├── storage/
│   ├── metadata/
│   ├── restore/
│   ├── retention/
│   ├── timeline/
│   ├── platform/
│   │   └── linux/
│   ├── cli/
│   └── main.cpp
├── migrations/
│   └── sqlite/
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
│   ├── sample-config.toml
│   └── demo-workspace/
├── packaging/
│   └── linux/
└── .github/
    └── workflows/
        ├── build.yml
        ├── test.yml
        ├── sanitizers.yml
        └── benchmark-smoke.yml
```

### How the structure should grow

-   **Do not create empty directories just to match this diagram.** Add
    a directory when the first file for that responsibility exists.
-   Start with `domain`, `storage`, `metadata`, `snapshot`, `restore`,
    `observer/linux`, and `cli`.
-   Keep operating-system calls behind platform interfaces. Add
    `observer/windows` only when Windows support is planned and tested.
-   Keep UI code outside the core. The core must remain usable from
    tests and the CLI.
-   Keep replication in a separate module after local recovery is
    reliable.
-   Keep benchmark tools separate from production code.
-   Keep decisions in short Architecture Decision Records (ADRs). Each
    ADR must state context, options, decision, trade-offs, and
    conditions that would make the decision change.
-   Avoid one giant `RewindManager` class. Use small components with
    clear ownership and testable contracts.
-   Avoid a generic plugin system until a real use case needs it.

------------------------------------------------------------------------

## 12. Phase-wise implementation plan

The phases define outcomes and exit checks. They are not calendar
promises. Complete a phase only when its exit conditions pass. Do not
start a later feature to hide a weak core.

### Phase 0 --- Validate the problem and product claim

**Purpose:** Confirm that the recovery workflow is painful enough to
solve and identify the best first user.

**Work** 1. Interview 10--15 potential users. 2. Ask about the last time
they lost, overwrote, or could not find a file version. 3. Ask what they
did next and how long recovery took. 4. Observe at least five people
attempt the same recovery tasks with tools they already use. 5. Test the
competitor workflows listed in Section 3. 6. Build a small clickable or
paper prototype of the timeline and restore flow. 7. Define the initial
workload for engineering benchmarks. 8. Record the user problems that
are observed, not only stated preferences.

**Deliverables** - Research notes with consent. - Competitor workflow
table with tested steps. - Three top user jobs. - Initial UX flow. -
Updated PRD and measurable success criteria.

**Exit checks** - At least five users describe a recent recovery problem
with a clear consequence. - At least three users complete a prototype
task and reveal a specific usability issue. - The team can explain why
the first workflow is different from simply running an existing backup
tool. - If these checks fail, revise the product before building more
code.

### Phase 1 --- Repository and engineering foundation

**Purpose:** Make the code easy to build, test, and inspect.

**Work** 1. Create the repository and apply a suitable open-source
license after reviewing its terms. 2. Add CMake and a supported compiler
baseline. 3. Set a C++ standard and document the supported compiler
versions. 4. Enable strict warnings. 5. Add formatting and static
analysis configuration. 6. Add a minimal executable and a `--version`
command. 7. Add unit-test and integration-test targets. 8. Add
continuous integration for build and tests. 9. Add sanitizer jobs where
supported. 10. Define error types, logging rules, and configuration
loading. 11. Write the first ADRs for Linux-first scope, SQLite, and
immutable object storage.

**Deliverables** - Clean build from a fresh checkout. - Automated tests
on each pull request. - Minimal CLI. - Contribution and security
reporting instructions. - First ADR set.

**Exit checks** - A new contributor can build and run tests from the
README. - CI fails on a compile error or failed test. - No secrets or
user file content appear in default logs. - Warnings are either fixed or
documented with a narrow reason.

### Phase 2 --- Safe local object store

**Purpose:** Store bytes reliably before adding automatic monitoring.

**Work** 1. Implement a content hash using a maintained library. 2.
Define the object path layout and format version. 3. Write objects to
temporary files. 4. Flush and close objects according to the durability
policy. 5. Verify object hashes after writes. 6. Publish completed
objects with an atomic operation where supported. 7. Implement `put`,
`get`, `contains`, `verify`, and safe error handling. 8. Add a store
inspector for tests and debugging. 9. Test duplicate content and
corrupted objects. 10. Test file permissions, disk-full conditions, and
interrupted writes. 11. Define store locking and behavior when two
Rewind processes start.

**Deliverables** - Immutable object store. - Object format document. -
Unit and integration tests. - Store integrity command.

**Exit checks** - The same bytes map to the same object identity. -
Repeated writes do not create repeated content objects. - A failed write
never publishes a partial object as valid. - Corrupt objects are
detected. - Tests show the documented result after process termination
during each write step.

### Phase 3 --- Metadata database and snapshot protocol

**Purpose:** Create complete recovery points that survive a process
crash.

**Work** 1. Add SQLite schema and migrations. 2. Add root, snapshot,
file-entry, object, and event tables. 3. Add foreign keys and integrity
checks. 4. Define the exact snapshot state machine. 5. Write the commit
protocol in a design document before implementation. 6. Implement a
manifest for each snapshot. 7. Ensure all referenced objects are
verified before snapshot commit. 8. Commit metadata in a transaction. 9.
Add startup recovery for abandoned temporary work. 10. Add a command to
list snapshots and inspect a manifest. 11. Add failure injection around
every commit boundary.

**Deliverables** - Transactional metadata layer. - Snapshot
coordinator. - Snapshot manifest format. - Crash matrix and automated
tests.

**Exit checks** - A snapshot is either committed and usable or
incomplete and not presented as usable. - A crash cannot make a
committed snapshot refer to an incomplete object. - Startup recovery is
repeatable. - Integrity checks detect missing references and
inconsistent metadata.

### Phase 4 --- Initial scan and explicit snapshots

**Purpose:** Prove that Rewind can capture and restore a folder before
background monitoring adds complexity.

**Work** 1. Let a user configure one protected root. 2. Walk the
directory safely. 3. Define handling for permissions, symlinks, special
files, and files that change during a scan. 4. Hash file content and
store objects as needed. 5. Build a manifest and commit a snapshot. 6.
Add CLI commands to list roots, create a snapshot, list snapshots, and
verify a snapshot. 7. Compare the restored tree with a known fixture. 8.
Add large-directory and large-file tests. 9. Record scan time, bytes
read, objects created, and memory use.

**Deliverables** - Manual snapshot command. - Safe directory scanner. -
Snapshot verification. - Deterministic integration fixtures.

**Exit checks** - A snapshot of a test tree can be restored to an
equivalent tree for all supported file types. - Permission errors and
changing files are reported. - A failed scan does not appear as a
complete snapshot. - The tool does not write outside its configured
store and restore target.

### Phase 5 --- Restore engine and safety model

**Purpose:** Make recovery safe, not merely possible.

**Work** 1. Implement a restore plan separate from the restore action.
2. Show source version, target path, and overwrite behavior. 3. Restore
to a temporary file. 4. Verify the temporary file against the expected
hash. 5. Preserve the current destination before replacement. 6. Use a
safe replacement operation. 7. Record the restore action and outcome. 8.
Support restore to an alternate path. 9. Add rollback behavior for
failures before replacement. 10. Test path traversal, symlink changes,
permission changes, and disk-full errors. 11. Define which file metadata
can be restored and which cannot.

**Deliverables** - Restore planner and executor. - Safety-copy
behavior. - Restore audit record. - Threat-model update.

**Exit checks** - The destination remains unchanged when a
pre-replacement step fails. - Existing files are not silently
overwritten. - The restored bytes match the selected object hash. - The
previous destination can be recovered after a successful replacement. -
Security tests reject paths outside the allowed restore root.

### Phase 6 --- Automatic observation and reconciliation

**Purpose:** Keep history up to date without asking the user to create
snapshots manually.

**Work** 1. Implement Linux filesystem notifications behind an
interface. 2. Translate raw events into internal event records. 3.
Coalesce event bursts. 4. Handle rename pairs where the platform
provides enough information. 5. Detect event queue overflow and
invalidated watches. 6. Restart watches safely after directory changes.
7. Add a periodic reconciliation scan. 8. Compare scan results with the
last committed manifest. 9. Schedule snapshots based on quiet periods
and configured limits. 10. Bound queue length, memory use, and work per
cycle. 11. Report the last successful scan and any gap in observation.
12. Test rapid create-write-rename-delete sequences.

**Deliverables** - Background observer. - Event queue and coalescer. -
Reconciliation scanner. - Clear history-completeness status.

**Exit checks** - Tests recover the correct final state after rapid file
changes. - A simulated event loss is detected by reconciliation. - The
product does not claim continuous protection when it has not completed a
scan. - Idle resource use is measured and reported. - A crash and
restart do not leave monitoring falsely marked as healthy.

### Phase 7 --- Timeline, search, and first usable interface

**Purpose:** Turn the engine into a tool that people can use without
reading internal implementation details.

**Work** 1. Add a CLI for status, roots, history, diff, restore, verify,
and configuration. 2. Add a simple timeline view. 3. Show added,
modified, deleted, and inferred changes. 4. Add path and date filters.
5. Add text diff preview with size limits. 6. Add file metadata preview
for binary files. 7. Show disk usage and retention status. 8. Add clear
warnings and recovery instructions. 9. Run usability tests with people
who did not build the project. 10. Only then choose a desktop toolkit or
local UI based on user needs.

**Deliverables** - Usable CLI. - Timeline and restore flow. - UX test
report. - A short demo recording with a real recovery task.

**Exit checks** - A new user can protect a folder, find a prior version,
and restore it without developer help. - The user can tell whether
history is current or incomplete. - Restore errors explain the next safe
action. - Usability issues found in testing are tracked and fixed.

### Phase 8 --- Retention, storage control, and performance

**Purpose:** Keep the store useful over time without risking committed
history.

**Work** 1. Define retention rules by age, count, and optional storage
limit. 2. Show a dry-run cleanup plan. 3. Compute object reachability
from committed snapshots. 4. Design a crash-safe garbage collection
protocol. 5. Prevent concurrent cleanup from deleting objects needed by
a new snapshot. 6. Add object integrity sweeps. 7. Measure fixed-size
chunking versus whole-file objects before changing the format. 8. Test
compression choices on source code, text, images, archives, and
already-compressed files. 9. Add exclusions for known high-volume
caches, with visible user control. 10. Publish benchmark scripts and raw
results.

**Deliverables** - Retention engine. - Dry-run garbage collection. -
Store integrity report. - Benchmark report with reproducible workloads.

**Exit checks** - Garbage collection never deletes an object referenced
by a committed snapshot in the test suite. - A crash during cleanup is
recoverable. - Storage reduction claims include the workload and
baseline. - Performance changes have measurements and regression tests.

### Phase 9 --- Security and release hardening

**Purpose:** Prepare the product for people who trust it with real work.

**Work** 1. Review all file-path and symlink operations. 2. Review
permissions and local store access. 3. Add fuzz tests for manifest
parsing and configuration input. 4. Add sanitizers and static analysis
to CI. 5. Test corrupted metadata and corrupted object data. 6. Test
disk-full and read-only filesystems. 7. Write a recovery-limits
document. 8. Define how users export or inspect their data. 9. Add
versioned formats and migration tests. 10. Create signed release
artifacts where practical. 11. Publish the threat model and security
contact. 12. Run a beta with opt-in user feedback.

**Deliverables** - Security review checklist. - Recovery and data-format
documentation. - Beta build. - Known limitations list. - Release
checklist.

**Exit checks** - No known critical data-loss bug remains open. - Every
reported failure has a reproducible test or a documented reason why it
cannot be reproduced. - Users can understand the product's protection
limits before enabling it. - Upgrade and downgrade behavior is
documented.

### Phase 10 --- Optional encrypted device-to-device replication

**Purpose:** Add a second copy without making a cloud account a
requirement.

**Work** 1. Define device identity and trust setup. 2. Select a
maintained transport and cryptographic library. 3. Define a versioned,
length-limited message format. 4. Authenticate peers and encrypt
transport. 5. Send object IDs and verified object bytes. 6. Use
acknowledgements only after the receiver has stored and verified the
object. 7. Make retries idempotent. 8. Support partial reads, partial
writes, backpressure, timeouts, and reconnects. 9. Resume interrupted
transfers. 10. Make replication state separate from local snapshot
state. 11. Handle revocation and key rotation. 12. Test a peer
disconnect during every transfer stage. 13. Document the limits of a
replica on a device that is always connected to the same compromised
account.

**Deliverables** - Optional peer transport. - Pairing and trust
workflow. - Replication status. - Network fault tests. - Security
review.

**Exit checks** - Local snapshots remain usable when the peer is
offline. - A receiver never acknowledges an object before verification
and durable storage. - Duplicate messages do not create inconsistent
state. - Interrupted transfer resumes without corrupting the store. - No
content leaves the device before the user enables and configures
replication.

### Phase 11 --- Cross-platform support and ecosystem

**Purpose:** Expand only after the Linux core has proved reliable.

**Work** 1. Review user demand for Windows and macOS. 2. Build a
platform interface for notifications, atomic replacement, permissions,
and durability operations. 3. Implement one operating system at a time.
4. Add platform-specific integration tests. 5. Document feature
differences by platform. 6. Consider package managers and signed
installers. 7. Add translations and accessibility improvements based on
user needs. 8. Add a stable import/export format before considering
integrations.

**Deliverables** - Supported platform matrix. - Platform-specific
tests. - Install and upgrade instructions. - Compatibility and
data-format policy.

**Exit checks** - Each supported platform passes its own crash, restore,
and security tests. - The README does not imply equal behavior where the
platform APIs differ. - A user can move or export recovery data using
documented steps.

------------------------------------------------------------------------

## 13. Core engineering choices

### C++ and modular design

Use modern C++ for control over file I/O, memory use, concurrency, and
platform interfaces. Keep ownership explicit. Prefer RAII, value types,
clear error handling, and small components. Do not use raw owning
pointers. Do not introduce concurrency until the single-threaded
protocol is correct.

### Metadata database

Use SQLite at first. It provides transactions and a mature storage
engine. The project should learn how to design safe commit protocols
around it, not spend its first months building a database.

### Object storage

Use immutable, hash-addressed objects. This makes integrity checks and
duplicate detection clear. Start with whole-file objects. Add chunking
only when benchmark results show that large files with small edits
create a meaningful storage problem.

### Change detection

Use filesystem notifications for speed and scans for correctness.
Notifications can be lost, coalesced, or invalidated. A scan can find
the current state but may not reconstruct every intermediate version.
The UI must distinguish these facts.

### Threading

Start with a small number of execution contexts and bounded queues. Add
worker threads only for measured work such as hashing or compression.
Define ownership and cancellation rules before adding asynchronous
tasks.

### Networking

Do not add networking to the MVP. When replication begins, test partial
reads and writes, bounded queues, retries, timeouts, authentication, and
integrity. A TCP connection is a byte stream, not a message protocol.

### Cryptography

Use established libraries and protocols. Do not invent encryption, key
exchange, or authentication algorithms. Document key creation, storage,
rotation, revocation, and recovery before enabling encrypted
replication.

### Observability

Log state changes, failures, timings, and counters. Do not log file
content. Provide a diagnostic command that can export safe metadata for
bug reports, with a preview before export.

------------------------------------------------------------------------

## 14. Testing and benchmark plan

Rewind's central promise is data recovery. Tests must prove behavior
under failure, not only prove that the happy path works.

### Test groups

-   **Unit:** hash calculation, path validation, event coalescing,
    manifest parsing, restore planning.
-   **Integration:** scan a fixture, commit a snapshot, list history,
    restore content.
-   **Crash:** terminate the process at each object-write and
    snapshot-commit boundary.
-   **Filesystem:** rename, delete, replace, permission change, symlink,
    hard link, sparse file, large file, and rapidly changing file.
-   **Storage:** duplicate content, corrupted object, missing object,
    full disk, read-only directory, and failed flush.
-   **Security:** traversal paths, hostile symlinks, malformed
    manifests, oversized inputs, and unexpected file types.
-   **Concurrency:** simultaneous change and scan, restore during a new
    change, and competing process startup.
-   **Recovery:** restart after failure and verify all previously
    committed snapshots.
-   **Usability:** a person who did not build Rewind performs a restore.
-   **Performance:** small-file tree, large files, source repository,
    many versions, and high-change workload.

### Benchmark rules

Every public benchmark must include:

-   Machine CPU, memory, storage device, and file system.
-   Operating system and kernel version.
-   Compiler, build type, and dependency versions.
-   Workload generator version and seed.
-   Number and size distribution of files.
-   Number of changed, added, renamed, and deleted files.
-   Cache conditions where relevant.
-   Baseline tool and exact settings.
-   Number of runs and summary method.
-   p50, p95, and p99 latency where relevant.
-   Throughput, CPU time, peak resident memory, and stored bytes.
-   Raw results or scripts that allow a reader to reproduce the result.

### Example benchmark workloads

1.  **Small project:** 20,000 files, mostly source and configuration
    files.
2.  **Large assets:** several large files with small edits.
3.  **Many small edits:** frequent updates to hundreds of files.
4.  **No change:** repeated scans when no file content changes.
5.  **Deletion burst:** delete and rename many files in a protected
    directory.
6.  **Recovery drill:** restore one file, a directory, and a full
    snapshot.
7.  **Low-space test:** approach a configured storage limit and verify
    the warning behavior.

The exact file counts and sizes must be recorded in the benchmark tool.
Do not select only workloads that make Rewind look good.

------------------------------------------------------------------------

## 15. Success metrics

### User value

-   Percentage of test users who restore a file without help.
-   Median time from "I lost a file" to a verified restore.
-   Number of user errors during restore.
-   Percentage of users who understand whether their history is current.
-   User-reported confidence after a recovery task.
-   Number of real recovery events where Rewind provided a useful
    version.

### Reliability

-   Number of committed snapshots that fail integrity verification.
-   Number of data-loss defects.
-   Number of crash-injection cases that leave invalid committed state.
-   Number of event gaps detected and reconciled.
-   Number of restores that do not match the selected content hash.
-   Number of garbage-collection tests that remove a referenced object.

For critical integrity metrics, the target is zero known failures in the
tested conditions. This does not prove that failure is impossible in all
conditions.

### Efficiency

-   Idle and active memory use.
-   Idle and active CPU use.
-   Time to first recovery point.
-   Time to scan and snapshot each workload.
-   Bytes read and written.
-   Physical store size divided by logical data size.
-   Restore throughput.
-   Background work after a large event burst.

### Adoption and trust

-   Time to first successful protected folder.
-   Percentage of users who understand the local-only default.
-   Number of users who can export or remove their data.
-   Issue response time and time to fix reproducible data-loss bugs.
-   Number of platform-specific limitations reported by users.

------------------------------------------------------------------------

## 16. Risks and responses

  -----------------------------------------------------------------------
  Risk                    Why it matters          Response
  ----------------------- ----------------------- -----------------------
  The idea is not         Existing backup tools   Validate real workflows
  distinct enough         already cover much of   and position on
                          the technical base      measurable ease of
                                                  recovery

  Filesystem events are   History can miss        Reconciliation scans
  lost                    changes                 and visible
                                                  incomplete-history
                                                  status

  Snapshot metadata       A snapshot may point to Define and test the
  commits before object   missing content         commit order; use
  data is safe                                    immutable verified
                                                  objects

  Restore damages newer   Recovery can become a   Preview, safety copy,
  work                    second loss             temporary write, hash
                                                  check, explicit
                                                  overwrite

  Store grows without     Users run out of disk   Visible storage budget,
  limit                   space                   retention policy,
                                                  dry-run garbage
                                                  collection

  Deduplication adds      Bugs can break object   Start with whole-file
  complexity              references              objects; add chunking
                                                  only after measurements

  Local store is lost     A local copy is not an  State the limit and add
  with the device         independent backup      optional second-device
                                                  replication later

  Security scope expands  Encryption and          Keep network disabled
  too early               networking add new      in the MVP; use
                          risks                   standard libraries
                                                  later

  The UI becomes a        A polished UI can hide  Prove CLI and restore
  distraction             weak recovery behavior  invariants before
                                                  investing in a full
                                                  desktop UI

  Cross-platform behavior File APIs and           Start on Linux and test
  differs                 durability guarantees   each later platform
                          differ                  separately

  "Lightweight" becomes   Resource use depends on Publish reproducible
  an unsupported claim    workload and machine    measurements and state
                                                  the test conditions

  A false product claim   Users may rely on       Keep a known-limits
  harms trust             Rewind with important   page and do not promise
                          data                    perfect recovery
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 17. Open questions and decision gates

Do not resolve these by guessing. Use a small experiment or a documented
decision.

-   What is the minimum snapshot interval that feels automatic without
    creating needless work?
-   Should snapshots occur after a quiet period, on a timer, or both?
-   What is the right default retention policy for a student laptop?
-   Which file metadata can be restored safely on the first platform?
-   How should Rewind behave when a file changes during a scan?
-   Should a first release use whole-file objects only, or does a real
    workload justify chunking?
-   Which desktop interface, if any, best serves the target user?
-   Is device-to-device replication a common need or only an advanced
    feature?
-   Which operating system should follow Linux?
-   What is the smallest useful set of exclusions?
-   What level of local encryption is needed, and how will users recover
    a lost key?
-   How should Rewind handle a protected folder that is on an external
    drive when the drive is disconnected?

Every decision should have an owner, evidence, date, and review
condition.

------------------------------------------------------------------------

## 18. Portfolio story: how to present the engineering

Rewind should stand out as a product and a systems project. The
portfolio must show evidence, not only a long feature list.

### The short pitch

> Rewind is a local-first file recovery system built in C++20. It
> records automatic history for selected folders, stores immutable
> content objects, and restores earlier file versions with integrity
> checks. The project focuses on crash-safe snapshot commits, event
> reconciliation, safe restore, storage efficiency, and optional
> encrypted replication.

### The engineering story

**1. Start with a real human problem.** Show a file being lost and the
recovery path that Rewind provides.

**2. Explain the hard part.** Automatic file history is not only a
watcher. The system must handle missed events, changing files, partial
writes, crashes, and safe restore.

**3. Show the invariants.** Explain why a committed snapshot must never
reference incomplete content and why restore must verify data before
replacement.

**4. Show failure tests.** Kill the process during writes, interrupt a
transfer in a later phase, corrupt an object, and fill the test disk.
Show what the system does.

**5. Show measurements.** Publish storage overhead, restore latency,
memory use, and CPU use against a clear baseline.

**6. Show the limits.** Explain why local history does not protect
against every disk failure and why replication is a separate feature.

### Portfolio assets to publish

-   This README as the single top-level product and roadmap document.
-   A 60--90 second demo with a real deletion or overwrite recovery.
-   Architecture diagrams and a short snapshot commit protocol.
-   ADRs for the main engineering choices.
-   Crash-injection test results.
-   A threat model and known-limits page.
-   Reproducible benchmark scripts and raw results.
-   A competitor workflow study based on hands-on testing.
-   A user-test summary with permission and personal details removed.
-   A release that others can build and test.

### Resume bullet template

Do not fill in unmeasured numbers. After the benchmarks exist, use a
result-based bullet such as:

> Built **Rewind**, a C++20 local-first file recovery system with
> immutable content storage, transactional snapshots, crash-safe
> restore, and filesystem-event reconciliation; measured **\[verified
> result\]** across **\[defined workload\]**.

A second bullet can cover reliability or efficiency only after tests
produce evidence:

> Designed and validated **\[specific invariant\]** using **\[number and
> type of failure tests\]**, achieving **\[measured result\]** on
> **\[test environment\]**.

Do not claim "zero data loss," "production-ready," "military-grade
encryption," or "faster than all alternatives" unless the exact claim is
defined and independently supported.

------------------------------------------------------------------------

## 19. First implementation checklist

The first coding session should end with a small, verified result. Do
not implement the full roadmap at once.

-   [ ] Create the repository.
-   [ ] Add CMake and a minimal C++ executable.
-   [ ] Add a `--version` command.
-   [ ] Add unit-test support.
-   [ ] Add formatting and warning checks.
-   [ ] Add CI for build and tests.
-   [ ] Add `docs/architecture/invariants.md`.
-   [ ] Add the first ADR for Linux-first support.
-   [ ] Add a test that writes and reads a known byte sequence.
-   [ ] Add a test that calculates and checks a content hash.
-   [ ] Add a small CLI command that reports the configured store path.
-   [ ] Document the exact build commands and expected output.

The first milestone is not a dashboard. It is a small core that builds,
runs, and has tests.

------------------------------------------------------------------------

## 20. Definition of done

A feature is done only when:

-   Its behavior is documented.
-   Its failure cases are defined.
-   Its tests cover the normal path and important error paths.
-   Its logs do not expose file content.
-   Its error messages tell the user what happened.
-   Its resource use is measured when it can affect the system.
-   Its security implications are reviewed.
-   It has no unsupported product claim.
-   It works from a clean build.
-   The relevant invariants still hold.

A phase is done only when its exit checks pass. A feature is not done
because it compiles or looks good in a demo.

------------------------------------------------------------------------

## 21. Contributing and project values

Rewind values correctness, clear design, measured performance, and
respectful collaboration.

Contributions should:

-   Solve a clear user or engineering problem.
-   Include tests for new behavior.
-   Preserve the documented invariants.
-   Explain trade-offs in the pull request.
-   Include benchmarks for performance claims.
-   Avoid adding a dependency without a clear reason.
-   Avoid changing the on-disk format without a migration plan.
-   Treat a possible data-loss bug as a high-priority issue.

A small, correct feature is better than a large feature that weakens
recovery.

------------------------------------------------------------------------

## 22. References

These links informed the initial competitive review and technical
framing. They do not prove that Rewind is better than the listed tools.

-   Apple Support --- Time Machine local snapshots:
    <https://support.apple.com/en-in/102154>
-   Apple Support --- Restore items backed up with Time Machine:
    <https://support.apple.com/en-us/guide/mac-help/mh11422/mac>
-   Microsoft Support --- Back up and restore with File History:
    <https://support.microsoft.com/en-us/windows/backup-and-restore-with-file-history>
-   Microsoft Support --- Recovery options in Windows:
    <https://support.microsoft.com/en-us/windows/recovery-options-in-windows>
-   BorgBackup documentation: <https://borgbackup.readthedocs.io/>
-   restic documentation: <https://restic.readthedocs.io/>
-   ASD-STE100 official website: <https://www.asd-ste100.org/>
-   ASD-STE100 Issue 9 overview:
    <https://www.asd-ste100.org/about_STE.html>

## Closing thought

A person should not have to be an expert in backups to recover a file.

Rewind cannot prevent every mistake. It can make many mistakes less
costly. It can give people a clear view of their work's history, a safe
way to return to an earlier version, and a better chance to continue
where they left off.

**The best recovery tool is not the one people remember to use. It is
the one that is ready when they need it.**
