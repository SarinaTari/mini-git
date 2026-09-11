# Mini Git

A **Git-inspired version control system built from scratch in C++20** to understand how modern version control systems work internally.

Mini Git is a serious systems-programming and software-engineering project focused on implementing the fundamental mechanisms behind version control from first principles.

The project progressively implements:

* Content-addressable storage
* SHA-256 object hashing
* Blob objects
* Tree objects
* Commit objects
* Object databases
* Serialization and deserialization
* Index / staging
* Repository state
* `HEAD`
* References
* Branches
* Commit history
* Checkout
* Diff
* Merge
* Conflict handling
* Repository integrity
* Educational repository introspection

Mini Git is **not intended to replace Git** and does not aim for full Git compatibility.

The goal is different:

> **Build a version control system from scratch to understand what is happening underneath Git commands.**

---

# Project Motivation

Git is extremely powerful, but most developers interact with it primarily through commands such as:

```bash
git add
git commit
git branch
git checkout
git merge
git log
git diff
```

Mini Git explores what actually happens behind those commands.

For example, staging a file conceptually becomes:

```text
                 Working Tree
                      │
                      ▼
                  FileReader
                      │
                      ▼
                    Blob
                      │
                      ▼
                  Serialize
                      │
                      ▼
                   SHA-256
                      │
                      ▼
                 Object ID
                      │
                      ▼
               Object Database
                      │
                      ▼
                    Index
```

A commit then builds on that:

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
Object Database
     │
     ▼
Branch Reference
     │
     ▼
    HEAD
```

Branches introduce multiple lines of development:

```text
                 Commit A
                 /      \
                /        \
             main       feature
              │             │
              ▼             ▼
           Commit B      Commit C
```

Diff introduces state comparison:

```text
Working Tree ───────┐
                    │
Index ──────────────┼──► Snapshot ──► Diff
                    │
Commit ─────────────┘
```

Mini Git is therefore both a practical version-control implementation and a way to study:

* filesystem internals
* data structures
* serialization
* hashing
* persistence
* trees
* graphs
* state management
* C++ architecture
* testing
* systems design
* algorithms

---

# Current Status

## Phase 14 — Diff & Change Inspection

Mini Git has completed the foundational repository, object, staging, commit, history, reference, branch, and checkout layers.

Phase 14 introduces the ability to compare repository states and display the actual content changes between them.

### Implemented

* C++20 project
* CMake build system
* Automated testing with CTest
* Repository initialization
* `.mini-git` repository structure
* SHA-256 hashing
* OpenSSL EVP hashing
* Blob objects
* Blob serialization
* Blob deserialization
* Tree objects
* Tree serialization
* Tree deserialization
* Commit objects
* Commit serialization
* Commit deserialization
* Content-addressable object storage
* Object retrieval
* Index / staging
* Tree construction from the Index
* Working Tree state
* Status infrastructure
* Commit creation
* Commit history
* References
* Symbolic `HEAD`
* Detached `HEAD` representation
* Branch creation
* Branch listing
* Branch reference updates
* Branch switching
* Checkout
* Recursive Tree restoration
* Working Tree restoration
* Diff subsystem
* Working Tree vs Index comparison
* Index vs `HEAD` comparison
* Commit vs Working Tree comparison
* Commit vs Commit comparison
* Added-file detection
* Deleted-file detection
* Modified-file detection
* Unchanged-file filtering
* Line-based diff generation
* Longest Common Subsequence based comparison
* Unified diff output
* Automated Diff tests

### Current Core Pipeline

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
Object Database
      │
      ▼
Branch Reference
      │
      ▼
    HEAD
```

### Current Diff Pipeline

```text
Working Tree
      │
      ▼
   Snapshot
      │
      │
Index ─────────► Snapshot
      │
      │
Commit ────────► Snapshot
      │
      ▼
Compare Snapshots
      │
      ▼
LCS-based Diff
      │
      ▼
Unified Output
```

---

# Goals

Mini Git is designed around several goals.

## 1. Understand Version Control Internals

The project should explain concepts that are normally hidden behind Git commands.

## 2. Build Everything Incrementally

Each subsystem is implemented independently and then integrated into the larger architecture.

## 3. Use Real Systems Concepts

The implementation uses:

* filesystem APIs
* persistent storage
* hashing
* serialization
* trees
* references
* graph traversal
* state comparison
* automated testing

## 4. Maintain Clean Architecture

Responsibilities should remain separated.

For example:

```text
Repository
    │
    ├── References
    ├── HEAD
    └── Working Tree

ObjectDatabase
    │
    ├── Blob
    ├── Tree
    └── Commit

Index
    │
    └── Staging State

Diff
    │
    └── State Comparison
```

## 5. Build Portfolio-Quality Engineering Experience

The project demonstrates:

* C++20
* CMake
* filesystem programming
* cryptographic hashing APIs
* persistence
* data modeling
* testing
* architecture
* Git concepts
* debugging
* documentation

---

# Core Concepts

## Working Tree

The Working Tree is the actual collection of files on disk.

Example:

```text
project/
├── main.cpp
├── README.md
└── src/
    └── parser.cpp
```

Mini Git does not store the Working Tree itself as a special object.

It reads the files from the filesystem.

---

# Index

The Index represents the staged state.

Conceptually:

```text
path → Blob ID
```

For example:

```text
main.cpp → a81f...
README.md → 8b23...
```

The Index allows Mini Git to distinguish between:

```text
Working Tree
```

and:

```text
Staged State
```

---

# Blob

A Blob stores file content.

For example:

```text
hello.txt

hello
world
```

becomes a Blob containing:

```text
hello
world
```

The serialized representation is hashed.

The resulting SHA-256 digest becomes the object's identity.

Conceptually:

```text
content
   │
   ▼
Blob serialization
   │
   ▼
SHA-256
   │
   ▼
Object ID
```

---

# Tree

A Tree represents a directory snapshot.

A Tree contains entries such as:

```text
blob <object-id> hello.txt
tree <object-id> src
```

Trees can therefore reference other Trees.

Example:

```text
Tree
├── hello.txt → Blob
├── main.cpp  → Blob
└── src        → Tree
                ├── parser.cpp → Blob
                └── lexer.cpp  → Blob
```

This produces a hierarchical snapshot.

---

# Commit

A Commit points to a Tree and optionally to a parent Commit.

Conceptually:

```text
Commit
 ├── tree
 ├── parent
 ├── author
 └── message
```

A history can therefore be traversed through parent relationships:

```text
Commit C
   │
   ▼
Commit B
   │
   ▼
Commit A
```

---

# Object Database

The Object Database stores immutable objects.

Conceptually:

```text
.mini-git/
└── objects/
    ├── ab/
    │   └── cdef...
    ├── 91/
    │   └── 2345...
    └── f8/
        └── 1234...
```

The object ID is derived from the object's serialized representation.

This gives Mini Git content-addressable storage.

---

# Content Addressing

Instead of assigning arbitrary IDs:

```text
object1
object2
object3
```

Mini Git derives the ID from content:

```text
serialized object
       │
       ▼
     SHA-256
       │
       ▼
    object ID
```

If two objects have identical serialized contents, they receive the same ID.

---

# Deterministic Trees

Tree serialization must be deterministic.

If the same entries are inserted in different orders:

```text
A
B
C
```

and:

```text
C
A
B
```

the serialized representation should still produce the same logical Tree.

Mini Git therefore sorts Tree entries before serialization.

This ensures:

```text
same contents
      ↓
same serialization
      ↓
same SHA-256
      ↓
same object ID
```

---

# References

References provide human-readable names for commits.

A branch reference can conceptually contain:

```text
refs/heads/main
```

whose content is:

```text
<commit-id>
```

Branches therefore do not duplicate commits.

They are simply names pointing to commit IDs.

---

# HEAD

`HEAD` identifies the current position in the repository.

A symbolic `HEAD` looks like:

```text
ref: refs/heads/main
```

The resolution process becomes:

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
Commit
```

Mini Git also models detached `HEAD`.

In detached mode:

```text
HEAD
 │
 ▼
Commit ID
```

rather than pointing to a branch reference.

---

# Branches

A branch is a named reference to a commit.

For example:

```text
refs/heads/main
refs/heads/feature
```

may point to:

```text
main    → Commit B
feature → Commit C
```

Creating a branch does not copy the history.

It creates another reference.

Example:

```text
                 Commit A
                 /      \
                /        \
           main            feature
             │                │
             ▼                ▼
          Commit B         Commit C
```

---

# Branch Commands

List branches:

```bash
mini-git branch
```

Create a branch:

```bash
mini-git branch feature
```

The current branch is marked with:

```text
*
```

Example:

```text
* main
  feature
```

---

# Checkout

Checkout changes the current branch and restores its committed Tree into the Working Tree.

Conceptually:

```text
mini-git checkout feature

        │
        ▼

refs/heads/feature

        │
        ▼

    Commit ID

        │
        ▼

      Commit

        │
        ▼

       Tree

        │
        ▼

Restore Working Tree

        │
        ▼

Update HEAD
```

The current implementation supports basic branch checkout and recursive Tree restoration.

---

# Checkout Limitations

The current checkout implementation is intentionally conservative.

It currently:

* resolves the target branch
* resolves its commit
* loads the commit
* loads the Tree
* recursively restores files
* updates symbolic `HEAD`

However, it does not yet fully implement all of Git's checkout safety behavior.

In particular, future work includes:

* robust dirty Working Tree detection
* deleting files absent from the target snapshot
* complete Index synchronization
* more sophisticated overwrite protection

These are intentionally left for later architectural improvements.

---

# Diff

Phase 14 introduces the Diff subsystem.

Diff answers:

> What changed between two repository states?

The subsystem first converts states into a common representation:

```text
path → content
```

For example:

```text
main.cpp  → "#include <iostream>\n..."
hello.txt → "hello\nworld\n"
```

Once states have the same representation, they can be compared independently of how they were originally stored.

---

# Diff Modes

## Working Tree vs Index

```bash
mini-git diff
```

This compares:

```text
Index
  ↕
Working Tree
```

It answers:

> What has changed in my Working Tree since I staged it?

---

## Index vs HEAD

```bash
mini-git diff --cached
```

This compares:

```text
HEAD
  ↕
Index
```

It answers:

> What changes are staged for the next commit?

---

## Commit vs Working Tree

```bash
mini-git diff <commit>
```

This compares:

```text
Commit
  ↕
Working Tree
```

It answers:

> How does the current Working Tree differ from this commit?

---

## Commit vs Commit

```bash
mini-git diff <commit1> <commit2>
```

This compares:

```text
Commit 1
   ↕
Commit 2
```

It answers:

> What changed between these two snapshots?

---

# Snapshot Representation

The Diff subsystem normalizes different repository states into:

```cpp
std::map<std::string, std::string>
```

Conceptually:

```text
"path/to/file" → "file contents"
```

For example:

```text
src/main.cpp → "#include <iostream>\n..."
README.md    → "# Mini Git\n..."
```

This creates a common abstraction for comparison.

---

# Why Snapshots?

Without snapshots, the Diff subsystem would need separate comparison algorithms for:

```text
Working Tree vs Index
Index vs Commit
Commit vs Commit
Commit vs Working Tree
```

Instead, every state is converted into the same representation:

```text
Repository State
       │
       ▼
   Snapshot
       │
       ▼
Compare
```

This simplifies the architecture.

---

# Diff Algorithm

Phase 14 uses a line-based comparison based on the **Longest Common Subsequence (LCS)** concept.

Given:

```text
A
B
C
```

and:

```text
A
X
C
```

the common subsequence is:

```text
A
C
```

Therefore:

```text
-B
+X
```

The algorithm builds an LCS table and reconstructs the changes.

---

# Unified Diff

Mini Git produces output in a familiar diff-like format.

Example:

```text
diff -- mini-git hello.txt
--- a/hello.txt
+++ b/hello.txt
@@ -1,2 +1,2 @@
 hello
-world
+Mini Git
```

The meaning is:

```text
 space = unchanged
 -     = removed
 +     = added
```

---

# Added Files

If a path exists only in the new state, it is treated as an addition.

Example:

```text
@@ -1,0 +1,2 @@
+hello
+world
```

---

# Deleted Files

If a path exists only in the old state, it is treated as deleted.

Example:

```text
@@ -1,2 +1,0 @@
-hello
-world
```

---

# Modified Files

If a path exists in both states but its content differs:

```text
-old content
+new content
```

the file is considered modified.

---

# Unchanged Files

If:

```text
old_content == new_content
```

the file produces no diff output.

This prevents unchanged files from generating unnecessary output.

---

# Repository State Model

At Phase 14, the core state model is:

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
                          ▼
                        Blobs
```

The Working Tree and Index exist alongside this persistent history:

```text
                  Persistent History
                         │
                         ▼
                       Commit
                         │
                         ▼
                        Tree
                         │
                         ▼
                        Blob


Working Tree ─────────────┐
                          │
Index ────────────────────┼──► Diff
                          │
Commit ───────────────────┘
```

---

# Core Model

The complete conceptual model is:

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
                  Object Database
                         │
              ┌──────────┼──────────┐
              │          │          │
              ▼          ▼          ▼
            Blob       Tree       Commit
                                    │
                                    ▼
                                  History
                                    │
                         ┌──────────┴──────────┐
                         │                     │
                       Branch               Branch
                         │                     │
                         └──────────┬──────────┘
                                    ▼
                                   HEAD
```

Diff operates across these states:

```text
Working Tree
      │
      ▼
   Snapshot
      │
      │
Index ───────► Snapshot
      │
      │
Commit ──────► Snapshot
      │
      ▼
   Compare
      │
      ▼
    Diff
```

---

# Command Reference

| Command                           | Status        | Purpose                     |
| --------------------------------- | ------------- | --------------------------- |
| `mini-git --version`              | ✅ Implemented | Show version                |
| `mini-git init`                   | ✅ Implemented | Initialize a repository     |
| `mini-git hash-file <file>`       | ✅ Implemented | Hash file content           |
| `mini-git hash-object <file>`     | ✅ Implemented | Store a Blob object         |
| `mini-git add <file>`             | ✅ Implemented | Stage a file                |
| `mini-git status`                 | ✅ Implemented | Inspect repository state    |
| `mini-git commit -m "<message>"`  | ✅ Implemented | Create a commit             |
| `mini-git log`                    | ✅ Implemented | Display commit history      |
| `mini-git branch`                 | ✅ Implemented | List branches               |
| `mini-git branch <name>`          | ✅ Implemented | Create a branch             |
| `mini-git checkout <branch>`      | ✅ Implemented | Switch branches             |
| `mini-git diff`                   | ✅ Implemented | Working Tree vs Index       |
| `mini-git diff --cached`          | ✅ Implemented | Index vs HEAD               |
| `mini-git diff <commit>`          | ✅ Implemented | Commit vs Working Tree      |
| `mini-git diff <commit> <commit>` | ✅ Implemented | Commit vs Commit            |
| `mini-git merge`                  | ⏳ Planned     | Merge histories             |
| `mini-git tag`                    | ⏳ Planned     | Create tags                 |
| `mini-git inspect`                | ⏳ Planned     | Inspect objects             |
| `mini-git explain`                | ⏳ Planned     | Explain internal operations |
| `mini-git graph`                  | ⏳ Planned     | Visualize history           |
| `mini-git stats`                  | ⏳ Planned     | Repository statistics       |
| `mini-git fsck`                   | ⏳ Planned     | Repository integrity        |

---

# Example Workflow

Initialize a repository:

```bash
mini-git init
```

Create a file:

```bash
printf 'hello\nworld\n' > hello.txt
```

Stage it:

```bash
mini-git add hello.txt
```

Create the first commit:

```bash
mini-git commit -m "Initial commit"
```

Create a feature branch:

```bash
mini-git branch feature
```

List branches:

```bash
mini-git branch
```

Example:

```text
* main
  feature
```

Switch to the feature branch:

```bash
mini-git checkout feature
```

Modify the file:

```bash
printf 'hello\nMini Git\n' > hello.txt
```

Inspect the unstaged changes:

```bash
mini-git diff
```

Stage the change:

```bash
mini-git add hello.txt
```

Inspect the staged change:

```bash
mini-git diff --cached
```

Commit:

```bash
mini-git commit -m "Update hello"
```

View history:

```bash
mini-git log
```

Switch back:

```bash
mini-git checkout main
```

Compare two commits:

```bash
mini-git diff <old-commit> <new-commit>
```

---

# Testing

Mini Git uses CTest and separate test executables for individual subsystems.

Current tests include:

```text
HashTests
ObjectTests
FileReaderTests
BlobTests
TreeBuilderTests
IndexTests
StatusTests
LogTests
CommitTests
ReferenceTests
RepositoryTests
BranchTests
CheckoutTests
DiffTests
```

---

# Diff Tests

Phase 14 specifically tests:

* Working Tree vs Index
* Index vs HEAD
* Commit vs Working Tree
* Commit vs Commit
* identical states
* modified files
* added files
* deleted files
* Blob reconstruction
* Tree reconstruction
* snapshot construction
* unified diff generation
* line-based differences
* LCS comparison

---

# Build

From the project root:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

Run the executable:

```bash
./build/mini-git --version
```

---

# Development Environment

The project is developed using:

* C++20
* Apple Silicon macOS
* Apple Clang
* CMake
* OpenSSL 3
* CTest
* Git
* Unix filesystem APIs

The project is designed around Unix-like filesystem behavior.

---

# Design Decisions

## C++20

C++20 provides:

* modern STL
* `std::filesystem`
* stronger language features
* cleaner abstractions
* modern compile-time facilities

---

## CMake

CMake separates:

```text
source code
```

from:

```text
build configuration
```

and makes the project easier to build and test.

---

## OpenSSL EVP

SHA-256 hashing is implemented using OpenSSL's EVP interface.

Conceptually:

```text
Object
  │
  ▼
Serialized bytes
  │
  ▼
EVP SHA-256
  │
  ▼
Digest
  │
  ▼
Hexadecimal Object ID
```

---

## Immutable Objects

Once an object is stored:

```text
object ID → object contents
```

the object is treated as immutable.

A new content state produces a new object ID.

This is one of the central ideas behind content-addressable storage.

---

## Separation of Concerns

Each subsystem has a focused responsibility.

```text
Hash
    hashing

Blob
    file content representation

Tree
    directory snapshot

Commit
    history metadata

ObjectDatabase
    persistent object storage

Index
    staging state

TreeBuilder
    construct Trees from staged state

Reference
    named repository references

Repository
    repository-level state and operations

Diff
    state comparison
```

---

# Known Simplifications

Mini Git intentionally does not reproduce every detail of Git.

## Object Format

Mini Git uses its own educational object format.

It is not byte-compatible with Git's object format.

---

## Index

The Index is a custom educational representation.

It is not Git's binary index format.

---

## Checkout

Checkout currently implements basic branch switching and Tree restoration.

Full dirty-tree safety and complete Index synchronization remain future work.

---

## Diff

The current Diff implementation is intentionally simple.

It provides:

* line-based comparison
* basic unified output
* LCS-based change detection

It does not yet implement:

* sophisticated Git-style hunks
* rename detection
* copy detection
* binary diffs
* word-level diffs
* colorized output
* patch application

---

# Roadmap

## Phase 0 — Git Concepts

Understand:

* version control
* snapshots
* commits
* branches
* references
* content addressing

**Status: Completed**

---

## Phase 1 — Project Foundation

Build:

* C++20 project
* CMake
* source/include separation
* testing infrastructure

**Status: Completed**

---

## Phase 2 — Repository Initialization

Implement:

* repository detection
* `.mini-git`
* objects directory
* refs directory
* `HEAD`

**Status: Completed**

---

## Phase 3 — Hashing

Implement:

* SHA-256
* hexadecimal object IDs
* OpenSSL EVP

**Status: Completed**

---

## Phase 4 — Object Model

Implement:

* Object abstraction
* Blob
* Tree
* Commit
* serialization

**Status: Completed**

---

## Phase 5 — Blob Objects

Implement:

* file reading
* Blob creation
* Blob serialization
* Blob storage

**Status: Completed**

---

## Phase 6 — Object Database

Implement:

* object storage
* object retrieval
* object existence
* content-addressable persistence

**Status: Completed**

---

## Phase 7 — Trees

Implement:

* Tree entries
* nested Trees
* deterministic serialization
* TreeBuilder

**Status: Completed**

---

## Phase 8 — Index / Staging

Implement:

* Index
* staging
* Blob IDs in the Index
* persistence

**Status: Completed**

---

## Phase 9 — Status

Implement repository state inspection.

**Status: Completed**

---

## Phase 10 — Commits

Implement:

* commit creation
* parent relationships
* commit references
* commit persistence

**Status: Completed**

---

## Phase 11 — Log / History

Implement:

* commit traversal
* parent traversal
* history display

**Status: Completed**

---

## Phase 12 — References & HEAD

Implement:

* references
* symbolic HEAD
* detached HEAD
* HEAD resolution

**Status: Completed**

---

## Phase 13 — Branches & Checkout

Implement:

* branch creation
* branch listing
* branch references
* current branch detection
* checkout
* target commit resolution
* Tree restoration
* symbolic HEAD switching

**Status: Completed**

---

## Phase 14 — Diff & Change Inspection

Implement:

* state snapshots
* Working Tree comparison
* Index comparison
* commit comparison
* added-file detection
* deleted-file detection
* modified-file detection
* LCS-based comparison
* unified diff output

**Status: Completed**

---

## Phase 15 — Merge

Planned:

* ancestor discovery
* merge base
* fast-forward merge
* branch merging

**Status: Future**

---

## Phase 16 — Conflict Handling

Planned:

* conflicting changes
* conflict markers
* conflict state
* conflict resolution

**Status: Future**

---

## Phase 17 — Tags

Planned:

* lightweight tags
* annotated-style tags
* tag references
* tag listing

**Status: Future**

---

## Phase 18 — Repository Integrity & Maintenance

Planned:

* `fsck`
* object reachability
* dangling objects
* corruption detection
* repository consistency
* garbage collection concepts

**Status: Future**

---

## Phase 19 — Educational Intelligence

Planned:

```bash
mini-git inspect
mini-git explain
mini-git graph
mini-git stats
```

The goal is to make internal repository behavior inspectable and explainable.

**Status: Future**

---

## Phase 20 — Portfolio Finalization

Planned:

* architectural cleanup
* documentation refinement
* performance improvements
* benchmarking
* stronger testing
* final README
* GitHub presentation
* interview explanations

**Status: Future**

---

# Roadmap at a Glance

```text
 0  Git Concepts                         ✅
 │
 1  Project Foundation                  ✅
 │
 2  Repository Initialization            ✅
 │
 3  Hashing                              ✅
 │
 4  Object Model                         ✅
 │
 5  Blob Objects                          ✅
 │
 6  Object Database                      ✅
 │
 7  Trees                                 ✅
 │
 8  Index / Staging                       ✅
 │
 9  Status                                ✅
 │
10  Commits                               ✅
 │
11  Log / History                         ✅
 │
12  References & HEAD                    ✅
 │
13  Branches & Checkout                  ✅
 │
14  Diff & Change Inspection              ✅
 │
15  Merge                                 ⏳
 │
16  Conflict Handling                     ⏳
 │
17  Tags                                  ⏳
 │
18  Repository Integrity & Maintenance    ⏳
 │
19  Educational Intelligence              ⏳
 │
20  Portfolio Finalization                ⏳
```

---

# What Makes Mini Git Different?

Mini Git is intentionally designed as an educational systems project rather than a Git clone.

The project focuses on exposing the internal mechanisms.

For example:

```text
mini-git add file.cpp
```

is not treated as a magical command.

The internal process is:

```text
file.cpp
   │
   ▼
FileReader
   │
   ▼
Blob
   │
   ▼
Serialize
   │
   ▼
SHA-256
   │
   ▼
Object ID
   │
   ▼
Object Database
   │
   ▼
Index
```

Similarly:

```text
mini-git commit
```

becomes:

```text
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
Object Database
  │
  ▼
Branch Reference
  │
  ▼
HEAD
```

And:

```text
mini-git diff
```

becomes:

```text
Working Tree
      │
      ▼
Snapshot
      │
      │
Index ───────► Snapshot
      │
      ▼
Compare
      │
      ▼
LCS
      │
      ▼
Unified Diff
```

This makes Mini Git useful for understanding both **software architecture** and **version-control internals**.

---

# Engineering Principles

The project follows several engineering principles.

## Single Responsibility

Each class should have a clear purpose.

## RAII

Resources should be managed automatically where appropriate.

## Const Correctness

Read-only operations should be marked `const` where possible.

## Explicit Error Handling

Invalid repository states and invalid commands should produce meaningful errors.

## Deterministic Serialization

Equivalent objects should produce stable serialized representations.

## Immutable Stored Objects

Stored objects should not be modified after creation.

## Separation of Storage and Logic

Object storage should not contain repository command logic.

## Testability

Subsystems should be independently testable.

## Incremental Architecture

Each phase should build naturally on the previous phases.

---

# Technology Stack

| Technology        | Purpose                        |
| ----------------- | ------------------------------ |
| C++20             | Main implementation language   |
| CMake             | Build system                   |
| OpenSSL 3         | SHA-256 hashing                |
| CTest             | Automated testing              |
| `std::filesystem` | Filesystem operations          |
| STL               | Data structures and algorithms |
| Git               | Project version control        |

---

# Project Structure

```text
mini-git/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── include/
│   ├── Object.hpp
│   ├── Hash.hpp
│   ├── FileReader.hpp
│   ├── Blob.hpp
│   ├── Tree.hpp
│   ├── Commit.hpp
│   ├── ObjectDatabase.hpp
│   ├── Repository.hpp
│   ├── Reference.hpp
│   ├── TreeBuilder.hpp
│   ├── Index.hpp
│   ├── Status.hpp
│   └── Diff.hpp
│
├── src/
│   ├── main.cpp
│   ├── Hash.cpp
│   ├── FileReader.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   ├── ObjectDatabase.cpp
│   ├── Repository.cpp
│   ├── Reference.cpp
│   ├── TreeBuilder.cpp
│   ├── Index.cpp
│   ├── Status.cpp
│   └── Diff.cpp
│
├── tests/
│   ├── HashTests.cpp
│   ├── ObjectTests.cpp
│   ├── FileReaderTests.cpp
│   ├── BlobTests.cpp
│   ├── TreeBuilderTests.cpp
│   ├── IndexTests.cpp
│   ├── StatusTests.cpp
│   ├── LogTests.cpp
│   ├── CommitTests.cpp
│   ├── ReferenceTests.cpp
│   ├── RepositoryTests.cpp
│   ├── BranchTests.cpp
│   ├── CheckoutTests.cpp
│   └── DiffTests.cpp
│
└── docs/
    ├── architecture.md
    └── diff.md
```

---

# Limitations

Mini Git is not Git.

It currently does not provide:

* Git repository compatibility
* Git's exact object format
* Git's binary Index format
* complete checkout semantics
* merge
* conflict resolution
* tags
* remotes
* networking
* push/pull
* GitHub integration
* complete repository maintenance
* complete Git-compatible diff behavior

These features are either intentionally simplified or planned for future phases.

The project's first priority is understanding **local version-control internals**.

---

# Learning Outcomes

By completing Mini Git, the project should provide practical understanding of:

## C++

* C++20
* classes
* interfaces
* RAII
* const correctness
* STL containers
* filesystem APIs
* exception handling
* modular design

## Systems Programming

* filesystem representation
* persistent storage
* file I/O
* object databases
* serialization
* repository state
* references

## Algorithms & Data Structures

* hashing
* trees
* recursion
* graph traversal
* ancestry
* state comparison
* LCS
* future three-way merge

## Software Engineering

* CMake
* testing
* Git workflows
* architecture
* documentation
* incremental development
* debugging
* refactoring

## Version Control

* Working Tree
* staging
* Index
* snapshots
* objects
* Blobs
* Trees
* commits
* references
* HEAD
* branches
* history
* checkout
* diff
* future merge
* conflicts
* tags
* integrity

---

# Final Architecture Goal

When the project is complete, the architecture should look approximately like this:

```text
                         Mini Git CLI
                              │
                              ▼
                       Command Layer
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
 Working Tree               Index              Repository
        │                     │                     │
        ▼                     │              ┌──────┴──────┐
   FileReader                 │              │             │
        │                     │             HEAD       References
        ▼                     │              │             │
      Blob                    │              └──────┬──────┘
        │                     │                     │
        └──────────────┬──────┘                     │
                       ▼                            │
                Object Database ◄──────────────────┘
                       │
             ┌─────────┼─────────┐
             │         │         │
             ▼         ▼         ▼
           Blob      Tree      Commit
                                 │
                                 ▼
                              History
                                 │
                 ┌───────────────┼───────────────┐
                 ▼               ▼               ▼
              Branch          Merge            Tags
                                 │
                                 ▼
                              Conflicts
                                 │
                                 ▼
                         Integrity / FSCK
                                 │
                                 ▼
                       Educational Layer
                                 │
              ┌──────────────────┼──────────────────┐
              ▼                  ▼                  ▼
           Inspect            Explain             Graph
              │                  │                  │
              └──────────────────┼──────────────────┘
                                 ▼
                               Stats
```

---

# Final Vision

The final Mini Git should not simply imitate Git's command names.

It should provide a clear, inspectable implementation of the fundamental ideas behind version control.

The intended progression is:

```text
create repository
      │
      ▼
stage files
      │
      ▼
create objects
      │
      ▼
build Trees
      │
      ▼
create commits
      │
      ▼
traverse history
      │
      ▼
create branches
      │
      ▼
switch branches
      │
      ▼
compare states
      │
      ▼
merge histories
      │
      ▼
resolve conflicts
      │
      ▼
inspect repository
      │
      ▼
analyze repository
      │
      ▼
verify repository integrity
```

The deeper goal is to make the system understandable enough that a developer can ask:

> **"What exactly happened internally?"**

and trace the answer through the implementation.

That is the central idea of Mini Git.

---

# Current Status

| Item              | Status                                                             |
| ----------------- | ------------------------------------------------------------------ |
| Current Phase     | **14 / 20**                                                        |
| Current Subsystem | **Diff & Change Inspection**                                       |
| Previous Phase    | Branches & Checkout                                                |
| Next Phase        | Merge                                                              |
| Language          | C++20                                                              |
| Build System      | CMake                                                              |
| Hashing           | SHA-256 / OpenSSL EVP                                              |
| Storage           | Content-addressable Object Database                                |
| Testing           | CTest                                                              |
| Project Type      | Systems Programming / Version Control / Educational Infrastructure |

---

# License

See `LICENSE` for the project's license and usage terms.
