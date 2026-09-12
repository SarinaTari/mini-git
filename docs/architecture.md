# Mini Git Architecture

## 1. Overview

Mini Git is an educational, Git-inspired version-control system implemented in C++20.

The project models several fundamental version-control concepts:

* repositories
* objects
* content-addressed storage
* blobs
* trees
* commits
* an index
* references
* branches
* tags
* `HEAD`
* checkout
* status
* diff
* merge
* commit ancestry
* object reachability
* repository integrity
* repository analysis

Mini Git uses its own repository format and is not intended to be compatible with the real Git object format or `.git` directory structure.

---

## 2. Main Components

The implementation is divided into several groups of responsibilities.

### Core repository components

* `Repository`
* `Reference`
* `Index`
* `ObjectDatabase`
* `TreeBuilder`

### Object model

* `Object`
* `Blob`
* `Tree`
* `Commit`
* `ObjectType`

### Repository operations

* `Status`
* `Diff`
* `Merge`
* `Ancestry`

### Repository analysis

* `Inspector`
* `Graph`
* `Stats`
* `Analyzer`
* `Impact`
* `Doctor`
* `StorageAnalyzer`
* `Benchmark`

### Integrity

* `Reachability`
* `IntegrityChecker`
* `GarbageCollector`

---

## 3. Repository

`Repository` is the main abstraction for repository-level state.

It manages information associated with:

* the repository root
* the `.mini-git` directory
* `HEAD`
* branches
* tags
* checkout
* commit restoration
* merge state
* merge conflicts
* index rebuilding

The repository root contains the user's working files.

Mini Git's internal data is stored under:

```text
.mini-git/
```

---

## 4. Repository Layout

A typical initialized repository has a structure similar to:

```text
project/
├── files...
└── .mini-git/
    ├── HEAD
    ├── index
    ├── objects/
    └── refs/
        ├── heads/
        └── tags/
```

The exact contents depend on repository state.

### `HEAD`

Stores the current HEAD information.

### `index`

Stores staged path/object information.

### `objects/`

Stores serialized repository objects.

### `refs/heads/`

Stores branch references.

### `refs/tags/`

Stores tag references.

---

## 5. Object Model

Mini Git has three repository object types:

```text
Object
├── Blob
├── Tree
└── Commit
```

### Blob

A Blob represents file content.

### Tree

A Tree represents entries in a directory-like structure.

Each tree entry contains:

* a name
* an object ID
* whether the entry represents a tree

### Commit

A Commit contains:

* a tree ID
* parent commit IDs
* author
* message

---

## 6. Object Database

`ObjectDatabase` provides persistence for serialized objects.

Its main operations are:

```text
store()
exists()
read()
object_ids()
```

Higher-level classes are responsible for interpreting the serialized data.

The database itself provides object storage and retrieval rather than implementing repository commands.

---

## 7. Hashing

`Hash` provides SHA-256 hashing.

The resulting hash is used as the object identifier.

The implementation uses OpenSSL's cryptographic library for SHA-256.

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

---

## 8. Index

The `Index` stores staged file information.

Each entry contains:

```text
path
object ID
```

The index therefore connects working-tree paths to stored objects.

The commit workflow uses the index as input for tree construction.

---

## 9. Tree Construction

`TreeBuilder` constructs tree objects from repository state.

The tree structure represents paths and their corresponding objects.

Conceptually:

```text
Index
  │
  ▼
TreeBuilder
  │
  ▼
Tree objects
```

Nested directories are represented through tree entries that reference other trees.

---

## 10. References

`Reference` provides access to named references.

Mini Git uses references for:

* branches
* tags

Branch references are stored under:

```text
.mini-git/refs/heads/
```

Tag references are stored under:

```text
.mini-git/refs/tags/
```

---

## 11. HEAD

`HEAD` identifies the current repository state.

In a normal branch-based state, it refers to a branch reference.

The repository implementation also contains support for determining whether HEAD is detached.

---

## 12. Status

`Status` compares repository/index state with the working tree.

The resulting `StatusResult` can contain:

* modified paths
* deleted paths
* untracked paths
* merge-in-progress state
* conflict paths

---

## 13. Diff

`Diff` works with snapshots represented internally as:

```text
path → content
```

The implementation supports comparisons involving:

* working tree and index
* index and HEAD
* a commit and working tree
* two commits

---

## 14. Merge

`Merge` performs repository merges using three repository states:

* merge base
* current state
* target state

The implementation can produce conflicts and stores merge state when a merge cannot be completed immediately.

Merge state includes information such as:

* original HEAD
* merge target
* merge message
* conflict paths

---

## 15. Ancestry

`Ancestry` operates on commit parent relationships.

It provides operations for:

* determining ancestry
* finding a merge base

Commit history is traversed through the parent IDs stored in commits.

---

## 16. Reachability

`Reachability` determines objects reachable from repository references.

It traverses relationships between:

```text
references
   ↓
commits
   ↓
trees
   ↓
blobs
```

The result can be used to identify unreachable objects.

---

## 17. Integrity Checking

`IntegrityChecker` produces an `IntegrityReport`.

The report can contain:

* total objects
* valid objects
* corrupted objects
* missing objects
* invalid references
* unreachable objects

The checker is designed to inspect repository state without being a command-specific implementation.

---

## 18. Garbage Collection Analysis

`GarbageCollector` currently provides analysis of unreachable objects.

Its report contains:

* unreachable object IDs
* reclaimable byte count

The current implementation exposes this as a preview/reporting mechanism rather than silently deleting objects.

---

## 19. Repository Intelligence

Mini Git contains several components that analyze repository state.

### `Inspector`

Inspects an individual object.

### `Explainer`

Provides explanations for supported commands/concepts.

### `Graph`

Produces a representation of commit history.

### `Stats`

Produces repository statistics.

### `Analyzer`

Performs repository analysis.

### `Impact`

Analyzes information associated with a commit.

### `Doctor`

Produces a repository health report.

### `StorageAnalyzer`

Analyzes repository storage.

### `Benchmark`

Provides repository-related benchmark output.

These components operate on repository data rather than maintaining a separate repository representation.

---

## 20. Testing Architecture

The project uses CMake and CTest.

The test suite contains independent executables for the major components and repository operations.

Repository-dependent tests create temporary repositories so that tests can operate independently.

---

## 21. Dependency Structure

The implementation broadly separates:

```text
CLI
 │
 ├── Repository operations
 │
 ├── Analysis
 │
 └── Integrity
       │
       ▼
Repository model
 │
 ├── Index
 ├── References
 ├── TreeBuilder
 └── ObjectDatabase
       │
       ▼
Objects
 ├── Blob
 ├── Tree
 └── Commit
```

This is an architectural description of the current implementation rather than a claim that every class follows a strict formal dependency-layer rule.

---

## 22. Core Data Flow

A simplified commit flow is:

```text
working tree
     │
     ▼
   index
     │
     ▼
TreeBuilder
     │
     ▼
   tree
     │
     ▼
  commit
     │
     ▼
ObjectDatabase
     │
     ▼
branch reference
```

History operations traverse the stored commit/tree/object relationships.

---

## 23. Design Objective

The architecture is intended to make version-control concepts visible through separate C++ components.

The project therefore emphasizes:

* explicit state
* small abstractions
* testable components
* filesystem-based persistence
* object serialization
* graph traversal
* repository diagnostics
* understandable implementation
