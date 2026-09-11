# Mini Git

A **Git-inspired version control system built from scratch in C++20** to understand how modern version control systems work internally.

Mini Git is a serious systems-programming and software-engineering project focused on implementing the fundamental mechanisms behind Git from first principles:

* Content-addressable storage
* SHA-256 object hashing
* Blob, Tree, and Commit objects
* Object databases
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

And history becomes a graph of commits:

```text
HEAD

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

References add another important layer:

```text
HEAD
 │
 ▼
refs/heads/main
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

Mini Git is therefore both a practical version-control implementation and a way to study:

* filesystem internals
* data structures
* serialization
* hashing
* persistence
* graphs
* state management
* C++ architecture
* testing
* systems design

---

# Current Status

## Phase 12 — References & HEAD

Mini Git has completed the foundational object, storage, staging, status, commit, history, and reference layers.

### Implemented

* C++20 project
* CMake build system
* Automated testing with CTest
* Repository initialization
* `.mini-git` repository structure
* SHA-256 hashing
* OpenSSL EVP integration
* Generic Object interface
* Blob objects
* Tree objects
* Commit objects
* Binary-safe file reading
* Persistent Object Database
* Object storage and retrieval
* Object deduplication
* Recursive Tree construction
* Index / staging area
* File staging
* Working Tree status
* Commit creation
* Commit parent relationships
* Commit deserialization
* Commit history traversal
* `mini-git log`
* Reference abstraction
* Symbolic references
* Direct references
* HEAD resolution
* Symbolic HEAD detection
* Detached HEAD detection
* Current branch resolution
* HEAD commit resolution
* Local branch listing
* `mini-git branch`

### Current core pipeline

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
Reference
     │
     ▼
    HEAD
     │
     ▼
   Log
```

### Reference architecture

```text
HEAD
 │
 ├── symbolic
 │      │
 │      ▼
 │  refs/heads/main
 │      │
 │      ▼
 │   Commit ID
 │
 └── detached
        │
        ▼
     Commit ID
```

### Next phase

**Phase 13 — Branches**

---

# Project Goals

The project has five major goals.

## 1. Understand Git Internals

Understand how:

* objects
* hashes
* trees
* commits
* references
* branches
* `HEAD`
* staging
* history

fit together.

## 2. Build Real Systems Software

Practice:

* C++20
* filesystem APIs
* binary I/O
* serialization
* persistent storage
* error handling
* modular architecture
* testing

## 3. Implement Incrementally

Each subsystem should build on the previous one.

The project should not become a huge collection of unrelated features.

## 4. Make Internals Observable

Mini Git should eventually provide educational commands that allow users to inspect and understand its internal state.

## 5. Produce a Portfolio-Quality Project

The final repository should demonstrate:

* systems programming
* C++ proficiency
* software architecture
* testing
* Git knowledge
* data structures
* debugging
* technical documentation

---

# Architecture Overview

Mini Git is organized around several cooperating subsystems.

```text
                         Mini Git CLI

                              │

                              ▼

                       Command Layer

                              │

          ┌───────────────────┼───────────────────┐
          │                   │                   │
          ▼                   ▼                   ▼

    Working Tree           Index             Repository
          │                   │                   │
          ▼                   │            ┌──────┼──────┐
      FileReader              │            │      │      │
          │                   │           HEAD References
          ▼                   │                   │
         Blob                 │                   │
          │                   │                   │
          └───────────┬───────┘                   │
                      ▼                           │
               Object Database ◄─────────────────┘
                      │
             ┌────────┼────────┐
             │        │        │
             ▼        ▼        ▼
           Blob     Tree     Commit
                              │
                              ▼
                           History
```

Each component has a focused responsibility.

| Component        | Responsibility                              |
| ---------------- | ------------------------------------------- |
| `Hash`           | SHA-256 hashing                             |
| `Object`         | Common object interface                     |
| `FileReader`     | Binary-safe file reading                    |
| `Blob`           | File content representation                 |
| `Tree`           | Directory representation                    |
| `TreeBuilder`    | Building Trees from filesystem/index state  |
| `Commit`         | Snapshot metadata and history relationships |
| `ObjectDatabase` | Persistent object storage                   |
| `Index`          | Staging area                                |
| `Status`         | Working Tree state analysis                 |
| `Reference`      | Safe named reference abstraction            |
| `Repository`     | Repository-level state                      |
| CLI              | User-facing commands                        |

---

# Core Concepts

## Content-Addressable Storage

Objects are identified by the hash of their serialized contents.

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

Therefore:

```text
same object
    ↓
same serialization
    ↓
same SHA-256
    ↓
same Object ID
```

This provides deterministic object identity and allows identical objects to be reused.

---

# Object Model

Mini Git currently uses three major object types:

```text
Object

├── Blob

├── Tree

└── Commit
```

The base abstraction is:

```cpp
class Object {
public:
    virtual ~Object() = default;

    virtual std::string serialize() const = 0;
};
```

Every object can therefore provide its serialized representation.

The Object Database can then treat all objects uniformly:

```text
Object
  │
  ├── serialize()
  │
  ▼
Serialized bytes
  │
  ▼
SHA-256
  │
  ▼
Object ID
```

---

# Blob Objects

A Blob represents file contents.

The Blob does not fundamentally represent a filename.

It represents the contents of a file.

```text
File
 │
 ▼
FileReader
 │
 ▼
Blob
```

The current educational Blob format is:

```text
blob <size>\0<content>
```

For example:

```text
blob 15\0Hello Mini Git!
```

The serialized representation is then hashed.

```text
File
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
```

---

# FileReader

`FileReader` isolates filesystem reading from object construction.

Its responsibility is intentionally simple:

> Read the exact bytes of a file.

Files are opened in binary mode.

This allows Mini Git to correctly handle files containing:

* text
* binary data
* null bytes
* arbitrary byte sequences

The separation is:

```text
Filesystem
    │
    ▼
FileReader
    │
    ▼
Raw bytes
    │
    ▼
Blob
```

---

# Object Database

The Object Database provides persistent storage for repository objects.

Its responsibilities are:

* storing objects
* generating object IDs
* checking whether objects exist
* reading objects
* preventing unnecessary duplicate storage

The current simplified storage layout is:

```text
.mini-git/

└── objects/

    ├── <object-id>

    ├── <object-id>

    └── <object-id>
```

An object's SHA-256 identifier is used as its filename.

The current implementation intentionally uses a flat object store to keep the storage mechanism easy to inspect.

---

# Trees

A Tree represents directory structure.

For example:

```text
project/

├── README.md

├── main.cpp

└── src/

    ├── App.cpp

    └── Utils.cpp
```

becomes conceptually:

```text
Root Tree

├── README.md → Blob

├── main.cpp  → Blob

└── src       → Tree

                 ├── App.cpp   → Blob

                 └── Utils.cpp → Blob
```

A Tree therefore contains references to other objects.

```text
Tree

├── Blob

├── Blob

└── Tree

    └── Blob
```

This allows an entire directory hierarchy to be represented as an object graph.

---

# Deterministic Trees

Filesystem iteration order should not be relied upon.

Therefore Mini Git sorts Tree entries before serialization.

For example:

```text
README.md
main.cpp
src
```

will be serialized deterministically.

This guarantees:

```text
same directory state
        ↓
same Tree serialization
        ↓
same Tree Object ID
```

This property is essential for content-addressable storage.

---

# TreeBuilder

`TreeBuilder` converts directory/index state into Tree objects.

For a filesystem tree:

```text
project/

├── main.cpp

├── README.md

└── src/

    ├── App.cpp

    └── Utils.cpp
```

TreeBuilder recursively constructs:

```text
Root Tree

├── main.cpp  → Blob ID

├── README.md → Blob ID

└── src       → Tree ID

                  ├── App.cpp   → Blob ID

                  └── Utils.cpp → Blob ID
```

The `.mini-git` directory is excluded from generated Trees.

TreeBuilder also supports constructing a Tree from staged Index entries.

That distinction is important because commits should represent the **staged snapshot**, not simply whatever happens to be present in the Working Tree.

---

# Index / Staging Area

The Index is the bridge between the Working Tree and a commit.

```text
Working Tree

     │

     │ add

     ▼

   Index

     │

     │ commit

     ▼

  Snapshot
```

An Index entry currently contains:

```text
path → object ID
```

For example:

```text
main.cpp    abc123...
README.md   def456...
src/App.cpp ghi789...
```

The Index is persisted at:

```text
.mini-git/index
```

The current educational representation is:

```text
path<TAB>object-id
```

The Index stores references to Blob objects rather than duplicating their contents.

---

# Staging a File

The current command is:

```bash
mini-git add main.cpp
```

The internal process is:

```text
main.cpp

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

Blob ID

   │

   ▼

Index
```

If the file changes and is staged again, the Index entry is updated to reference the new Blob.

The previous Blob remains immutable.

---

# Status

Mini Git currently supports:

```bash
mini-git status
```

The current implementation compares the Index with the Working Tree.

It can detect:

* modified files
* deleted tracked files
* untracked files
* nested untracked files
* clean Working Tree state

Example:

```text
On branch main

Changes not staged for commit:

  modified: main.cpp
```

Untracked:

```text
On branch main

Untracked files:

  notes.txt
```

Deleted:

```text
On branch main

Deleted files:

  deleted: main.cpp
```

Phase 12 also introduces detached HEAD awareness.

For example:

```text
HEAD detached at 8c91abc
```

The long-term status model will become:

```text
HEAD

 │

 ▼

Index

 │

 ▼

Working Tree
```

which will allow Mini Git to distinguish:

* committed changes
* staged changes
* unstaged changes
* untracked files

---

# Commits

A Commit represents a repository snapshot and its position in history.

A Commit contains:

```text
Commit

├── tree

├── parent

├── author

└── message
```

The parent is optional for the initial commit.

Example initial commit:

```text
tree <tree-id>

author <author>

Initial commit
```

Later commits contain a parent:

```text
tree <tree-id>

parent <parent-id>

author <author>

Update project
```

This creates a linked history:

```text
Commit C

   │

   │ parent

   ▼

Commit B

   │

   │ parent

   ▼

Commit A
```

---

# Commit Creation

The conceptual commit pipeline is:

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
```

The important design principle is:

> A commit represents the staged state, not an arbitrary snapshot of the Working Tree.

This is one of the key ideas behind Git's architecture.

---

# Commit History

Mini Git supports:

```bash
mini-git log
```

The history system follows the chain of parent references.

Conceptually:

```text
HEAD

 │

 ▼

refs/heads/main

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

The traversal process is:

```text
Read HEAD

   │

   ▼

Resolve HEAD

   │

   ▼

Resolve current commit

   │

   ▼

Read Commit object

   │

   ▼

Deserialize Commit

   │

   ▼

Display information

   │

   ▼

Read parent ID

   │

   ▼

Repeat
```

Example:

```text
commit 8c91...

Author: Sarina

    Update project

commit 3a42...

Author: Sarina

    Initial commit
```

If the repository has no commits:

```text
No commits yet.
```

---

# References

Phase 12 introduces a dedicated `Reference` abstraction.

Instead of allowing different parts of the program to manipulate reference files directly, references are represented through a common interface.

A reference conceptually maps:

```text
Reference Name
      │
      ▼
 Object ID
```

For example:

```text
refs/heads/main
        │
        ▼
    8c91abcd...
```

The repository therefore separates:

```text
Reference Name
      ↓
Reference
      ↓
Object ID
      ↓
Object
```

This creates a foundation for branches, tags, and other named repository references.

---

# Symbolic HEAD

A symbolic HEAD contains a reference rather than a commit ID.

The default repository HEAD is:

```text
ref: refs/heads/main
```

The resolution process is:

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

The important distinction is:

```text
HEAD
  ↓
Reference
  ↓
Commit
```

rather than:

```text
HEAD
  ↓
Commit
```

This allows HEAD to follow a branch as the branch moves.

---

# Detached HEAD

Phase 12 also introduces support for direct HEAD references.

A detached HEAD contains a commit ID directly:

```text
8c91abcd1234...
```

The resolution process becomes:

```text
HEAD

 │

 ▼

Commit ID

 │

 ▼

Commit Object
```

Mini Git can therefore distinguish:

```text
Symbolic HEAD
```

from:

```text
Detached HEAD
```

The repository exposes this state through:

```cpp
Repository::is_detached_head()
```

When HEAD is symbolic:

```cpp
Repository::current_branch()
```

returns the current branch.

When HEAD is detached, there is no current branch.

---

# HEAD Resolution

The Repository now exposes several explicit state queries:

```text
head_reference()
is_detached_head()
current_branch()
head_commit()
branches()
```

Conceptually:

```text
                  HEAD
                   │
          ┌────────┴────────┐
          │                 │
      symbolic           detached
          │                 │
          ▼                 ▼
 refs/heads/main       Commit ID
          │                 │
          └────────┬────────┘
                   ▼
              Commit ID
```

This makes repository state explicit rather than hidden inside command-specific file handling.

---

# Branch References

Branches are represented by references under:

```text
.mini-git/refs/heads/
```

For example:

```text
.mini-git/

└── refs/

    └── heads/

        └── main
```

The `main` file contains the commit ID at the tip of the branch.

Conceptually:

```text
refs/heads/main
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

A branch therefore does not contain a copy of the history.

It is simply a named reference to a commit.

---

# Branch Listing

Phase 12 introduces:

```bash
mini-git branch
```

The command currently lists existing local branches.

Example:

```text
* main
```

The `*` indicates the current branch.

Branch creation and switching are intentionally deferred to Phase 13.

---

# Repository Layout

A Mini Git repository currently looks like:

```text
project/

│

├── .mini-git/

│   ├── objects/

│   │   ├── <object-id>

│   │   └── <object-id>

│   │

│   ├── refs/

│   │   └── heads/

│   │       └── main

│   │

│   ├── HEAD

│   └── index

│

├── README.md

├── main.cpp

└── src/

    └── App.cpp
```

The Working Tree contains project files.

`.mini-git` contains repository state.

---

# Complete Core Model

The current architecture can be summarized as:

```text
                         HEAD

                          │

                          ▼

                     Reference

                          │

                          ▼

                        Commit

                       /      \

                      /        \

                 Tree            Parent

                /   \              │

               /     \             ▼

            Blob     Tree        Commit
```

And the staging pipeline is:

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

Together:

```text
                 ┌──────────── Working Tree
                 │
                 ▼
               FileReader
                 │
                 ▼
                Blob
                 │
                 ▼
          Object Database
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
             Reference
                 │
                 ▼
                HEAD
                 │
                 ▼
               History
```

---

# Command Reference

| Command                          | Status        | Purpose                         |
| -------------------------------- | ------------- | ------------------------------- |
| `mini-git --version`             | ✅ Implemented | Show version                    |
| `mini-git init`                  | ✅ Implemented | Initialize repository           |
| `mini-git hash-file <file>`      | ✅ Implemented | Hash a Blob representation      |
| `mini-git hash-object <file>`    | ✅ Implemented | Store a Blob                    |
| `mini-git add <file>`            | ✅ Implemented | Stage a file                    |
| `mini-git status`                | ✅ Implemented | Inspect Working Tree state      |
| `mini-git commit -m "<message>"` | ✅ Implemented | Create a commit                 |
| `mini-git log`                   | ✅ Implemented | Display commit history          |
| `mini-git branch`                | ✅ Implemented | List local branches             |
| `mini-git branch <name>`         | ⏳ Planned     | Create a branch                 |
| `mini-git checkout <branch>`     | ⏳ Planned     | Switch branches / restore state |
| `mini-git diff`                  | ⏳ Planned     | Compare repository states       |
| `mini-git merge`                 | ⏳ Planned     | Merge histories                 |
| `mini-git tag`                   | ⏳ Planned     | Create tags                     |
| `mini-git inspect`               | ⏳ Planned     | Inspect internal objects        |
| `mini-git explain`               | ⏳ Planned     | Explain internal operations     |
| `mini-git graph`                 | ⏳ Planned     | Visualize commit graph          |
| `mini-git stats`                 | ⏳ Planned     | Repository statistics           |
| `mini-git fsck`                  | ⏳ Planned     | Repository integrity analysis   |

---

# Testing

Mini Git uses CTest for automated testing.

The project currently tests several independent layers.

## Hash Tests

Test:

* SHA-256 correctness
* known vectors
* empty input
* deterministic results
* different inputs
* binary data

## FileReader Tests

Test:

* text files
* binary files
* null bytes
* missing files

## Blob Tests

Test:

* file-to-Blob conversion
* Blob serialization
* binary content

## Object Tests

Test:

* object serialization
* Blob objects
* Tree objects
* Commit objects
* object persistence
* object retrieval
* duplicate storage

## TreeBuilder Tests

Test:

* recursive directory traversal
* nested directories
* Blob creation
* Tree creation
* empty directories
* `.mini-git` exclusion

## Index Tests

Test:

* adding entries
* replacing entries
* duplicate paths
* persistence
* reloading

## Status Tests

Test:

* clean state
* modified files
* deleted files
* untracked files
* nested files
* `.mini-git` exclusion

## Commit Tests

Test:

* initial commits
* commits with parents
* serialization
* deterministic representation

## Log Tests

Test:

* Commit deserialization
* initial commits
* parent relationships
* multiline commit messages
* Commit round trips
* persistent Commit objects
* history chains

## Repository Tests

Test:

* repository initialization
* symbolic HEAD
* current branch resolution
* HEAD commit resolution
* detached HEAD
* branch references
* branch listing

## Reference Tests

Test:

* reference creation
* reference existence
* reference reading
* reference writing
* symbolic references
* invalid reference names
* path traversal protection

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

---

# Build Instructions

## Requirements

* C++20 compiler
* CMake 3.20+
* OpenSSL 3
* Git
* macOS or Linux

The project is primarily developed and tested on Apple Silicon macOS.

## Configure

From the project root:

```bash
cmake -S . -B build
```

If OpenSSL needs to be specified explicitly on Apple Silicon:

```bash
cmake -S . -B build \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3
```

## Build

```bash
cmake --build build
```

The executable is:

```text
build/mini-git
```

Run:

```bash
./build/mini-git
```

---

# Example Workflow

Create a repository:

```bash
mkdir mini-git-demo
cd mini-git-demo

/path/to/mini-git/build/mini-git init
```

Create a file:

```bash
printf 'Hello Mini Git!\n' > main.cpp
```

Stage it:

```bash
/path/to/mini-git/build/mini-git add main.cpp
```

Check status:

```bash
/path/to/mini-git/build/mini-git status
```

Commit:

```bash
/path/to/mini-git/build/mini-git commit -m "Initial commit"
```

List branches:

```bash
/path/to/mini-git/build/mini-git branch
```

Inspect history:

```bash
/path/to/mini-git/build/mini-git log
```

Modify the file:

```bash
printf 'Hello again!\n' > main.cpp
```

Stage and commit again:

```bash
/path/to/mini-git/build/mini-git add main.cpp

/path/to/mini-git/build/mini-git commit -m "Update main file"
```

Then:

```bash
/path/to/mini-git/build/mini-git log
```

produces a history conceptually like:

```text
commit <new-id>

Author: Sarina

    Update main file

commit <old-id>

Author: Sarina

    Initial commit
```

---

# Design Decisions

## Why SHA-256?

SHA-256 provides a strong and widely understood cryptographic hashing primitive.

Mini Git uses OpenSSL's EVP interface rather than implementing cryptography itself.

This allows the project to focus on version-control architecture rather than cryptographic implementation.

## Why C++20?

C++20 provides:

* modern filesystem support
* strong type system
* RAII
* standard library containers
* algorithms
* smart pointers
* structured bindings
* modern language features

The project is intended to demonstrate modern C++ rather than C-style systems programming.

## Why a Custom Object Format?

Mini Git is not trying to become Git-compatible.

A custom educational format makes it possible to understand:

* serialization
* object identity
* storage
* references
* object relationships

without simultaneously reproducing every compatibility detail of Git.

## Why a Flat Object Store?

Git's actual storage architecture is more sophisticated.

Mini Git currently uses:

```text
.mini-git/objects/<full-object-id>
```

because it makes the relationship between:

```text
Object ID → Stored Object
```

immediately visible.

Storage can be optimized later without changing the conceptual object model.

## Why Build the Index?

The Index is one of the most important concepts in Git.

It creates a meaningful distinction between:

```text
Working Tree

     ↓

   Index

     ↓

   Commit
```

Without a staging layer, the project would miss one of Git's most interesting architectural ideas.

## Why a Reference Abstraction?

References are central to version control.

Without an abstraction, repository code would need to know exactly where reference files are stored.

The `Reference` class provides a clean boundary:

```text
Repository
    │
    ▼
Reference
    │
    ▼
Reference File
    │
    ▼
Object ID
```

This becomes especially important as Mini Git introduces:

* branches
* tags
* detached HEAD
* multiple references
* reference validation

in later phases.

---

# Known Simplifications

Mini Git intentionally differs from production Git.

### Object Format

Mini Git uses an educational object format.

### Hash Algorithm

Mini Git uses SHA-256.

### Object Storage

Mini Git currently uses flat storage.

### Index

The current Index is a simple text representation:

```text
path<TAB>object-id
```

It is not Git's binary index format.

### Status

Current status primarily compares:

```text
Index ↔ Working Tree
```

Full HEAD-aware status will come later.

### Paths

The current Index representation has limitations around paths containing whitespace.

### Empty Directories

Mini Git can represent empty directories as Tree objects.

Real Git normally does not track empty directories.

### References

Mini Git now has a dedicated Reference abstraction, but its reference rules are intentionally simpler than Git's complete reference implementation.

### Branches

Phase 12 supports branch listing but not branch creation or switching.

### Compatibility

Mini Git does not claim compatibility with Git repositories.

These simplifications are intentional and documented.

---

# 21-Phase Roadmap

The project is organized into **21 phases**.

The phases are ordered so that every major feature is built on top of concepts established earlier.

---

## Phase 0 — Git Concepts

Understand the architecture before implementing it.

Topics:

* Version control
* Working Tree
* snapshots
* objects
* hashes
* content-addressable storage
* Trees
* Commits
* references
* `HEAD`
* branches
* staging
* history

**Status: Completed**

---

## Phase 1 — Project Foundation

Establish the engineering environment.

Topics:

* C++20
* CMake
* project structure
* Git workflow
* testing
* documentation
* compiler configuration

**Status: Completed**

---

## Phase 2 — Repository Initialization

Create the repository abstraction.

Implement:

```bash
mini-git init
```

Concepts:

* `.mini-git`
* repository root
* `HEAD`
* `refs`
* object directory

**Status: Completed**

---

## Phase 3 — Hashing

Implement the hashing layer.

Topics:

* SHA-256
* OpenSSL EVP
* deterministic hashing
* hexadecimal representation
* hashing tests

**Status: Completed**

---

## Phase 4 — Object Model

Create the generic object abstraction.

Implement:

* `Object`
* `Blob`
* `Tree`
* `Commit`
* serialization

**Status: Completed**

---

## Phase 5 — Blob Objects

Implement file-content objects.

Topics:

* binary file reading
* Blob creation
* Blob serialization
* Blob hashing
* file-to-object pipeline

**Status: Completed**

---

## Phase 6 — Object Database

Build persistent object storage.

Implement:

* object storage
* object lookup
* object retrieval
* existence checks
* duplicate prevention

**Status: Completed**

---

## Phase 7 — Trees

Represent directories as object graphs.

Implement:

* Tree entries
* recursive Trees
* nested directories
* deterministic ordering
* Tree persistence
* staged Tree construction

**Status: Completed**

---

## Phase 8 — Index / Staging

Build the staging layer.

Implement:

* Index entries
* persistent Index
* staging files
* updating staged files
* loading/saving Index state

**Status: Completed**

---

## Phase 9 — Status

Analyze repository state.

Implement:

* modified files
* deleted files
* untracked files
* clean state
* recursive scanning
* `.mini-git` exclusion

Future improvements will make status fully HEAD-aware.

**Status: Completed**

---

## Phase 10 — Commits

Create immutable repository snapshots.

Implement:

* Commit objects
* Commit serialization
* parent references
* Tree generation from Index
* commit persistence
* branch reference updates

Core model:

```text
Index
  ↓
Tree
  ↓
Commit
  ↓
Reference
```

**Status: Completed**

---

## Phase 11 — Log / History

Traverse repository history.

Implement:

* reading `HEAD`
* resolving current branch
* reading branch references
* loading Commit objects
* Commit deserialization
* displaying commit information
* parent traversal
* initial-history handling

Command:

```bash
mini-git log
```

**Status: Completed**

---

## Phase 12 — References & HEAD

Formalize repository references.

Implement:

* `Reference` abstraction
* safe reference paths
* symbolic references
* direct references
* reference reading
* reference writing
* HEAD resolution
* symbolic HEAD detection
* detached HEAD detection
* current branch resolution
* HEAD commit resolution
* local branch listing
* `mini-git branch`

The goal is to stop treating references as raw files scattered throughout the code.

Phase 12 also establishes the foundation required for branch creation and checkout.

**Status: Completed**

---

## Phase 13 — Branches

Introduce multiple lines of development.

Implement:

```bash
mini-git branch
mini-git branch <name>
```

Concepts:

* branch references
* branch creation
* branch listing
* current branch
* branch tips

Model:

```text
           Commit A

          /        \

     main           feature

       │               │

       ▼               ▼

   Commit B         Commit C
```

**Status: Next**

---

## Phase 14 — Checkout

Switch repository state.

Implement:

```bash
mini-git checkout <branch>
```

Topics:

* changing `HEAD`
* resolving branch references
* restoring Trees
* updating Working Tree
* updating Index
* safety checks

**Status: Future**

---

## Phase 15 — Diff

Compare repository states.

Implement:

```bash
mini-git diff

mini-git diff <commit>

mini-git diff <commit> <commit>
```

Potential comparisons:

```text
Working Tree ↔ Index

Index ↔ HEAD

Commit ↔ Commit
```

**Status: Future**

---

## Phase 16 — Merge

Combine histories.

Implement:

* fast-forward merge
* common ancestor discovery
* three-way merge
* merge commits
* multiple parents

The Commit model will evolve from:

```text
Commit
└── parent
```

to:

```text
Commit
├── parent
└── parent
```

**Status: Future**

---

## Phase 17 — Conflict Handling

Handle situations where changes cannot be merged automatically.

Implement:

* conflict detection
* conflict representation
* conflict markers
* merge state
* conflict resolution
* merge completion

Example:

```text
<<<<<<< HEAD

current version

=======

incoming version

>>>>>>> feature
```

**Status: Future**

---

## Phase 18 — Tags

Introduce named references to specific objects.

Implement:

```bash
mini-git tag <name>
```

Topics:

* lightweight tags
* tag references
* tag lookup
* tagged commits

This builds naturally on the Reference abstraction created in Phase 12.

**Status: Future**

---

## Phase 19 — Repository Maintenance & Integrity

Make the repository self-analyzing.

Implement concepts such as:

* reachability
* unreachable objects
* repository consistency
* object validation
* reference validation
* garbage-collection concepts
* repository cleanup

Introduce the foundations for:

```bash
mini-git fsck
```

**Status: Future**

---

## Phase 20 — Educational Intelligence & Portfolio

The final phase combines the project's unique educational layer with portfolio-quality finishing.

### Educational commands

Implement:

```bash
mini-git inspect
mini-git explain
mini-git graph
mini-git stats
mini-git fsck
```

### `inspect`

Expose object internals.

### `explain`

Explain what Mini Git is doing internally.

Example:

```bash
mini-git explain add main.cpp
```

could show:

```text
1. Read file
2. Create Blob
3. Serialize Blob
4. Calculate SHA-256
5. Store object
6. Update Index
```

### `graph`

Visualize history:

```text
* Commit C
|
* Commit B
|
* Commit A
```

and eventually branches/merges.

### `stats`

Display repository statistics:

* object count
* Blob count
* Tree count
* Commit count
* storage usage
* tracked files
* branches

### `fsck`

Validate:

* objects
* references
* Trees
* Commits
* parent relationships
* reachability

### Portfolio finalization

The final stage also includes:

* complete test suite
* architecture documentation
* design documentation
* polished README
* demonstrations
* benchmark results
* clean repository history
* release preparation
* resume description
* interview explanation

**Status: Future**

---

# Roadmap at a Glance

```text
 0  Git Concepts

 │

 1  Project Foundation

 │

 2  Repository Initialization

 │

 3  Hashing

 │

 4  Object Model

 │

 5  Blob Objects

 │

 6  Object Database

 │

 7  Trees

 │

 8  Index / Staging

 │

 9  Status

 │

10  Commits

 │

11  Log / History

 │

12  References & HEAD        ← COMPLETED

 │

13  Branches                 ← CURRENT

 │

14  Checkout

 │

15  Diff

 │

16  Merge

 │

17  Conflict Handling

 │

18  Tags

 │

19  Repository Maintenance

 │

20  Educational Intelligence
    + Portfolio Finalization
```

---

# What Makes Mini Git Different?

Mini Git is intentionally more than a collection of Git commands.

The project has three layers of purpose.

## Layer 1 — Version Control

Implement the fundamental mechanics:

```text
Objects

Storage

Index

Commits

History

References

Branches

Checkout

Diff

Merge
```

## Layer 2 — Systems Engineering

Demonstrate:

```text
C++20

CMake

Filesystem APIs

Binary I/O

Serialization

Hashing

Persistence

Testing

Architecture

Error Handling
```

## Layer 3 — Educational Intelligence

Expose the internal system:

```text
inspect

explain

graph

stats

fsck
```

This third layer is what can make Mini Git stand out as a portfolio project.

Instead of simply saying:

> "I implemented a Git clone."

the project demonstrates:

> "I implemented a content-addressable version-control system and built tools that expose and explain its internal data model."

---

# Engineering Principles

Mini Git follows several engineering principles.

## Separation of Concerns

Each subsystem should have one clear responsibility.

## Immutability

Objects should be treated as immutable once stored.

Changing a file should create a new Blob rather than modifying an existing Blob.

## Determinism

Equivalent states should produce deterministic serialized representations.

## Explicit State

Repository state should be represented explicitly through:

```text
Working Tree

Index

HEAD

References

Objects
```

## Testability

Core logic should be testable independently from the CLI.

## Error Handling

Invalid repository state and filesystem failures should result in clear errors.

## Incremental Architecture

New features should be built on existing abstractions instead of bypassing them.

---

# Technology Stack

| Technology                | Purpose                        |
| ------------------------- | ------------------------------ |
| C++20                     | Main implementation language   |
| CMake                     | Build system                   |
| OpenSSL 3                 | SHA-256 hashing                |
| CTest                     | Automated testing              |
| Git                       | Version control                |
| Unix-like filesystem APIs | Repository and file operations |

---

# Project Structure

```text
mini-git/

│

├── CMakeLists.txt

├── README.md

├── LICENSE

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
│   └── Status.hpp

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
│   ├── CommitTests.cpp
│   ├── RepositoryTests.cpp
│   ├── ReferenceTests.cpp
│   └── LogTests.cpp

│

└── docs/
```

The structure will evolve as new subsystems are introduced.

---

# Limitations

Mini Git is not Git.

It currently does not provide:

* Git repository compatibility
* Git object compatibility
* Git's binary Index format
* Git's exact object format
* Git's complete reference implementation
* distributed networking
* remotes
* push/pull
* GitHub integration

These features are outside the project's core educational goal.

The project focuses first on understanding **local version-control internals**.

---

# Learning Outcomes

By completing Mini Git, the project should provide practical understanding of:

### C++

* C++20
* RAII
* classes and interfaces
* const correctness
* STL containers
* filesystem APIs
* error handling
* modular design

### Systems

* filesystem representation
* persistent storage
* binary I/O
* object databases
* state management
* serialization

### Algorithms & Data Structures

* hashing
* trees
* graphs
* recursion
* graph traversal
* ancestry
* reachability
* three-way merge

### Software Engineering

* CMake
* testing
* Git workflows
* architecture
* documentation
* incremental development
* debugging
* refactoring

### Version Control

* staging
* snapshots
* objects
* commits
* references
* `HEAD`
* branches
* history
* checkout
* diff
* merge
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

The end goal is a system where a user can:

```text
create repository

      ↓

stage files

      ↓

create objects

      ↓

build Trees

      ↓

create commits

      ↓

traverse history

      ↓

create branches

      ↓

switch branches

      ↓

compare states

      ↓

merge histories

      ↓

resolve conflicts

      ↓

inspect repository

      ↓

analyze repository

      ↓

verify repository integrity
```

while also being able to ask:

```text
"What exactly happened internally?"
```

and have Mini Git explain it.

That is the central idea of the project.

---

# Status

**Current Phase:** 12 / 20

**Current subsystem:** References & HEAD

**Next subsystem:** Branches

**Language:** C++20

**Build System:** CMake

**Hashing:** SHA-256 / OpenSSL EVP

**Testing:** CTest

**Project Type:** Systems Programming / Version Control / Educational Infrastructure

---

## License

See `LICENSE` for the project's license and usage terms.
