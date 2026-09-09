# Mini Git Architecture

## Overview

Mini Git is an educational version control system written in modern C++.

The project demonstrates how a Git-like version control system can be constructed from fundamental concepts including:

* filesystem operations
* content-addressable storage
* hashing
* repository management
* objects
* object databases
* trees
* staging
* status detection
* commits
* references
* branches
* history

Mini Git is **not intended to be a drop-in replacement for Git** and does not aim for compatibility with Git's complete internal format.

Instead, the project focuses on understanding the architecture, data flow, and engineering principles behind a version control system.

The implementation is intentionally incremental. Each phase introduces a new subsystem and builds on functionality implemented previously.

---

# High-Level Architecture

The canonical architecture of Mini Git is:

```text
                              mini-git CLI
                                   │
                                   ▼
                            Command Layer
                                   │
              ┌────────────────────┼────────────────────┐
              ▼                    ▼                    ▼
         Working Tree            Index             Repository
              │                    │                    │
              │                    │                    ▼
              │                    │             Object Database
              │                    │                    │
              │                    │              ┌─────┼─────┐
              │                    │              ▼     ▼     ▼
              │                    │            Blob   Tree  Commit
              │                    │
              │                    ▼
              │                  Status
              │
              ▼
            Trees
              │
              ▼
        Future Commits
              │
              ▼
         References
              │
              ▼
             HEAD
```

The major architectural relationships are:

```text
Working Tree
     │
     ├── filesystem inspection ──→ Status
     │
     ├── file content ──→ Blob ──→ Object Database
     │
     └── directory structure ──→ Tree ──→ Object Database

Working Tree
     │
     ▼
    Index
     │
     ▼
   Future Tree
     │
     ▼
   Commit
     │
     ▼
 Reference
     │
     ▼
    HEAD
```

The first half of this architecture is currently implemented through Phase 9.

---

# Repository Structure

Mini Git currently follows this structure:

```text
mini-git/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── include/
│   ├── Repository.hpp
│   ├── Hash.hpp
│   ├── Object.hpp
│   ├── Blob.hpp
│   ├── Tree.hpp
│   ├── Commit.hpp
│   ├── FileReader.hpp
│   ├── ObjectDatabase.hpp
│   ├── TreeBuilder.hpp
│   ├── Index.hpp
│   └── Status.hpp
│
├── src/
│   ├── main.cpp
│   ├── Repository.cpp
│   ├── Hash.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   ├── FileReader.cpp
│   ├── ObjectDatabase.cpp
│   ├── TreeBuilder.cpp
│   ├── Index.cpp
│   └── Status.cpp
│
├── tests/
│   ├── HashTests.cpp
│   ├── ObjectTests.cpp
│   ├── FileReaderTests.cpp
│   ├── BlobTests.cpp
│   ├── TreeBuilderTests.cpp
│   ├── IndexTests.cpp
│   └── StatusTests.cpp
│
└── docs/
    └── architecture.md
```

The `include/` directory contains class declarations.

The `src/` directory contains implementations.

The `tests/` directory contains automated tests.

The `docs/` directory contains architectural and project documentation.

---

# Repository

The `Repository` class represents the repository-level structure of a Mini Git repository.

Repository initialization creates:

```text
.mini-git/
├── HEAD
├── objects/
└── refs/
    └── heads/
```

The initial `HEAD` contains:

```text
ref: refs/heads/main
```

This establishes `main` as the default branch reference.

At the current stage, `main` does not point to a commit because repository-level commit creation and reference updates have not yet been implemented.

The `Repository` component is responsible for identifying the `.mini-git` directory associated with the repository root and creating the initial repository structure.

Currently, repository initialization and commands assume that the command is executed from the repository root.

Repository discovery from nested directories is planned for a later phase.

---

# Working Tree

The working tree is the collection of actual files and directories being managed by Mini Git.

For example:

```text
project/
├── main.cpp
├── README.md
└── src/
    ├── App.cpp
    └── Utils.cpp
```

These files exist outside `.mini-git/`.

The working tree represents the user's current filesystem state.

Mini Git uses the working tree as an input to several subsystems:

```text
Working Tree
    │
    ├── file contents ──→ Blob
    │
    ├── directory structure ──→ Tree
    │
    └── current filesystem state ──→ Status
```

---

# Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The `Hash` class provides the hashing abstraction.

Conceptually:

```text
Input Data
    │
    ▼
 SHA-256
    │
    ▼
64-character hexadecimal Object ID
```

For example:

```text
"hello"
   │
   ▼
SHA-256
   │
   ▼
2cf24dba5fb0a30e26e83b2ac5b9e29e...
```

The same input produces the same hash:

```text
same input
    ↓
same hash
```

Different input produces a different hash:

```text
different input
    ↓
different hash
```

Mini Git hashes the **serialized object representation**, rather than directly hashing raw file contents.

This distinction is important because the object type and its serialized representation become part of the object's identity.

The hashing implementation uses OpenSSL's cryptographic library.

---

# Object Model

Mini Git represents repository objects using a common object abstraction.

```text
Object
  │
  ├── Blob
  │
  ├── Tree
  │
  └── Commit
```

The base `Object` interface requires:

```cpp
virtual std::string serialize() const = 0;
```

This creates a common object pipeline:

```text
Object
   │
   ▼
serialize()
   │
   ▼
Serialized Bytes
   │
   ▼
SHA-256
   │
   ▼
Object ID
```

The `ObjectDatabase` operates on this common abstraction and therefore does not need to know the internal implementation details of individual object types.

---

# Blob

A Blob represents file contents.

```text
File
 │
 ▼
Blob
 │
 └── file contents
```

A Blob does not store the original filename.

For example:

```text
main.cpp
```

may contain:

```cpp
int main() {
    return 0;
}
```

The Blob represents the content itself.

The filename is stored separately by structures such as Trees and Index entries.

This separation means that identical contents can result in the same Blob object even when referenced by different paths.

---

# FileReader

`FileReader` separates filesystem input from object representation.

Its interface is:

```cpp
class FileReader {
public:
    static std::string read(
        const std::filesystem::path& path
    );
};
```

Files are opened in binary mode:

```cpp
std::ifstream file(path, std::ios::binary);
```

This allows Mini Git to preserve arbitrary byte sequences rather than assuming files contain text.

For example:

```text
00 01 7F FF
```

must be preserved exactly.

The resulting byte sequence is stored in a `std::string`.

Although `std::string` is commonly used for text, it can contain arbitrary bytes, including null bytes.

Therefore:

```text
Filesystem File
      │
      ▼
  FileReader
      │
      ▼
Exact Bytes
```

No intentional text conversion is performed.

---

# Blob::from_file()

A Blob can be created directly from a filesystem file through:

```cpp
Blob::from_file(path)
```

The flow is:

```text
Blob::from_file()
       │
       ▼
FileReader::read()
       │
       ▼
File Contents
       │
       ▼
Blob
```

This keeps filesystem access separate from object representation.

`FileReader` handles reading.

`Blob` handles representing the resulting contents as an object.

---

# Blob Serialization

Mini Git currently uses the following simplified Blob serialization:

```text
blob <size>\0<content>
```

For example:

```text
Hello Mini Git!
```

contains 15 bytes.

The serialized representation is conceptually:

```text
blob 15\0Hello Mini Git!
```

The `\0` represents an actual null byte.

The size is calculated from the byte count:

```cpp
content_.size()
```

The serialized representation is what gets hashed to generate the object ID.

The format is intentionally inspired by Git's object model but is **not Git-compatible**.

---

# Tree

A Tree represents a directory-like structure.

Each Tree entry currently contains:

```text
name
object_id
is_tree
```

Conceptually:

```text
Tree
├── main.cpp  → Blob
├── README.md → Blob
└── src       → Tree
```

The filename belongs to the Tree entry rather than the Blob.

This separates file content from filesystem naming and structure.

---

# Tree Serialization

Mini Git serializes Tree entries using:

```text
blob <object-id> <name>
tree <object-id> <name>
```

Each entry occupies one line.

For example:

```text
blob aaa README.md
tree bbb src
blob zzz main.cpp
```

Entries are sorted by name before serialization.

This ensures deterministic Tree identity regardless of filesystem traversal order.

The process is:

```text
Directory Entries
       │
       ▼
   Sort by Name
       │
       ▼
Deterministic Serialization
       │
       ▼
     SHA-256
       │
       ▼
    Tree ID
```

Therefore, inserting the same entries in different orders produces the same serialized Tree.

---

# TreeBuilder

`TreeBuilder` converts actual filesystem directories into Tree objects.

Its responsibility is to connect the working tree to the object model.

```text
Filesystem Directory
        │
        ▼
    TreeBuilder
        │
   ┌────┴────┐
   ▼         ▼
 File     Directory
   │         │
   ▼         ▼
 Blob    Recursive TreeBuilder
   │         │
   └────┬────┘
        ▼
      Tree
        │
        ▼
 ObjectDatabase
```

`TreeBuilder` receives an `ObjectDatabase` reference.

It constructs relationships between Blobs and Trees, while the Object Database handles persistence.

This separation keeps filesystem traversal independent from object storage.

---

# Recursive Tree Construction

Consider:

```text
project/
├── main.cpp
└── src/
    ├── App.cpp
    └── Utils.cpp
```

The resulting object graph is:

```text
Root Tree
├── main.cpp → Blob
└── src → Tree
           ├── App.cpp → Blob
           └── Utils.cpp → Blob
```

The recursive process is:

```text
build(project/)
    │
    ├── main.cpp
    │      │
    │      ▼
    │     Blob
    │
    └── src/
           │
           ▼
      build(src/)
           │
           ├── App.cpp → Blob
           └── Utils.cpp → Blob
           │
           ▼
          Tree
           │
           ▼
       Parent Tree
```

Every generated Blob and Tree is stored in the Object Database.

The resulting object IDs are used to establish relationships between parent and child Trees.

---

# Empty Directories

Empty directories are represented by empty Tree objects.

For:

```text
project/
└── empty/
```

Mini Git produces:

```text
Root Tree
└── empty → Tree
```

The `empty` Tree contains no entries but is still stored in the Object Database.

This demonstrates that Trees represent directory structure independently from whether a directory contains files.

---

# `.mini-git` Exclusion

The `.mini-git` directory contains repository metadata and must not become part of the repository's own working-tree snapshot.

`TreeBuilder` therefore explicitly excludes:

```text
.mini-git/
```

during recursive traversal.

Without this exclusion, the root Tree could recursively include the repository's own object database.

Conceptually:

```text
.mini-git/
    │
    ▼
   Tree
    │
    ▼
Object Database
    │
    ▼
.mini-git/
```

This would create an undesirable self-reference.

Mini Git prevents this by skipping `.mini-git` during Tree construction.

---

# Object Database

The `ObjectDatabase` provides persistent storage for serialized objects.

Its responsibilities are:

* storing serialized objects
* generating object identifiers
* checking whether objects exist
* retrieving serialized objects
* avoiding duplicate storage

The generic pipeline is:

```text
Object
   │
   ▼
serialize()
   │
   ▼
Serialized Data
   │
   ▼
SHA-256
   │
   ▼
Object ID
   │
   ▼
ObjectDatabase
   │
   ▼
Persistent Object
```

The Object Database does not need to know whether the object is a Blob, Tree, or Commit.

---

# Object Storage Layout

Mini Git currently uses a simplified flat object database:

```text
.mini-git/
└── objects/
    ├── <object-id-1>
    ├── <object-id-2>
    └── <object-id-3>
```

Each object is stored in a file named using its object ID.

The file contains the serialized object data.

This differs from Git's production object storage layout, which uses additional directory structure.

The flat layout is an intentional educational simplification.

---

# ObjectDatabase::store()

The storage process is:

```text
Object
   │
   ▼
serialize()
   │
   ▼
SHA-256
   │
   ▼
Object ID
   │
   ▼
Check Existing Object
   │
   ├── exists → reuse ID
   │
   └── missing
         │
         ▼
    Write Object
```

If an object with the same ID already exists, Mini Git reuses it.

For example:

```text
Blob A
"Hello Mini Git!"
      │
      ▼
 Object X


Blob B
"Hello Mini Git!"
      │
      ▼
 Object X
```

Only one copy is stored.

This demonstrates content-addressable storage and object deduplication.

---

# Object Retrieval

Objects can be retrieved using their object IDs.

```text
Object ID
    │
    ▼
ObjectDatabase::read()
    │
    ▼
.mini-git/objects/<id>
    │
    ▼
Serialized Object Data
```

The current `read()` operation returns serialized bytes.

Deserialization into concrete Blob, Tree, or Commit objects will be introduced as the object system becomes more complete.

---

# Object Existence

The Object Database provides:

```cpp
database.exists(object_id);
```

Conceptually:

```text
Object ID
    │
    ▼
Check Object Path
    │
    ├── exists → true
    │
    └── missing → false
```

This supports duplicate-object detection and repository operations that need to verify object presence.

---

# Hash-File

Mini Git retains the educational command:

```bash
mini-git hash-file <file>
```

It demonstrates:

```text
File
 │
 ▼
Blob
 │
 ▼
Serialization
 │
 ▼
SHA-256
 │
 ▼
Object ID
```

Unlike `hash-object`, it is primarily a hashing/debugging interface and does not represent the main persistent object-storage workflow.

---

# Hash-Object

Mini Git also provides:

```bash
mini-git hash-object <file>
```

Its workflow is:

```text
File
 │
 ▼
Blob::from_file()
 │
 ▼
Blob
 │
 ▼
ObjectDatabase::store()
 │
 ▼
Object ID
 │
 ▼
.mini-git/objects/<object-id>
```

Running the command again without changing the file produces the same object ID and reuses the existing object.

Changing the file contents produces a different object ID.

---

# Index / Staging Area

The Index is the staging area between the working tree and future repository snapshots.

The current conceptual workflow is:

```text
Working Tree
      │
      │ mini-git add
      ▼
    Index
      │
      │ future commit
      ▼
 Repository Snapshot
```

The Index records which object version is staged for each path.

For example:

```text
Index
├── main.cpp    → Blob A
├── README.md   → Blob B
└── src/App.cpp → Blob C
```

The current representation is:

```cpp
struct IndexEntry {
    std::string path;
    std::string object_id;
};
```

Therefore:

```text
path → object_id
```

is the central data model of the Index.

---

# Index Responsibilities

The `Index` class is responsible for:

* storing staged paths
* associating paths with object IDs
* updating existing staged paths
* checking whether a path is staged
* exposing staged entries
* saving the index
* loading the index

The Index does **not**:

* read files
* calculate hashes
* create Blobs
* store objects
* construct Trees
* create commits

Those responsibilities belong to other components.

The separation is:

```text
FileReader
    → reads file bytes

Blob
    → represents file contents

Hash
    → generates object IDs

ObjectDatabase
    → stores serialized objects

Index
    → records staged path → object ID
```

---

# Index Persistence

The Index is persisted at:

```text
.mini-git/index
```

The current format is intentionally simple:

```text
path<TAB>object_id
```

For example:

```text
main.cpp    abc123...
README.md   def456...
src/App.cpp ghi789...
```

The format is **not compatible with Git's binary index format**.

The simplified text format is intentional because it makes the staging mechanism easy to inspect during development.

---

# Index::add()

`Index::add()` inserts a new entry or updates an existing path.

Conceptually:

```text
Index::add(path, object_id)
        │
        ▼
Search existing entries
        │
   ┌────┴────┐
   ▼         ▼
 found     missing
   │         │
   ▼         ▼
 update    insert
```

For example:

```text
Before:

main.cpp  → Blob A
README.md → Blob B
```

After:

```text
add main.cpp → Blob C
```

the Index becomes:

```text
main.cpp  → Blob C
README.md → Blob B
```

Only one current staged entry exists for each path.

---

# Index Persistence Flow

Saving:

```text
Index in Memory
      │
      ▼
   save()
      │
      ▼
.mini-git/index
```

Loading:

```text
.mini-git/index
      │
      ▼
   load()
      │
      ▼
Index in Memory
```

Therefore staged information survives after Mini Git exits.

---

# Add Command

Mini Git currently supports:

```bash
mini-git add <file>
```

The complete staging pipeline is:

```text
Working Tree
     │
     ▼
    File
     │
     ▼
 FileReader
     │
     ▼
   Blob
     │
     ▼
ObjectDatabase
     │
     ▼
 Object ID
     │
     ▼
   Index
     │
     ▼
.mini-git/index
```

The command therefore connects four major subsystems:

```text
Filesystem
    ↓
Blob
    ↓
Object Database
    ↓
Index
```

---

# Add and Object Immutability

When a file is staged, its current contents are stored as a Blob.

Suppose:

```text
main.cpp
Version A
```

is staged:

```text
main.cpp → Blob A
```

After changing the file:

```text
main.cpp
Version B
```

and staging again:

```text
main.cpp → Blob B
```

The old Blob remains in the Object Database.

This is possible because stored objects are immutable:

```text
Version A
    │
    ▼
  Blob A
    │
    ▼
  Object Database


Version B
    │
    ▼
  Blob B
    │
    ▼
  Object Database
```

The Index simply changes which object ID is associated with the path.

---

# Status

Phase 9 introduces the `Status` subsystem.

Status examines the relationship between the current working tree and the Index.

At this stage, commits and HEAD-aware comparison do not yet exist, so the implemented model is:

```text
Index
  │
  │ compare
  ▼
Working Tree
```

The `Status` subsystem can currently identify:

* modified tracked files
* deleted tracked files
* untracked files
* nested untracked files
* a clean working tree

The current `StatusResult` contains:

```cpp
struct StatusResult {
    std::vector<std::string> modified;
    std::vector<std::string> deleted;
    std::vector<std::string> untracked;
};
```

---

# Status Detection

For each Index entry, Mini Git examines the corresponding working-tree path.

The comparison is:

```text
Index Entry
     │
     ▼
Read current file
     │
     ▼
Create Blob
     │
     ▼
Serialize Blob
     │
     ▼
SHA-256
     │
     ▼
Current Object ID
     │
     ▼
Compare with Index Object ID
```

If the IDs differ:

```text
modified
```

If the indexed file no longer exists:

```text
deleted
```

This means status uses the same Blob serialization and hashing pipeline as object creation.

---

# Untracked Files

Status recursively scans the working tree for regular files.

For each file:

```text
Working Tree File
       │
       ▼
Repository-relative path
       │
       ▼
Is path in Index?
       │
   ┌───┴───┐
   ▼       ▼
  yes      no
   │       │
   ▼       ▼
tracked  untracked
```

Nested files are also detected.

For example:

```text
project/
├── main.cpp
└── src/
    └── App.cpp
```

If neither file is staged, status can report:

```text
main.cpp
src/App.cpp
```

---

# `.mini-git` and Status

The repository's internal `.mini-git` directory must not be reported as an untracked project directory.

Status therefore excludes `.mini-git` from recursive traversal.

Importantly, merely ignoring the `.mini-git` directory entry is not sufficient because a recursive iterator may continue descending into it.

The implementation disables recursion when `.mini-git` is encountered.

Conceptually:

```text
Encounter .mini-git
       │
       ▼
Disable recursion
       │
       ▼
Skip repository metadata
```

This prevents internal files such as:

```text
.mini-git/index
.mini-git/HEAD
.mini-git/objects/...
```

from appearing as untracked project files.

---

# Status Command

Mini Git currently supports:

```bash
mini-git status
```

The current command reports:

```text
On branch main

Changes not staged for commit:
  modified: main.cpp

Deleted files:
  deleted: old.cpp

Untracked files:
  notes.txt
  src/App.cpp
```

If no changes are detected:

```text
On branch main

Working tree clean.
```

The current branch name is displayed as `main` because the repository initializes `HEAD` to `refs/heads/main`.

However, branch management and commit-aware branch state are not yet implemented.

---

# Current Status Model

The current implementation should be understood as:

```text
                Index
                  │
          compare │
                  ▼
            Working Tree
                  │
        ┌─────────┼─────────┐
        ▼         ▼         ▼
    modified    deleted  untracked
```

It does **not yet** implement the complete Git model:

```text
HEAD
 │
 ▼
Index
 │
 ▼
Working Tree
```

Specifically, the current implementation cannot yet report:

* staged changes relative to HEAD
* changes between HEAD and Index
* changes between HEAD and Working Tree
* commit-aware branch state

These capabilities depend on the Commit and Reference systems and will be introduced later.

---

# Index and Tree Relationship

The Index and Tree represent different concepts.

The Index represents staged path-to-object mappings:

```text
Index
├── main.cpp    → Blob A
├── README.md   → Blob B
└── src/App.cpp → Blob C
```

A Tree represents hierarchical directory structure:

```text
Root Tree
├── main.cpp    → Blob A
├── README.md   → Blob B
└── src          → Tree
                   └── App.cpp → Blob C
```

Therefore:

```text
Index
  │
  ▼
Tree Construction
  │
  ▼
Root Tree
  │
  ▼
Commit
```

The Index-to-Tree snapshot conversion will be implemented as part of the commit workflow.

---

# Commit

A Commit represents a repository snapshot.

The current Commit object contains:

```text
tree
parent
author
message
```

Conceptually:

```text
Commit
├── tree
├── parent
├── author
└── message
```

A commit points to a Tree.

The Tree points to Blobs and other Trees.

Therefore:

```text
Commit
   │
   ▼
 Tree
   │
   ├── Blob
   ├── Blob
   └── Tree
        ├── Blob
        └── Blob
```

A commit can also reference a previous commit:

```text
Commit C
   │
   └── parent → Commit B
                    │
                    └── parent → Commit A
```

The Commit class and serialization are implemented.

However, the repository-level commit command is not yet implemented.

The future workflow is:

```text
Index
  │
  ▼
Tree
  │
  ▼
Commit
  │
  ▼
Reference
```

---

# HEAD

During repository initialization, Mini Git creates:

```text
.mini-git/HEAD
```

containing:

```text
ref: refs/heads/main
```

Conceptually:

```text
HEAD
 │
 ▼
refs/heads/main
 │
 ▼
Commit
```

Currently, the final relationship does not yet exist because commits and references have not been implemented at the repository level.

---

# References

References provide human-readable names for commits.

The planned structure is:

```text
.mini-git/
└── refs/
    └── heads/
        ├── main
        ├── feature-a
        └── feature-b
```

A branch reference will eventually contain a commit object ID.

For example:

```text
main
 │
 ▼
Commit C
 │
 ▼
Commit B
 │
 ▼
Commit A
```

Reference management is planned for later phases.

---

# Commit History

Once commits and references are implemented, history will form a directed graph.

Linear history:

```text
A ← B ← C
          ↑
         main
```

Branching history:

```text
        B ← C
       /
A ←───
       \
        D ← E
             ↑
           feature
```

The branch reference points to the latest commit on that branch.

History traversal and visualization will be introduced later.

---

# Testing Architecture

Mini Git uses automated tests for individual components and their interactions.

## Hash Tests

`HashTests.cpp` covers:

* empty input
* known SHA-256 values
* deterministic hashing
* different inputs
* binary data

## Object Tests

`ObjectTests.cpp` covers:

* Blob serialization
* Tree serialization
* deterministic Tree serialization
* Commit serialization
* initial Commit serialization
* Object Database storage
* object existence
* object retrieval
* duplicate-object detection

## FileReader Tests

`FileReaderTests.cpp` covers:

* text-file reading
* binary-file reading
* null-byte preservation
* missing-file errors

## Blob Tests

`BlobTests.cpp` covers:

* Blob creation from files
* Blob serialization
* binary-file Blob handling

## TreeBuilder Tests

`TreeBuilderTests.cpp` covers:

* directory Tree construction
* file-to-Blob conversion
* nested directory traversal
* nested Tree creation
* empty directory handling
* `.mini-git` exclusion

## Index Tests

`IndexTests.cpp` covers:

* adding Index entries
* updating existing entries
* multiple staged entries
* Index persistence
* update persistence

## Status Tests

`StatusTests.cpp` covers:

* clean working trees
* modified tracked files
* deleted tracked files
* untracked files
* nested untracked files
* `.mini-git` exclusion

The tests are registered with CTest.

The complete suite can be executed with:

```bash
ctest --test-dir build --output-on-failure
```

Filesystem-based tests use isolated temporary files and directories where appropriate.

---

# Component Responsibilities

The current responsibilities are:

```text
Repository
    → manages repository-level structure

FileReader
    → reads exact filesystem bytes

Hash
    → calculates SHA-256 identifiers

Object
    → defines common object serialization

Blob
    → represents file contents

Tree
    → represents directory/object relationships

Commit
    → represents snapshot metadata

ObjectDatabase
    → stores and retrieves serialized objects

TreeBuilder
    → converts filesystem directories into Trees

Index
    → records staged path → object ID mappings

Status
    → compares Index state with the Working Tree
```

Keeping these responsibilities separate prevents individual components from becoming unnecessarily complex.

---

# Current Data Flow

The main currently implemented object pipeline is:

```text
                    Working Tree
                         │
                         ▼
                    TreeBuilder
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
            File                Directory
              │                     │
              ▼                     ▼
         FileReader             TreeBuilder
              │                     │
              ▼                     ▼
            Blob                    Tree
              │                     │
              └──────────┬──────────┘
                         ▼
                  ObjectDatabase
                         │
                         ▼
                  .mini-git/objects/
```

For individual staging:

```text
Working Tree
     │
     ▼
   File
     │
     ▼
FileReader
     │
     ▼
   Blob
     │
     ▼
serialize()
     │
     ▼
 SHA-256
     │
     ▼
Object ID
     │
     ▼
ObjectDatabase
     │
     ▼
   Index
     │
     ▼
.mini-git/index
```

Status uses:

```text
Index
  │
  │ compare
  ▼
Working Tree
  │
  ├── modified
  ├── deleted
  └── untracked
```

The future repository flow is:

```text
Working Tree
      │
      ▼
    Index
      │
      ▼
    Tree
      │
      ▼
   Commit
      │
      ▼
 Reference
      │
      ▼
    HEAD
```

---

# Content-Addressable Storage

Mini Git identifies objects using their serialized contents.

The general process is:

```text
Object
  │
  ▼
Serialize
  │
  ▼
SHA-256
  │
  ▼
Object ID
```

This applies to different object types:

```text
Blob
 │
 ▼
Serialized Blob
 │
 ▼
SHA-256
 │
 ▼
Blob ID
```

and:

```text
Tree
 │
 ▼
Serialized Tree
 │
 ▼
SHA-256
 │
 ▼
Tree ID
```

Therefore:

```text
same serialized object
        ↓
same SHA-256
        ↓
same object ID
```

This enables deterministic identity and object deduplication.

---

# Design Principles

## 1. Separation of Responsibilities

Each subsystem should have a focused responsibility.

```text
FileReader
    → reads files

Blob
    → represents file contents

Tree
    → represents directory structure

TreeBuilder
    → converts filesystem directories into Trees

Hash
    → calculates object IDs

ObjectDatabase
    → stores serialized objects

Index
    → records staged paths and object IDs

Status
    → analyzes Working Tree versus Index

Repository
    → manages repository structure
```

Components should not unnecessarily duplicate one another's responsibilities.

---

## 2. Deterministic Behavior

Identical inputs should produce identical results.

For example:

```text
same serialized object
        ↓
same SHA-256
        ↓
same object ID
```

Trees explicitly sort entries before serialization.

Therefore:

```text
same directory contents
        ↓
same Tree entries
        ↓
same sorted serialization
        ↓
same Tree ID
```

Determinism is fundamental to content-addressable storage.

---

## 3. Binary Safety

Filesystem data must be treated as bytes rather than assuming everything is text.

Binary-safe behavior applies to:

* FileReader
* Blob serialization
* ObjectDatabase storage
* ObjectDatabase retrieval

---

## 4. Testability

Core functionality should be independently testable.

Examples include:

```text
Hash
FileReader
Blob
Tree
Commit
ObjectDatabase
TreeBuilder
Index
Status
```

Filesystem tests use isolated temporary resources where appropriate.

---

## 5. Incremental Development

Mini Git is implemented subsystem by subsystem.

The current progression is:

```text
Files
  ↓
Blobs
  ↓
Object Database
  ↓
Trees
  ↓
Index
  ↓
Status
  ↓
Commits
  ↓
References
  ↓
Branches
```

Each phase builds on previously established concepts.

---

## 6. Explicit Simplification

Differences from real Git are intentional and documented.

Current simplifications include:

* SHA-256 as the project's chosen hashing algorithm
* simplified object serialization
* flat object storage
* simplified Tree representation
* simplified Commit representation
* simplified text-based Index
* simplified repository layout
* educational recursive Tree construction
* no Git-compatible repository format

The goal is understanding rather than reproducing Git's complete implementation.

---

# Current Limitations

The current implementation still has several intentional limitations.

### Repository discovery

Commands currently assume execution from the repository root.

Nested-directory repository discovery is not implemented.

### Index paths

The initial Index implementation uses a simplified path representation.

Repository-relative path normalization will be improved in later robustness work.

### Paths with special characters

The current text-based Index parser uses whitespace-based extraction.

Therefore paths containing whitespace or certain special characters are not yet handled robustly.

### `add .`

The following command is not yet implemented:

```bash
mini-git add .
```

### Deletion staging

Deleted files can be detected by Status, but deletion staging is not yet implemented.

### HEAD-aware status

Status currently compares:

```text
Index ↔ Working Tree
```

Full Git-like status requires:

```text
HEAD ↔ Index ↔ Working Tree
```

This will be implemented after commits and references exist.

---

# Educational Tools

The project plans to include educational commands that expose internal behavior:

```text
mini-git inspect
mini-git graph
mini-git explain
mini-git stats
mini-git fsck
```

For example:

```bash
mini-git explain add main.cpp
```

could eventually show:

```text
Working Tree
     │
     ▼
Read file
     │
     ▼
Create Blob
     │
     ▼
Serialize Blob
     │
     ▼
Calculate SHA-256
     │
     ▼
Store Object
     │
     ▼
Update Index
```

These commands are intended to make the internal architecture visible rather than hiding it behind the CLI.

---

# Phase Roadmap

```text
Phase 0
Understand Git

Phase 1
Project Setup

Phase 2
Repository Initialization

Phase 3
Hashing

Phase 4
Object Model

Phase 5
Blob Objects and File Integration

Phase 6
Object Database

Phase 7
Trees

Phase 8
Index / Staging

Phase 9
Status

Phase 10
Commits

Phase 11
Log

Phase 12
HEAD and References

Phase 13
Branches

Phase 14
Checkout

Phase 15
Diff

Phase 16
Merge

Phase 17
Conflict Handling

Phase 18
Tags

Phase 19
Garbage Collection

Phase 20
Comprehensive Testing

Phase 21
Robustness and Error Handling

Phase 22
Performance

Phase 23
Architecture and C++ Refactoring

Phase 24
Documentation

Phase 25
Mini Git vs Real Git

Phase 26
Portfolio and Interview Preparation
```

---

# Current Implementation Status

The implementation status at the end of Phase 9 is:

| Component                        | Status          |
| -------------------------------- | --------------- |
| Project structure                | Implemented     |
| CMake build system               | Implemented     |
| Git/GitHub integration           | Implemented     |
| Repository initialization        | Implemented     |
| `.mini-git/` directory           | Implemented     |
| `HEAD` initialization            | Implemented     |
| SHA-256 hashing                  | Implemented     |
| Object abstraction               | Implemented     |
| Blob                             | Implemented     |
| Tree                             | Implemented     |
| Commit                           | Implemented     |
| FileReader                       | Implemented     |
| Binary-safe file reading         | Implemented     |
| `Blob::from_file()`              | Implemented     |
| Blob serialization               | Implemented     |
| File → Blob pipeline             | Implemented     |
| File → Object ID pipeline        | Implemented     |
| ObjectDatabase                   | Implemented     |
| Persistent object storage        | Implemented     |
| Object retrieval                 | Implemented     |
| Object existence checking        | Implemented     |
| Duplicate-object detection       | Implemented     |
| `hash-file`                      | Implemented     |
| `hash-object`                    | Implemented     |
| Deterministic Tree serialization | Implemented     |
| TreeBuilder                      | Implemented     |
| Recursive directory traversal    | Implemented     |
| Directory → Tree conversion      | Implemented     |
| Nested Trees                     | Implemented     |
| Empty-directory Trees            | Implemented     |
| `.mini-git` exclusion            | Implemented     |
| Index                            | Implemented     |
| Index entries                    | Implemented     |
| Index add/update                 | Implemented     |
| Index persistence                | Implemented     |
| Index loading                    | Implemented     |
| Index saving                     | Implemented     |
| `add <file>`                     | Implemented     |
| Status subsystem                 | Implemented     |
| Modified-file detection          | Implemented     |
| Deleted-file detection           | Implemented     |
| Untracked-file detection         | Implemented     |
| Nested untracked-file detection  | Implemented     |
| `.mini-git` exclusion in Status  | Implemented     |
| Clean working-tree detection     | Implemented     |
| Hash tests                       | Implemented     |
| Object tests                     | Implemented     |
| FileReader tests                 | Implemented     |
| Blob tests                       | Implemented     |
| TreeBuilder tests                | Implemented     |
| Index tests                      | Implemented     |
| Status tests                     | Implemented     |
| Repository discovery             | Not implemented |
| `add .`                          | Not implemented |
| Deletion staging                 | Not implemented |
| HEAD-aware status                | Not implemented |
| Index → Tree snapshot            | Not implemented |
| Real commit command              | Not implemented |
| `log`                            | Not implemented |
| Reference management             | Not implemented |
| Branch management                | Not implemented |
| Checkout                         | Not implemented |
| Diff                             | Not implemented |
| Merge                            | Not implemented |
| Conflict handling                | Not implemented |
| Tags                             | Not implemented |
| Garbage collection               | Not implemented |
| Repository integrity checking    | Not implemented |
| Performance benchmarking         | Not implemented |

---

# End of Phase 9

At the end of Phase 9, Mini Git has progressed from repository initialization and object storage to a system capable of maintaining staged state and analyzing changes in the working tree.

The major implemented pipeline is:

```text
                     Working Tree
                          │
             ┌────────────┼────────────┐
             │            │            │
             ▼            ▼            ▼
         FileReader   TreeBuilder    Status
             │            │            │
             ▼            ▼            │
           Blob          Tree          │
             │            │            │
             └──────┬─────┘            │
                    ▼                  │
             ObjectDatabase           │
                    │                  │
                    ▼                  ▼
             Object Storage       Index Comparison
                    │                  │
                    │           ┌──────┼──────┐
                    │           ▼      ▼      ▼
                    │       modified deleted untracked
                    │
                    ▼
              Object IDs
                    │
                    ▼
                  Index
                    │
                    ▼
             .mini-git/index
```

Mini Git can now:

* initialize a repository
* create repository metadata
* initialize `HEAD`
* read real files
* preserve binary file contents
* represent file contents as Blob objects
* serialize objects
* generate deterministic SHA-256 object identifiers
* persist serialized objects
* retrieve stored objects
* check whether objects exist
* detect and reuse duplicate objects
* construct Trees from directories
* recursively process nested directories
* create nested Tree objects
* preserve empty directories as Tree objects
* ignore `.mini-git` during Tree construction
* deterministically serialize Tree entries
* persist Trees
* create and persist an Index
* add paths to the Index
* update existing staged paths
* load and save the Index
* stage files with `mini-git add <file>`
* store staged file Blobs
* detect modified tracked files
* detect deleted tracked files
* detect untracked files
* detect nested untracked files
* ignore `.mini-git` during Status traversal
* report a clean working tree
* run automated tests for all implemented subsystems

The architecture now contains the important early Git concepts:

```text
Working Tree
      │
      ▼
     Blob
      │
      ▼
Object Database
      │
      ▼
   Object ID
      │
      ▼
    Index
      │
      ▼
    Status
```

The next major subsystem is **Phase 10 — Commits**.

Phase 10 will connect the existing systems:

```text
Index
  │
  ▼
Tree
  │
  ▼
Commit
  │
  ▼
Reference
```

This will introduce the first actual repository snapshots and allow the previously implemented Commit object to become part of a complete repository-level workflow.

Once commits exist, Status can eventually evolve from the current:

```text
Index ↔ Working Tree
```

model into the complete:

```text
HEAD
 │
 ▼
Index
 │
 ▼
Working Tree
```

model used by Git-like version control systems.
