# UML & Sequence Diagrams

> **Status:** Outline / Planned  
> **Diagram Format:** Mermaid

## 1. System Architecture Diagram

```mermaid
graph TD
    User([User / CLI]) --> AppCoordinator[App Coordinator]
    
    subgraph Core Subsystems
        AppCoordinator --> Observer[Observer / inotify Engine]
        AppCoordinator --> SnapshotCoord[Snapshot Coordinator]
        AppCoordinator --> RestorePlanner[Restore Planner & Executor]
        AppCoordinator --> TimelineEngine[Timeline Engine]
        
        Observer --> EventQueue[Event Queue & Coalescer]
        EventQueue --> SnapshotCoord
        
        SnapshotCoord --> Scanner[Filesystem Scanner]
        SnapshotCoord --> Storage[Immutable Object Store]
        SnapshotCoord --> Metadata[SQLite Metadata DB]
        
        RestorePlanner --> Storage
        RestorePlanner --> Metadata
        RestorePlanner --> SafetyCopy[Safety Copy Engine]
        
        TimelineEngine --> Metadata
    end
    
    subgraph Filesystem
        Storage --> ObjectFiles[(.rewind/objects/)]
        Metadata --> SQLiteFile[(.rewind/metadata.db)]
        SafetyCopy --> SafetyFiles[(.rewind/safety/)]
        RestorePlanner --> TargetFiles[Live User Files]
    end
```

## 2. Snapshot Commit Protocol Sequence

```mermaid
sequenceDiagram
    autonumber
    participant Coord as SnapshotCoordinator
    participant FS as FilesystemScanner
    participant Store as ObjectStore
    participant DB as MetadataRepository

    Coord->>FS: Scan monitored root directory
    FS-->>Coord: List of file entries (path, stat, size)
    
    loop For each modified file
        Coord->>Store: Stream and write content to tmp/
        Store->>Store: Compute hash & fsync
        Store->>Store: Atomically rename tmp/ to objects/ab/hash
        Store-->>Coord: Return verified content hash
    end
    
    Coord->>DB: Begin SQLite Transaction
    Coord->>DB: Insert file_entry rows referencing object hashes
    Coord->>DB: Insert snapshot record (status: COMMITTED)
    Coord->>DB: Commit SQLite Transaction
    Coord-->>Coord: Snapshot fully committed
```

## 3. Restore Safety Protocol Sequence

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Planner as RestorePlanner
    participant Store as ObjectStore
    participant Target as DestinationFS
    participant Safety as SafetyStore
    participant DB as MetadataRepository

    User->>Planner: Request restore(file, snapshot_id, dest)
    Planner->>Store: Fetch object bytes
    Planner->>Planner: Write to staging temp file and verify hash
    
    alt Destination file exists
        Planner->>Safety: Create safety copy in .rewind/safety/
        Safety-->>Planner: Safety copy confirmed
    end
    
    Planner->>Target: Atomically replace destination with staged file
    Planner->>DB: Record restore operation audit entry
    Planner-->>User: Restore verified & completed successfully
```

---
*Comprehensive class diagrams, statecharts, and component models will be integrated during implementation.*
