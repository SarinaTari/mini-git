# Mini Git Architecture

## Overview

Mini Git is an educational version control system written in modern C++20.

The project demonstrates how a Git-like version control system can be constructed from fundamental systems concepts including:

* filesystem operations
* content-addressable storage
* cryptographic hashing
* repository management
* immutable objects
* object databases
* blobs
* trees
* staging
* working-tree analysis
* commits
* references
* HEAD
* commit history
* branches
* checkout
* diff
* merge
* repository integrity
* maintenance
* testing
* performance
* systems-oriented C++ design

Mini Git is **not intended to be a drop-in replacement for Git** and does not aim for compatibility with Git's complete internal format.

Instead, the project focuses on understanding:

* how version control systems are structured
* how content-addressable storage works
* how snapshots are represented
* how commits form history
* how references point to history
* how staging separates working state from committed state
* how branches are implemented
* how changes can be detected and compared
* how repository integrity can be verified

The implementation is intentionally incremental.

Each phase introduces a new subsystem, establishes tests around it, and builds on functionality implemented previously.

---

# 1. High-Level Architecture

The long-term architecture of Mini Git is:

```text
                              mini-git CLI
                                   │
                                   ▼
                            Command Layer
                                   │
        ┌──────────────────────────┼──────────────────────────┐
        │                          │                          │
        ▼                          ▼                          ▼
   Working Tree                  Index                   Repository
        │                          │                          │
        │                          │                          ├── HEAD
        │                          │                          ├── References
        │                          │                          ├── Branches
        │                          │                          └── Tags
        │                          │
        │                          ▼
        │                        Status
        │
        ├──────────────┐
        │              │
        ▼              ▼
      Blob           TreeBuilder
        │              │
        │              ▼
        │             Tree
        │              │
        └──────┬───────┘
               │
               ▼
        Object Database
               │
        ┌──────┼───────┐
        ▼      ▼       ▼
      Blob    Tree    Commit
                       │
                       ▼
                    History
                       │
                 ┌─────┴─────┐
                 ▼           ▼
              Branches      Tags
                 │
                 ▼
                HEAD
```

The architecture can also be viewed as a snapshot pipeline:

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

And history traversal follows:

```text
HEAD
 │
 ▼
Branch Reference
 │
 ▼
Latest Commit
 │
 ▼
Parent Commit
 │
 ▼
Parent Commit
 │
 ▼
...
```

This separation is fundamental to Mini Git.

---

# 2. Current Implementation

At the current stage, Mini Git has progressed through:

```text
Phase 0  — Git Concepts
Phase 1  — Project Setup
Phase 2  — Repository Initialization
Phase 3  — Hashing
Phase 4  — Object Model
Phase 5  — Blob Objects and File Integration
Phase 6  — Object Database
Phase 7  — Trees
Phase 8  — Index / Staging
Phase 9  — Status
Phase 10 — Commits
Phase 11 — Log / History
```

The project therefore already contains the core foundations required for a small content-addressable version control system.

The next major subsystem is:

```text
Phase 12 — HEAD and References
```

---

# 3. Repository Structure

The project currently follows a modular C++ structure:

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
│   ├── StatusTests.cpp
│   └── LogTests.cpp
│
└── docs/
    ├── architecture.md
    └── ...
```

The responsibilities are:

```text
include/
    Public class declarations and interfaces

src/
    Implementation

tests/
    Automated component and subsystem tests

docs/
    Architecture, design, testing, and educational documentation
```

---

# 4. Repository

The `Repository` class represents the repository-level structure of Mini Git.

Initializing a repository creates:

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

This establishes `main` as the initial branch reference.

The repository abstraction currently provides functionality for:

* identifying the repository metadata directory
* initializing repository metadata
* reading the current branch
* reading the commit referenced by the current branch
* updating branch references

The repository metadata is intentionally simplified compared with Git.

---

# 5. Working Tree

The Working Tree represents the actual files and directories being managed.

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

The Working Tree is the source of filesystem state for several subsystems:

```text
Working Tree
     │
     ├── file contents ──────────→ Blob
     │
     ├── directory structure ────→ Tree
     │
     └── current state ──────────→ Status
```

---

# 6. Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The `Hash` abstraction provides:

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

The same serialized data produces the same object ID:

```text
same serialized data
        ↓
same SHA-256
        ↓
same object ID
```

Different serialized data produces a different identifier.

Mini Git hashes the **serialized object representation**, not merely raw file contents.

This means object serialization is part of object identity.

The hashing implementation uses OpenSSL's EVP API.

---

# 7. Object Model

Mini Git uses a common abstraction for repository objects:

```text
Object
  │
  ├── Blob
  │
  ├── Tree
  │
  └── Commit
```

The base interface requires:

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

The `ObjectDatabase` works with the common `Object` interface and therefore does not need to know the internal representation of every object type.

---

# 8. Blob

A Blob represents file contents.

```text
File
 │
 ▼
Blob
 │
 └── exact file contents
```

A Blob does not contain the original filename.

For example:

```text
main.cpp
```

may reference a Blob containing:

```cpp
int main() {
    return 0;
}
```

The Blob represents only the content.

Names and paths are handled by structures such as Trees and the Index.

This separation allows identical content to be represented by the same object regardless of filename.

---

# 9. FileReader

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

This allows arbitrary byte sequences to be preserved.

For example:

```text
00 01 7F FF
```

must be read exactly as stored.

`std::string` is used as a byte container here; it can contain null bytes and other non-text data.

The flow is:

```text
Filesystem File
      │
      ▼
  FileReader
      │
      ▼
   Exact Bytes
```

---

# 10. Blob::from_file()

A Blob can be constructed directly from a filesystem path:

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

The responsibilities remain separate:

```text
FileReader
    → reads bytes

Blob
    → represents those bytes as an object
```

---

# 11. Blob Serialization

Mini Git currently uses the simplified representation:

```text
blob <size>\0<content>
```

For example:

```text
Hello Mini Git!
```

is serialized conceptually as:

```text
blob 15\0Hello Mini Git!
```

where `\0` represents an actual null byte.

The size is based on the byte count:

```cpp
content_.size()
```

The serialized representation is hashed to generate the Blob's object ID.

The format is intentionally inspired by Git's object model but is **not Git-compatible**.

---

# 12. Tree

A Tree represents directory structure.

Each entry contains:

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

The filename belongs to the Tree entry, not the Blob.

This separates:

```text
Content
```

from:

```text
Filesystem Structure
```

---

# 13. Tree Serialization

Mini Git serializes entries using:

```text
blob <object-id> <name>
tree <object-id> <name>
```

For example:

```text
blob aaa README.md
tree bbb src
blob zzz main.cpp
```

Entries are sorted by name before serialization.

Therefore:

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

This guarantees deterministic Tree identity independent of filesystem traversal order.

---

# 14. TreeBuilder

`TreeBuilder` converts directory structures into Tree objects.

Its responsibility is to connect filesystem structure to the object model.

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

It constructs relationships between:

* Blobs
* Trees
* directories
* object IDs

The Object Database remains responsible for persistence.

---

# 15. Recursive Tree Construction

Consider:

```text
project/

├── main.cpp
└── src/
    ├── App.cpp
    └── Utils.cpp
```

The object graph becomes:

```text
Root Tree
├── main.cpp → Blob
└── src → Tree
           ├── App.cpp → Blob
           └── Utils.cpp → Blob
```

The recursive algorithm is conceptually:

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
       Root Tree
```

Every generated Blob and Tree is stored in the Object Database.

---

# 16. Empty Directories

Mini Git represents empty directories using empty Tree objects.

For:

```text
project/

└── empty/
```

the representation becomes:

```text
Root Tree
└── empty → Tree
```

The empty Tree contains no entries but still receives an object ID.

This is an educational design choice.

---

# 17. `.mini-git` Exclusion

The `.mini-git` directory contains repository metadata.

It must never become part of the repository's own snapshot.

Therefore both Tree construction and Status traversal exclude:

```text
.mini-git/
```

Otherwise the repository could recursively attempt to include its own object database.

The conceptual problem would be:

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

Mini Git prevents this by stopping traversal into `.mini-git`.

---

# 18. Object Database

The `ObjectDatabase` provides persistent storage for serialized objects.

Its responsibilities are:

* storing serialized objects
* generating object identifiers
* checking object existence
* reading objects
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

The Object Database does not need to know whether an object is a Blob, Tree, or Commit.

---

# 19. Object Storage Layout

Mini Git currently uses a simplified flat object database:

```text
.mini-git/

└── objects/
    ├── <object-id-1>
    ├── <object-id-2>
    └── <object-id-3>
```

Each object is stored under its complete object ID.

The stored file contains the serialized object.

This differs from Git's production object storage layout.

The flat layout is intentional and exists to make content-addressable storage easier to understand.

---

# 20. Object Storage and Deduplication

The storage algorithm is:

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
Already exists?
   │
   ├── yes → reuse object
   │
   └── no
         │
         ▼
      write object
```

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

Only one object is stored.

This demonstrates content-addressable storage and object deduplication.

---

# 21. Object Retrieval

Objects can be loaded using their object IDs:

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

The Object Database returns the serialized representation.

Concrete objects can then be reconstructed using their corresponding deserialization logic.

Commit deserialization is already implemented for history traversal.

---

# 22. Index / Staging Area

The Index represents staged state between the Working Tree and future repository snapshots.

Conceptually:

```text
Working Tree
      │
      │ add
      ▼
    Index
      │
      │ commit
      ▼
 Repository Snapshot
```

An Index entry currently contains:

```cpp
struct IndexEntry {
    std::string path;
    std::string object_id;
};
```

The central relationship is:

```text
path → object_id
```

For example:

```text
Index

├── main.cpp    → Blob A
├── README.md   → Blob B
└── src/App.cpp → Blob C
```

---

# 23. Index Responsibilities

The Index is responsible for:

* storing staged paths
* associating paths with object IDs
* updating existing paths
* checking whether paths are staged
* exposing staged entries
* loading persisted state
* saving persisted state

The Index does **not**:

* read files
* calculate hashes
* construct Blobs
* store objects
* construct Trees
* create commits

Those responsibilities belong elsewhere.

The separation is:

```text
FileReader
    → reads file bytes

Blob
    → represents file contents

Hash
    → generates object IDs

ObjectDatabase
    → stores objects

Index
    → records staged path → object ID
```

---

# 24. Index Persistence

The Index is stored at:

```text
.mini-git/index
```

The current educational format is:

```text
path<TAB>object-id
```

For example:

```text
main.cpp    abc123...
README.md   def456...
src/App.cpp ghi789...
```

The format is intentionally simple and human-readable.

It is **not compatible with Git's binary index format**.

---

# 25. Add Command

Mini Git currently supports:

```bash
mini-git add <file>
```

The staging pipeline is:

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

This connects:

```text
Filesystem
    ↓
Blob
    ↓
Object Database
    ↓
Index
```

The current implementation stages individual files.

Recursive `add .` behavior is part of the future command/robustness work.

---

# 26. Status

Status currently compares:

```text
Index ↔ Working Tree
```

It can identify:

* modified tracked files
* deleted tracked files
* untracked files
* nested untracked files
* clean working trees

The result structure is:

```cpp
struct StatusResult {
    std::vector<std::string> modified;
    std::vector<std::string> deleted;
    std::vector<std::string> untracked;
};
```

The current status model intentionally does not yet provide complete Git-style:

```text
HEAD ↔ Index ↔ Working Tree
```

comparison.

That becomes possible once reference-aware repository state is fully established.

---

# 27. Status Detection

For each indexed file:

```text
Index Entry
     │
     ▼
Working Tree Path
     │
     ▼
Read File
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
Compare with Index ID
```

If the IDs differ:

```text
modified
```

If the file no longer exists:

```text
deleted
```

Untracked files are found by recursively scanning the Working Tree and checking whether their paths exist in the Index.

---

# 28. Commit Object

A Commit represents a repository snapshot and its metadata.

The current Commit model contains:

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

The Tree identifies the complete snapshot.

The parent identifies the previous commit.

The author and message describe the commit.

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

And history is represented by parent links:

```text
Commit C
   │
   └── parent → Commit B
                    │
                    └── parent → Commit A
```

---

# 29. Commit Serialization

A simplified Commit representation is:

```text
tree <tree-id>
parent <parent-id>
author <author>
<blank line>
<message>
```

The parent line is omitted for the initial commit.

For example:

```text
tree abc123
parent def456
author Mini Git User

Implement object database
```

Commit serialization is deterministic.

Commit deserialization is used by the history subsystem to reconstruct commit metadata from stored objects.

---

# 30. Commit History

The commit parent relationship forms a linked history.

A linear history looks like:

```text
A ← B ← C ← D
            ↑
          main
```

Each commit contains the ID of its parent.

This means history can be traversed without storing a separate history database.

The algorithm is:

```text
Start at HEAD commit
        │
        ▼
Read commit object
        │
        ▼
Display commit
        │
        ▼
Read parent ID
        │
        ▼
Repeat
        │
        ▼
No parent
        │
        ▼
Stop
```

This is the foundation of `mini-git log`.

---

# 31. HEAD

Mini Git initializes:

```text
.mini-git/HEAD
```

with:

```text
ref: refs/heads/main
```

Therefore HEAD is not normally the commit ID itself.

Instead:

```text
HEAD
 │
 ▼
refs/heads/main
 │
 ▼
Commit ID
 │
 ▼
Commit Object
```

This indirection is important because moving a branch only requires changing the branch reference.

The HEAD mechanism will be expanded further in the References and Branches phases.

---

# 32. Log

Mini Git currently supports:

```bash
mini-git log
```

The history traversal pipeline is:

```text
mini-git log
      │
      ▼
     HEAD
      │
      ▼
refs/heads/main
      │
      ▼
Latest Commit ID
      │
      ▼
ObjectDatabase
      │
      ▼
Commit::deserialize()
      │
      ▼
Display Commit
      │
      ▼
parent_id()
      │
      ▼
Previous Commit
      │
      ▼
Repeat
```

The command currently displays:

```text
commit <object-id>
Author: <author>

    <message>
```

History traversal continues until a commit without a parent is reached.

If the current branch has no commit yet, Mini Git reports:

```text
No commits yet.
```

---

# 33. Why Log Is Important

The `log` subsystem demonstrates an important property of content-addressed history:

A commit does not need to contain an entire copy of previous repository states.

Instead:

```text
Commit
  │
  ├── tree → snapshot
  │
  └── parent → previous snapshot
```

This creates a chain:

```text
Commit C
   │
   ▼
Commit B
   │
   ▼
Commit A
```

The object database stores each object independently.

The references determine where history traversal begins.

---

# 34. References

References provide stable names for commits.

The intended structure is:

```text
.mini-git/

├── HEAD
│
├── objects/
│
└── refs/
    └── heads/
        ├── main
        ├── feature-a
        └── feature-b
```

A branch reference contains a commit ID:

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

References allow the commit graph to move without modifying commits themselves.

Reference management is the next major architectural subsystem.

---

# 35. Branches

A branch is fundamentally a named reference to a commit.

Conceptually:

```text
main
 │
 ▼
Commit C
```

Creating another branch:

```text
main ───────→ C
               ↑
feature ───────┘
```

After new work:

```text
main ─────→ C

feature ──→ D
             │
             └── parent → C
```

Branches therefore do not require copying repository contents.

They are movable references into the commit graph.

Branch creation, switching, and management will be implemented in later phases.

---

# 36. Index → Tree Snapshot

The Index and Tree represent different layers.

The Index is flat:

```text
Index

main.cpp    → Blob A
README.md   → Blob B
src/App.cpp → Blob C
```

The Tree is hierarchical:

```text
Root Tree
├── main.cpp    → Blob A
├── README.md   → Blob B
└── src
    └── App.cpp → Blob C
```

The conversion is:

```text
Index
  │
  ▼
TreeBuilder::build_from_index()
  │
  ▼
Nested Trees
  │
  ▼
Root Tree
  │
  ▼
Tree Object ID
```

This is the bridge between staging and committed snapshots.

---

# 37. Complete Snapshot Pipeline

The core repository model is:

```text
                  Working Tree
                       │
                       ▼
                     Index
                       │
                       ▼
                TreeBuilder
                       │
                       ▼
                      Tree
                       │
                       ▼
                    Commit
                       │
                       ▼
                Branch Reference
                       │
                       ▼
                     HEAD
```

The reverse direction is used for inspection:

```text
HEAD
 │
 ▼
Branch
 │
 ▼
Commit
 │
 ▼
Tree
 │
 ├── Blob
 ├── Blob
 └── Tree
      └── Blob
```

This separation between mutable working state and immutable repository objects is one of the central architectural ideas of Mini Git.

---

# 38. Current Data Flow

The currently implemented foundations can be summarized as:

```text
                         Working Tree
                              │
              ┌───────────────┼───────────────┐
              │               │               │
              ▼               ▼               ▼
          FileReader      TreeBuilder       Status
              │               │               │
              ▼               ▼               │
            Blob             Tree             │
              │               │               │
              └───────┬───────┘               │
                      ▼                       │
               ObjectDatabase                │
                      │                       │
                      ▼                       ▼
               Object Storage          Index Comparison
                                              │
                                      ┌───────┼───────┐
                                      ▼       ▼       ▼
                                   modified deleted untracked
```

The repository/history path is:

```text
HEAD
 │
 ▼
Branch Reference
 │
 ▼
Commit
 │
 ▼
Parent Commit
 │
 ▼
Parent Commit
 │
 ▼
...
```

---

# 39. Content-Addressable Storage

Mini Git identifies objects from their serialized content:

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

For a Blob:

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

For a Tree:

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

For a Commit:

```text
Commit
 │
 ▼
Serialized Commit
 │
 ▼
SHA-256
 │
 ▼
Commit ID
```

Therefore:

```text
same serialized object
        ↓
same object ID
```

This provides deterministic identity and enables deduplication.

---

# 40. Immutability

Repository objects are conceptually immutable.

If a file changes:

```text
Version A
   │
   ▼
Blob A
```

and later:

```text
Version B
   │
   ▼
Blob B
```

the original Blob A remains unchanged.

The Index changes its path mapping:

```text
Before:

main.cpp → Blob A


After:

main.cpp → Blob B
```

Similarly, commits are not edited after creation.

Instead, new commits reference previous commits:

```text
A ← B ← C
```

This immutable-object model is fundamental to version control.

---

# 41. Design Principles

## 41.1 Separation of Responsibilities

Each subsystem has a focused responsibility.

```text
Repository
    → repository metadata and references

FileReader
    → filesystem byte input

Hash
    → SHA-256 calculation

Object
    → common object interface

Blob
    → file contents

Tree
    → directory/object relationships

Commit
    → snapshot metadata and history links

ObjectDatabase
    → persistent object storage

TreeBuilder
    → filesystem/index → Tree construction

Index
    → staged path → object mappings

Status
    → Working Tree analysis

Log
    → commit history traversal
```

---

## 41.2 Deterministic Behavior

Identical serialized objects must produce identical object IDs.

Trees therefore sort their entries before serialization.

```text
same entries
    ↓
same ordering
    ↓
same serialization
    ↓
same SHA-256
    ↓
same Tree ID
```

---

## 41.3 Binary Safety

Filesystem data is treated as bytes.

Binary safety applies to:

* FileReader
* Blob
* object serialization
* ObjectDatabase storage
* ObjectDatabase retrieval

---

## 41.4 Immutability

Stored objects should not be modified after creation.

Changes produce new objects.

This allows previous repository states to remain addressable.

---

## 41.5 Testability

Core components are independently testable.

The project uses CTest together with focused executable-based tests.

---

## 41.6 Incremental Development

Mini Git is intentionally implemented subsystem by subsystem.

The development direction is:

```text
Filesystem
    ↓
Hashing
    ↓
Objects
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
History
    ↓
References
    ↓
Branches
    ↓
Checkout
    ↓
Diff
    ↓
Merge
```

---

## 41.7 Explicit Simplification

Mini Git intentionally differs from real Git.

Current simplifications include:

* SHA-256 instead of Git's production hash model
* simplified object serialization
* flat object storage
* simplified Tree representation
* simplified Commit representation
* text-based Index
* simplified repository layout
* educational empty-directory handling
* simplified command implementation
* no Git repository compatibility requirement

These differences are intentional.

The goal is understanding the architecture rather than reproducing Git's complete production implementation.

---

# 42. Testing Architecture

Mini Git uses automated tests for individual components and larger subsystem behavior.

Current test areas include:

## Hash Tests

`HashTests.cpp`

Tests:

* empty input
* known SHA-256 values
* deterministic hashing
* different inputs
* binary data

## Object Tests

`ObjectTests.cpp`

Tests:

* Blob serialization
* Tree serialization
* deterministic Tree serialization
* Commit serialization
* Object Database storage
* object existence
* object retrieval
* duplicate-object behavior

## FileReader Tests

`FileReaderTests.cpp`

Tests:

* text files
* binary files
* null-byte preservation
* missing files

## Blob Tests

`BlobTests.cpp`

Tests:

* Blob construction
* file-based Blob creation
* serialization
* binary files

## TreeBuilder Tests

`TreeBuilderTests.cpp`

Tests:

* directory Tree construction
* file-to-Blob conversion
* nested directories
* nested Trees
* empty directories
* `.mini-git` exclusion

## Index Tests

`IndexTests.cpp`

Tests:

* adding entries
* updating entries
* multiple entries
* persistence
* loading
* saving

## Status Tests

`StatusTests.cpp`

Tests:

* clean trees
* modified files
* deleted files
* untracked files
* nested untracked files
* `.mini-git` exclusion

## Log Tests

`LogTests.cpp`

Tests:

* Commit deserialization
* initial commits without parents
* multiline commit messages
* serialization/deserialization round-trips
* storing and reading commits
* parent relationships
* history traversal

The complete test suite is executed with:

```bash
ctest --test-dir build --output-on-failure
```

Filesystem tests use isolated temporary directories whenever appropriate.

---

# 43. Component Responsibilities

The current architecture can be summarized as:

```text
Repository
    → manages repository metadata and references

FileReader
    → reads exact filesystem bytes

Hash
    → calculates SHA-256 identifiers

Object
    → defines the common object interface

Blob
    → represents file contents

Tree
    → represents directory/object relationships

Commit
    → represents repository snapshots and history links

ObjectDatabase
    → stores and retrieves serialized objects

TreeBuilder
    → constructs Trees

Index
    → stores staged path/object mappings

Status
    → compares Working Tree state against the Index

Log
    → traverses commit history
```

This separation prevents the CLI and individual components from becoming monolithic.

---

# 44. Current Limitations

The current implementation intentionally has several limitations.

## Repository Discovery

Commands currently assume execution from the repository root.

Repository discovery from nested directories will be added later.

---

## Path Normalization

The early Index implementation uses simplified path handling.

Repository-relative normalization will be strengthened during the robustness phases.

---

## Special Paths

The current text-based Index representation does not yet provide complete support for every possible filesystem path containing whitespace or special characters.

---

## `add .`

The recursive staging command:

```bash
mini-git add .
```

is planned for a later command/robustness phase.

---

## Deletion Staging

Status can detect deleted files, but deletion staging requires additional Index semantics.

---

## Full HEAD-Aware Status

The current Status implementation primarily compares:

```text
Index ↔ Working Tree
```

The mature model will compare:

```text
HEAD ↔ Index ↔ Working Tree
```

This will allow Mini Git to distinguish:

* committed changes
* staged changes
* unstaged changes
* untracked files

---

## Git Compatibility

Mini Git does not attempt to read or write Git's native repository format.

The project is Git-inspired rather than Git-compatible.

---

# 45. Educational Layer

One of Mini Git's long-term goals is to expose internal version-control behavior through educational commands.

Planned commands include:

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

could eventually display:

```text
Working Tree
     │
     ▼
Read File
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
Store Object
     │
     ▼
Update Index
```

The purpose is not merely to add extra commands.

The educational layer should expose the internal architecture that the normal CLI hides.

---

# 46. Future Architecture

The mature architecture is expected to become:

```text
                           mini-git CLI
                                │
                                ▼
                         Command Layer
                                │
        ┌───────────────────────┼────────────────────────┐
        │                       │                        │
        ▼                       ▼                        ▼
  Working Tree                Index                 Repository
        │                       │                        │
        │                       │                 ┌──────┼──────┐
        │                       │                 │      │      │
        │                       │                 ▼      ▼      ▼
        │                       │               HEAD  Branches Tags
        │                       │                 │
        │                       │                 ▼
        │                       │             References
        │                       │
        ▼                       ▼
      Status               Snapshot Builder
                                │
                                ▼
                               Tree
                                │
                                ▼
                         Object Database
                                │
                     ┌──────────┼──────────┐
                     ▼          ▼          ▼
                   Blob       Tree       Commit
                                           │
                                           ▼
                                        History
                                           │
                         ┌─────────────────┼────────────────┐
                         ▼                 ▼                ▼
                       Log              Graph             Diff
                                           │
                                           ▼
                                         Merge
                                           │
                                           ▼
                                  Conflict Resolution
```

Later subsystems such as:

* repository integrity
* garbage collection
* performance analysis
* educational inspection
* graph visualization

will build on this foundation.

---

# 47. Development Roadmap

The project follows a **21-phase engineering roadmap**.

```text
Phase 0  — Git Concepts and System Design
Phase 1  — Project Setup and C++ Architecture
Phase 2  — Repository Initialization
Phase 3  — Hashing and Object Identity
Phase 4  — Object Model
Phase 5  — Blob Objects and File Integration
Phase 6  — Object Database
Phase 7  — Trees and Tree Construction
Phase 8  — Index and Staging
Phase 9  — Status
Phase 10 — Commits
Phase 11 — Log and History
Phase 12 — HEAD and References
Phase 13 — Branches
Phase 14 — Checkout and Working-Tree Restoration
Phase 15 — Diff
Phase 16 — Merge
Phase 17 — Conflict Handling
Phase 18 — Tags and Repository Metadata
Phase 19 — Integrity, Maintenance, and Garbage Collection
Phase 20 — Testing, Robustness, Performance, and Portfolio Hardening
```

This roadmap is intentionally shorter and more engineering-focused than the earlier 26-phase plan.

Documentation, refactoring, Git comparison, educational tooling, and portfolio preparation are treated as **cross-cutting goals** rather than unnecessary standalone phases.

---

# 48. Phase 11 Completion

At the end of Phase 11, Mini Git has progressed from basic filesystem and object storage concepts to actual commit-history traversal.

The major pipeline is now:

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
Branch Reference
     │
     ▼
   HEAD
     │
     ▼
    Log
     │
     ▼
Parent Commit
     │
     ▼
Parent Commit
     │
     ▼
...
```

The project now understands both:

```text
How repository state is constructed
```

and:

```text
How repository history is traversed
```

This is a major architectural milestone.

---

# 49. Current Implementation Status

| Component                        | Status      |
| -------------------------------- | ----------- |
| Project structure                | Implemented |
| CMake build system               | Implemented |
| Git/GitHub integration           | Implemented |
| Repository initialization        | Implemented |
| `.mini-git/` directory           | Implemented |
| HEAD initialization              | Implemented |
| SHA-256 hashing                  | Implemented |
| Object abstraction               | Implemented |
| Blob                             | Implemented |
| Tree                             | Implemented |
| Commit object                    | Implemented |
| FileReader                       | Implemented |
| Binary-safe file reading         | Implemented |
| `Blob::from_file()`              | Implemented |
| Blob serialization               | Implemented |
| File → Blob pipeline             | Implemented |
| File → Object ID pipeline        | Implemented |
| ObjectDatabase                   | Implemented |
| Persistent object storage        | Implemented |
| Object retrieval                 | Implemented |
| Object existence checking        | Implemented |
| Duplicate-object detection       | Implemented |
| `hash-file`                      | Implemented |
| `hash-object`                    | Implemented |
| Deterministic Tree serialization | Implemented |
| TreeBuilder                      | Implemented |
| Recursive directory traversal    | Implemented |
| Directory → Tree conversion      | Implemented |
| Nested Trees                     | Implemented |
| Empty-directory Trees            | Implemented |
| `.mini-git` exclusion            | Implemented |
| Index                            | Implemented |
| Index entries                    | Implemented |
| Index add/update                 | Implemented |
| Index persistence                | Implemented |
| Index loading                    | Implemented |
| Index saving                     | Implemented |
| `add <file>`                     | Implemented |
| Status subsystem                 | Implemented |
| Modified-file detection          | Implemented |
| Deleted-file detection           | Implemented |
| Untracked-file detection         | Implemented |
| Nested untracked-file detection  | Implemented |
| `.mini-git` exclusion in Status  | Implemented |
| Clean working-tree detection     | Implemented |
| Commit serialization             | Implemented |
| Commit deserialization           | Implemented |
| Parent commit relationships      | Implemented |
| Commit history traversal         | Implemented |
| `log`                            | Implemented |
| Log tests                        | Implemented |
| Repository discovery             | Planned     |
| `add .`                          | Planned     |
| Deletion staging                 | Planned     |
| Complete HEAD-aware status       | Planned     |
| Full reference management        | Next        |
| Branch management                | Planned     |
| Checkout                         | Planned     |
| Diff                             | Planned     |
| Merge                            | Planned     |
| Conflict handling                | Planned     |
| Tags                             | Planned     |
| Repository integrity checking    | Planned     |
| Garbage collection               | Planned     |
| Performance work                 | Planned     |
| Educational/intelligence layer   | Planned     |

---

# 50. Architectural Milestone

Mini Git has now crossed three important levels.

### Level 1 — Content

```text
File
 ↓
Blob
 ↓
Object ID
```

### Level 2 — Structure

```text
Files
 ↓
Blobs
 ↓
Trees
 ↓
Snapshot
```

### Level 3 — History

```text
Snapshot
 ↓
Commit
 ↓
Parent
 ↓
History
 ↓
Reference
 ↓
HEAD
```

The project is therefore no longer simply a file-hashing utility.

It is becoming a real miniature version-control architecture.

---

# 51. Next Architectural Step

The immediate next subsystem is:

```text
Phase 12 — HEAD and References
```

The goal is to make references a first-class repository subsystem rather than having history logic interpret repository files directly.

The target architecture becomes:

```text
HEAD
 │
 ▼
Reference
 │
 ▼
Commit ID
 │
 ▼
Commit
 │
 ▼
Tree
```

This will establish the foundation required for:

```text
Branches
    ↓
Checkout
    ↓
Diff
    ↓
Merge
    ↓
Conflict Handling
```

The project should therefore continue in this order rather than jumping directly into merge or advanced Git behavior.

---

# 52. Final Architectural Goal

The final Mini Git architecture should conceptually provide:

```text
                         mini-git
                            │
                            ▼
                      Command Layer
                            │
       ┌────────────────────┼────────────────────┐
       │                    │                    │
       ▼                    ▼                    ▼
 Working Tree             Index              Repository
       │                    │                    │
       │                    │             ┌──────┼──────┐
       │                    │             ▼      ▼      ▼
       │                    │           HEAD  Branches Tags
       │                    │
       │                    ▼
       │               Snapshot Builder
       │                    │
       │                    ▼
       │                   Tree
       │                    │
       └────────────┬───────┘
                    ▼
             Object Database
                    │
          ┌─────────┼─────────┐
          ▼         ▼         ▼
        Blob       Tree     Commit
                              │
                              ▼
                           History
                              │
               ┌──────────────┼──────────────┐
               ▼              ▼              ▼
              Log            Diff           Graph
                                              │
                                              ▼
                                            Merge
                                              │
                                              ▼
                                     Conflict Handling
                                              │
                                              ▼
                                  Integrity / Maintenance
```

The final system should demonstrate the complete conceptual lifecycle:

```text
Create File
    ↓
Stage File
    ↓
Create Snapshot
    ↓
Create Commit
    ↓
Move Branch Reference
    ↓
Advance HEAD
    ↓
Create More Commits
    ↓
Inspect History
    ↓
Create Branch
    ↓
Checkout Branch
    ↓
Compare Changes
    ↓
Merge Histories
    ↓
Handle Conflicts
    ↓
Verify Repository Integrity
    ↓
Maintain Repository
```

Mini Git's primary purpose remains educational:

> **Build a version control system from first principles to understand how the pieces actually work.**

It is not designed to replace Git.

It is designed to make Git-like architecture understandable.
