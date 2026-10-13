# Filesystem Observer & Reconciliation

> **Status:** Outline / Planned  
> **Backend:** Linux `inotify`  
> **Source Directory:** `src/observer/`, `include/rewind/observer/`

## 1. Overview

The observer subsystem tracks changes in monitored directory trees without placing excessive load on system resources. Because filesystem notification APIs are subject to event drops and overflows, Rewind couples notification-driven capture with a periodic reconciliation engine.

## 2. Notification Pipeline

1. **Linux `inotify` Watcher:** Monitors directory handles recursively.
2. **Event Queue:** Bounded queue buffering raw filesystem notifications.
3. **Event Coalescer:** Collapses redundant rapid-fire events (e.g., temporary editor saves, sequential writes) across configurable quiet periods (e.g., 500ms).
4. **Snapshot Trigger:** Signals the snapshot coordinator when file changes settle.

## 3. Queue Overflow & Missing Events

- Operating-system notification queues can drop events during bursty filesystem activity (`IN_Q_OVERFLOW`).
- When an overflow is detected, Rewind marks the history confidence as `incomplete` and triggers a full reconciliation scan.

## 4. Periodic Reconciliation Engine

- Runs at scheduled intervals or upon restart to detect changes that occurred while the daemon was offline or when events were missed.
- Compares live directory trees against the last committed snapshot manifest.

---
*Thread topology, debounce algorithm parameters, and inotify limits will be documented in Phase 6.*
