# Mini Git Storage

## 1. Overview

Mini Git stores repository metadata and serialized objects inside:

```text
.mini-git/
```

The storage system is intentionally simple and filesystem-based.

---

## 2. Repository Storage Layout

The main structure is:

```text
.mini-git/
├── HEAD
├── index
├── objects/
└── refs/
    ├── heads/
    └── tags/
```

---

## 3. HEAD

`HEAD` stores information about the current repository state.

The `Repository` class is responsible for reading and updating this state.

---

## 4. Index

The index stores staged entries.

An index entry contains:

```text
path
object ID
```

The `Index` class is responsible for:

* adding entries
* checking whether a path is present
* exposing entries
* clearing entries
* saving
* loading

---

## 5. References

References are represented as files.

Branches:

```text
.mini-git/refs/heads/
```

Tags:

```text
.mini-git/refs/tags/
```

A reference stores an object ID.

The `Reference` class abstracts access to these files.

---

## 6. Object Database

Objects are stored under:

```text
.mini-git/objects/
```

`ObjectDatabase` provides the main storage interface.

Its responsibilities include:

```text
store object
read object
check existence
enumerate object IDs
```

---

## 7. Object IDs

An object ID is derived using SHA-256.

Conceptually:

```text
serialized object
      │
      ▼
   SHA-256
      │
      ▼
 object ID
```

Other objects store these IDs when referring to existing objects.

---

## 8. Stored Object Data

The database stores serialized object data rather than C++ object instances.

For example:

```text
Blob object
   │
   ▼
serialize()
   │
   ▼
string
   │
   ▼
ObjectDatabase
```

The reverse operation reconstructs an appropriate C++ object through deserialization.

---

## 9. Object Type Identification

The repository may need to determine the type of a serialized object.

The `ObjectType` subsystem recognizes:

```text
Blob
Tree
Commit
```

It uses the project's serialization formats and deserialization behavior.

Unknown or invalid data produces an exception.

---

## 10. Tree Storage

Trees store entries containing:

```text
name
object ID
tree/blob indicator
```

Nested trees allow a directory structure to be represented without storing a complete filesystem tree inside one object.

---

## 11. Commit Storage

Commits contain:

```text
tree ID
parent IDs
author
message
```

The tree ID identifies the snapshot represented by the commit.

Parent IDs identify previous commits.

---

## 12. Reference-Based History

History is not stored in a separate mutable history table.

Instead:

```text
branch reference
       │
       ▼
    commit
       │
       ▼
    parents
```

allows history to be reconstructed from the object graph.

---

## 13. Reachability

Starting from repository references, Mini Git can traverse object relationships.

For example:

```text
branch
  │
  ▼
commit
  ├── tree
  │    ├── tree
  │    └── blob
  │
  └── parent
```

Objects that are not reached during traversal can be reported as unreachable.

---

## 14. Storage Analysis

`StorageAnalyzer` provides repository storage information based on the objects present in the repository.

`GarbageCollector` provides unreachable-object analysis and reports estimated reclaimable bytes.

The current implementation does not make normal diagnostic commands silently delete unreachable objects.

---

## 15. Storage and Integrity

The same object database is used by both normal repository operations and diagnostic operations.

This means integrity checking operates on the actual stored repository objects rather than on a separate cache or database.

---

## 16. Filesystem Dependency

Mini Git's repository format is directly represented through filesystem paths.

The implementation therefore uses C++ filesystem operations for:

* path construction
* directory creation
* file checks
* directory traversal
* file removal
* repository state manipulation

---

## 17. Repository Isolation

Repository metadata is kept inside `.mini-git`.

Working-tree traversal excludes this directory from normal project content processing.

This prevents internal repository files from being treated as ordinary project files.

---

## 18. Storage Design Summary

The storage architecture is:

```text
Repository
    │
    ├── HEAD
    ├── index
    ├── refs
    │    ├── heads
    │    └── tags
    │
    └── objects
         ├── blobs
         ├── trees
         └── commits
```

The object database provides the persistence boundary, while repository/reference/index classes manage higher-level repository state.
