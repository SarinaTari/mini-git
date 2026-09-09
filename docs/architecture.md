# Mini Git Architecture

## Overview

Mini Git is a Git-inspired version control system implemented from scratch in C++20.

The project is designed as an educational systems-programming project.

The goal is not to reproduce the entire Git implementation, but to build a simplified version-control system while understanding the internal mechanisms that make Git work.

Mini Git is developed incrementally. Each subsystem is designed, implemented, tested, and documented before more complex functionality is built on top of it.

> **Note:** Mini Git is inspired by Git but is not intended to be a replacement for Git or a fully compatible Git implementation.

---

# High-Level Architecture

The planned system is organized into several logical layers:

```text
                  mini-git CLI
                       │
                       ▼
                  Command Parser
                       │
                       ▼
                 Command Layer
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
       Working       Index       Repository
        Tree            │            │
                        │            ▼
                        │        Object DB
                        │            │
                        │      ┌─────┼─────┐
                        │      ▼     ▼     ▼
                        │     Blob  Tree  Commit
                        │
                        ▼
                      Status
                        │
                        ▼
                    References
                        │
                        ▼
                       HEAD
```

Not all components are implemented yet.

The architecture will evolve as additional functionality is introduced.

---

# Current Architecture

At the end of Phase 4, the project contains:

```text
mini-git/
├── include/
│   ├── Repository.hpp
│   ├── Hash.hpp
│   ├── Object.hpp
│   ├── Blob.hpp
│   ├── Tree.hpp
│   └── Commit.hpp
│
├── src/
│   ├── main.cpp
│   ├── Repository.cpp
│   ├── Hash.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   └── Commit.cpp
│
├── tests/
│   ├── HashTests.cpp
│   └── ObjectTests.cpp
│
└── docs/
    └── architecture.md
```

The current implementation can be represented as:

```text
                    Mini Git
                       │
          ┌────────────┴────────────┐
          ▼                         ▼
      Repository                   Hash
          │                         │
          ▼                         ▼
    .mini-git/                   SHA-256
                                   
                       │
                       ▼
                    Object
                 ┌────┼────┐
                 ▼    ▼    ▼
               Blob  Tree Commit
```

The object database has not yet been implemented.

---

# Command Layer

The command-line entry point is:

```text
src/main.cpp
```

The `main()` function currently handles:

1. Reading command-line arguments
2. Determining the requested command
3. Creating required application objects
4. Handling command-level errors
5. Returning an appropriate exit status

Currently supported commands include:

```bash
mini-git
mini-git --version
mini-git init
```

As the project grows, command parsing and command execution will be separated into more dedicated components.

The planned command architecture is:

```text
CLI
 │
 ▼
Command Parser
 │
 ▼
Command Layer
 │
 ├── init
 ├── add
 ├── status
 ├── commit
 ├── log
 ├── branch
 ├── checkout
 └── ...
```

---

# Repository

The `Repository` class represents a Mini Git repository.

Current interface:

```cpp
class Repository {
public:
    explicit Repository(const std::filesystem::path& root);

    void initialize();

private:
    std::filesystem::path root_;
    std::filesystem::path git_dir_;
};
```

The class currently stores:

* The repository root
* The path to the `.mini-git` directory

The repository abstraction keeps repository-specific filesystem operations separate from the command-line entry point.

This separation will become increasingly important as repository functionality grows.

---

# Repository Initialization

Running:

```bash
mini-git init
```

initializes a repository in the current working directory.

The current initialization flow is:

```text
CLI
 │
 ▼
main.cpp
 │
 ▼
Repository
 │
 ▼
Repository::initialize()
 │
 ▼
.mini-git/
```

The repository structure created by initialization is:

```text
.mini-git/
├── objects/
├── refs/
│   └── heads/
└── HEAD
```

At this stage:

* `objects/` exists but contains no stored objects.
* `refs/heads/` exists but contains no branch reference files.
* `HEAD` points symbolically to `refs/heads/main`.

---

# `.mini-git`

`.mini-git` is Mini Git's internal metadata directory.

It is intentionally named differently from Git's `.git` directory.

The directory contains information needed to represent repository state.

Project files remain outside `.mini-git` as part of the working tree.

Conceptually:

```text
Project
│
├── Working Tree
│   ├── main.cpp
│   ├── README.md
│   └── ...
│
└── .mini-git
    ├── objects
    ├── refs
    └── HEAD
```

The `.mini-git` directory is managed by Mini Git and should not be treated as normal project content.

---

# Working Tree

The working tree consists of the actual files and directories being version-controlled.

For example:

```text
project/
├── main.cpp
├── README.md
├── src/
│   └── app.cpp
└── .mini-git/
```

The working tree represents the current state of the files on disk.

Later, Mini Git will compare the working tree against:

* The index
* The current commit

These comparisons will allow commands such as:

```bash
mini-git status
mini-git diff
```

---

# Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The hashing functionality is isolated behind the `Hash` abstraction.

The current relationship is:

```text
Mini Git
   │
   ▼
Hash::sha256()
   │
   ▼
OpenSSL EVP
   │
   ▼
SHA-256
```

The interface is defined in:

```text
include/Hash.hpp
```

and implemented in:

```text
src/Hash.cpp
```

OpenSSL provides the cryptographic implementation rather than Mini Git implementing SHA-256 manually.

This keeps cryptographic functionality isolated from the rest of the version-control system.

---

## SHA-256 Output

SHA-256 produces:

```text
256 bits
   ↓
32 bytes
   ↓
64 hexadecimal characters
```

Mini Git represents the resulting hash as a 64-character hexadecimal string.

For example:

```text
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
```

This value will eventually be used as an object identifier.

---

# Content-Addressable Storage

Mini Git is designed around content-addressable storage.

Instead of identifying an object primarily using a filename or sequential database ID, its content determines its object identifier.

The conceptual flow is:

```text
Object Content
      │
      ▼
    SHA-256
      │
      ▼
  Object ID
      │
      ▼
Object Database
```

Therefore:

```text
same content
     ↓
same object ID
```

while changing the content should produce a different object ID with overwhelming probability.

This provides the foundation for object identity and deduplication.

Actual object storage will be implemented in a later phase.

---

# Object Model

Mini Git represents repository data using three primary object types:

```text
Object
├── Blob
├── Tree
└── Commit
```

The common interface is defined by the `Object` abstraction:

```cpp
class Object {
public:
    virtual ~Object() = default;

    virtual std::string serialize() const = 0;
};
```

Every object can therefore produce a serialized representation.

The planned relationship is:

```text
Object
   │
   ├── serialize()
   │
   ▼
Serialized Data
   │
   ▼
SHA-256
   │
   ▼
Object ID
```

Object storage will connect this model to the repository's object database in a later phase.

---

# Blob

A blob represents file content.

Conceptually:

```text
File Contents
      │
      ▼
    Blob
      │
      ▼
 Object ID
```

A blob represents the content itself rather than the filename.

For example:

```text
hello.txt → "Hello Mini Git!"
test.txt  → "Hello Mini Git!"
```

Both files can refer to the same blob because their contents are identical.

This is an important consequence of content-addressable storage.

The current `Blob` class stores its content and serializes it directly.

```text
Blob
 │
 └── content
```

The filename is not part of the blob's responsibility.

---

# Tree

A tree represents directory structure.

Conceptually:

```text
Directory
    │
    ├── file1 → Blob
    ├── file2 → Blob
    └── src/  → Tree
```

A tree entry contains:

* A name
* An object identifier
* Information about whether the entry represents a tree or blob

The current simplified representation is:

```cpp
struct TreeEntry {
    std::string name;
    std::string object_id;
    bool is_tree;
};
```

The tree therefore connects filesystem names to objects:

```text
Filename
   +
Object Type
   +
Object ID
       │
       ▼
     Tree
```

The current Mini Git tree serialization is intentionally simple and human-readable.

It is not intended to exactly reproduce Git's internal binary tree format.

---

# Commit

A commit represents a repository snapshot together with metadata and history information.

The current simplified commit structure contains:

```text
Commit
├── tree
├── parent
├── author
└── message
```

Conceptually:

```text
Commit
  │
  ├── tree ──────► Tree
  │                  │
  │                  ├── file → Blob
  │                  └── directory → Tree
  │
  └── parent ─────► Previous Commit
```

The first commit has no parent.

Later commits reference their previous commit.

For example:

```text
Commit C
   │
   ▼
Commit B
   │
   ▼
Commit A
```

This forms the foundation of the commit history graph.

---

# Object Relationships

The three object types work together:

```text
                    Commit
                       │
                       │ tree
                       ▼
                     Tree
                  ┌────┴────┐
                  │         │
                  ▼         ▼
                Blob      Tree
                          │
                          ▼
                         Blob
```

A more complete example:

```text
Commit
   │
   └── tree
         │
         ├── main.cpp ─────► Blob
         │
         ├── README.md ────► Blob
         │
         └── src ──────────► Tree
                                │
                                └── app.cpp ─► Blob
```

This structure allows unchanged objects to be reused between different snapshots.

---

# Object Serialization

Every Mini Git object provides a serialized representation through:

```cpp
std::string serialize() const
```

The conceptual pipeline is:

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

This design separates:

* Object structure
* Object serialization
* Hash generation
* Object storage

Each responsibility can therefore evolve independently.

---

# Index

The index is the staging area.

The conceptual workflow is:

```text
Working Tree
      │
      │ mini-git add
      ▼
    Index
      │
      │ mini-git commit
      ▼
   Repository
```

The index has **not yet been implemented**.

It will eventually record the files selected for the next commit.

The planned relationship is:

```text
Working Tree
      │
      │ add
      ▼
    Index
      │
      │ commit
      ▼
    Tree
```

The index will be introduced after object storage and tree construction are sufficiently developed.

---

# References

The repository contains:

```text
.mini-git/refs/heads/
```

This directory will contain branch references.

For example:

```text
refs/
└── heads/
    ├── main
    └── feature
```

A branch reference will eventually contain or identify a commit.

At the current stage, no branch reference file is created because commits have not yet been implemented at the repository level.

---

# HEAD

The repository contains:

```text
.mini-git/HEAD
```

The initial contents are:

```text
ref: refs/heads/main
```

This represents a symbolic reference.

Conceptually:

```text
HEAD
 │
 ▼
refs/heads/main
 │
 ▼
latest commit
```

At the current stage:

```text
HEAD
 │
 ▼
refs/heads/main
 │
 ▼
(no commit yet)
```

Eventually, when commits and branch references are implemented:

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

---

# Complete Version-Control Data Flow

The intended staging and commit workflow is:

```text
                  Working Tree
                       │
                       │ mini-git add
                       ▼
                     Index
                       │
                       │ mini-git commit
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

The object database provides the persistent storage underneath the object model:

```text
                  Object Database
                         │
          ┌──────────────┼──────────────┐
          ▼              ▼              ▼
        Blob            Tree          Commit
```

The complete conceptual system is therefore:

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

while the objects themselves are stored through the object database.

---

# Object Database

The object database is planned but **not yet implemented**.

It will eventually provide persistent storage and retrieval for:

```text
Object Database
      │
      ├── Blob
      ├── Tree
      └── Commit
```

The planned storage flow is:

```text
Object
   │
   ▼
Serialize
   │
   ▼
Hash
   │
   ▼
Object ID
   │
   ▼
Object Database
```

The object database will be responsible for operations such as:

* Storing objects
* Reading objects
* Checking whether an object exists
* Retrieving objects by object ID

This functionality will be introduced in a later phase.

---

# Planned Component Structure

As the project develops, the codebase is expected to evolve toward modules similar to:

```text
include/
├── Repository.hpp
├── Hash.hpp
├── Object.hpp
├── Blob.hpp
├── Tree.hpp
├── Commit.hpp
├── ObjectDatabase.hpp
├── Index.hpp
├── Reference.hpp
├── Head.hpp
└── CommandParser.hpp
```

and:

```text
src/
├── main.cpp
├── Repository.cpp
├── Hash.cpp
├── Object.cpp
├── Blob.cpp
├── Tree.cpp
├── Commit.cpp
├── ObjectDatabase.cpp
├── Index.cpp
├── Reference.cpp
├── Head.cpp
└── CommandParser.cpp
```

This is a **planned architecture**, not the current implementation.

New components will only be introduced when their responsibilities become necessary.

---

# Design Principles

## Separation of Concerns

Each component should have a clear responsibility.

For example:

```text
Hash
    → generates object identifiers

Object
    → defines the common object interface

Blob
    → represents file content

Tree
    → represents directory structure

Commit
    → represents snapshots and history

ObjectDatabase
    → stores and retrieves objects

Repository
    → manages repository-level state

Index
    → manages staged files

Reference
    → manages branch references

HEAD
    → identifies the current repository position
```

A component should not unnecessarily take responsibility for unrelated operations.

---

## Standard C++

Mini Git primarily uses the C++ standard library.

External dependencies are introduced only when they provide a meaningful advantage.

Currently, OpenSSL is used for cryptographic hashing.

The project avoids implementing cryptographic algorithms manually because correctness and security are more important than reimplementing established cryptographic primitives.

---

## RAII

Resources should be managed using C++ lifetime semantics wherever practical.

This includes:

* File streams
* Memory
* Locks
* Other resources introduced later

As the codebase grows, resource-management patterns will be reviewed and improved where appropriate.

---

## Const Correctness

Functions that do not modify an object should be marked `const` where appropriate.

For example:

```cpp
std::string serialize() const;
```

This communicates that serialization should not modify the object.

---

## Error Handling

Operations that can fail should detect and report errors clearly.

Examples include:

* Repository already exists
* Repository does not exist
* File cannot be opened
* Object does not exist
* Invalid object
* Invalid reference
* Invalid command

The CLI should provide useful error messages rather than silently failing.

---

# Testing Architecture

Mini Git uses automated tests to verify individual components.

Current test structure:

```text
tests/
├── HashTests.cpp
└── ObjectTests.cpp
```

Hash tests verify:

* Known SHA-256 values
* Deterministic hashing
* Different inputs producing different hashes
* Binary data handling

Object tests verify:

* Blob serialization
* Tree serialization
* Commit serialization
* Initial commits without parents

CTest is used to execute the test suite.

The testing architecture will expand as repository functionality becomes more complex.

Future tests will cover:

* Repository initialization
* Object storage
* Object retrieval
* Tree construction
* Index behavior
* Status calculation
* Commit creation
* References
* Branches
* Checkout
* Diff
* Merge
* Conflict handling
* Repository integrity
* Error conditions
* Edge cases

---

# Current Implementation Status

At the end of Phase 4:

| Component                 | Status          |
| ------------------------- | --------------- |
| C++20 project structure   | Implemented     |
| CMake build system        | Implemented     |
| CLI executable            | Implemented     |
| `mini-git --version`      | Implemented     |
| `mini-git init`           | Implemented     |
| Repository abstraction    | Implemented     |
| `.mini-git/`              | Implemented     |
| `HEAD` initialization     | Implemented     |
| SHA-256 hashing           | Implemented     |
| OpenSSL integration       | Implemented     |
| Hash abstraction          | Implemented     |
| Object abstraction        | Implemented     |
| Blob                      | Implemented     |
| Tree                      | Implemented     |
| Commit                    | Implemented     |
| Object serialization      | Implemented     |
| Hash tests                | Implemented     |
| Object tests              | Implemented     |
| CTest integration         | Implemented     |
| Object database           | Not implemented |
| Persistent object storage | Not implemented |
| Repository discovery      | Not implemented |
| Index                     | Not implemented |
| `add`                     | Not implemented |
| `status`                  | Not implemented |
| Commit creation command   | Not implemented |
| `log`                     | Not implemented |
| Branch management         | Not implemented |
| Checkout                  | Not implemented |
| Diff                      | Not implemented |
| Merge                     | Not implemented |

---

# Current Limitations

The current implementation is intentionally incomplete.

The following major features are still planned:

* Repository discovery
* Object database
* Persistent object storage
* Index
* `mini-git add`
* `mini-git status`
* Repository-level commits
* `mini-git log`
* Branch management
* Checkout
* Diff
* Merge
* Conflict handling
* Tags
* Repository integrity checking
* Garbage collection concepts

The current Blob, Tree, and Commit classes represent the object model, but they are not yet connected to persistent repository storage.

---

# Future Architecture

The eventual architecture should resemble:

```text
                         CLI
                          │
                          ▼
                   Command Parser
                          │
                          ▼
                    Command Layer
                          │
          ┌───────────────┼────────────────┐
          │               │                │
          ▼               ▼                ▼
     Working Tree       Index         Repository
          │               │                │
          │               │                ▼
          │               │          Object Database
          │               │                │
          │               │       ┌────────┼────────┐
          │               │       ▼        ▼        ▼
          │               │     Blob      Tree    Commit
          │               │                         │
          │               │                         ▼
          │               │                    References
          │               │                         │
          └───────────────┼─────────────────────────┤
                          │                         │
                          ▼                         ▼
                        Status                    HEAD
```

The architecture may change as implementation experience reveals better designs.

Documentation will be updated whenever a major architectural decision changes.

---

# Educational Features

Mini Git will eventually include features specifically designed to expose its internal behavior.

Planned educational commands include:

```text
mini-git inspect
mini-git graph
mini-git explain
mini-git stats
mini-git fsck
```

These features are intended to make the internal mechanisms of the system observable.

For example:

```text
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
Calculate SHA-256
     │
     ▼
Update Index
```

These commands are part of Mini Git's educational purpose and differentiate the project from simply reproducing Git's command-line interface.

---

# Architectural Goal

The most important architectural goal is not to create the largest possible codebase.

The goal is to create a system where the relationships between the components are understandable.

The object model should eventually follow:

```text
Files
  ↓
Blobs
  ↓
Trees
  ↓
Commits
  ↓
References
  ↓
HEAD
```

while the staging workflow remains:

```text
Working Tree
      ↓
    Index
      ↓
   Commit
```

The object database connects these objects to persistent storage:

```text
Object
   ↓
Serialize
   ↓
Hash
   ↓
Object ID
   ↓
Object Database
```

Together, these mechanisms form the foundation of the Mini Git version-control system.
