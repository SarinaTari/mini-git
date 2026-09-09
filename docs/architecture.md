# Mini Git Architecture

## Overview

Mini Git is a Git-inspired version control system implemented from scratch in C++20.

The project is designed as an educational systems-programming project.

The goal is not to reproduce the entire Git implementation, but to build a simplified version-control system while understanding the internal mechanisms that make Git work.

---

# High-Level Architecture

The system is organized into several logical layers.

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

The architecture will grow incrementally as new phases are completed.

---

# Current Architecture

The current implementation contains a simple command-line layer and repository abstraction.

```text
main.cpp
    │
    ▼
Repository
    │
    ▼
std::filesystem
    │
    ▼
.mini-git/
```

The current source structure is:

```text
mini-git/
├── include/
│   └── Repository.hpp
├── src/
│   ├── main.cpp
│   └── Repository.cpp
├── tests/
└── docs/
```

---

# Command Layer

The command-line entry point is:

```text
src/main.cpp
```

The `main()` function is responsible for:

1. Reading command-line arguments
2. Determining which command was requested
3. Creating the required application objects
4. Handling command-level errors
5. Returning an appropriate exit status

Currently supported commands include:

```bash
mini-git
mini-git --version
mini-git init
```

As the project grows, command parsing and command execution will become more modular.

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

The repository abstraction prevents `main.cpp` from having to directly manage all repository filesystem operations.

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

---

# `.mini-git`

`.mini-git` is Mini Git's internal metadata directory.

It is intentionally named differently from Git's `.git` directory.

The directory contains information needed to represent repository state.

The working files of the project remain outside `.mini-git`.

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

---

# Objects Directory

The current repository contains:

```text
.mini-git/objects/
```

This directory is currently empty.

It will eventually contain the repository's content-addressed objects.

The planned object types are:

```text
Object Database
      │
      ├── Blob
      ├── Tree
      └── Commit
```

Objects will be identified by cryptographic hashes.

Object storage will be implemented in later phases.

---

# References

The repository currently contains:

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

A branch reference will eventually identify a commit.

At the current stage, no branch reference file is created because no commit exists yet.

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

The `main` branch will begin pointing to a commit after commit functionality is implemented.

---

# Working Tree

The working tree consists of the user's actual project files.

For example:

```text
project/
├── main.cpp
├── README.md
├── src/
│   └── app.cpp
└── .mini-git/
```

The working tree is separate from repository metadata.

Later, Mini Git will compare the working tree against:

* The index
* The current commit

This comparison will allow commands such as:

```bash
mini-git status
mini-git diff
```

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

The index has not yet been implemented.

It will eventually store information about the files that have been selected for the next commit.

---

# Object Model

Mini Git will use three primary object types.

```text
                Object Database
                      │
          ┌───────────┼───────────┐
          ▼           ▼           ▼
        Blob         Tree       Commit
          │           │           │
      File data   Directory    Metadata
                   structure    + history
```

## Blob

A blob represents file content.

Conceptually:

```text
file contents
      │
      ▼
    Blob
      │
      ▼
    Hash
```

Two identical file contents should produce the same object identifier.

---

## Tree

A tree represents directory structure.

Conceptually:

```text
Directory
    │
    ├── file1 → Blob
    ├── file2 → Blob
    └── src/  → Tree
```

Trees connect file names and directory structure to object identifiers.

---

## Commit

A commit represents a snapshot of the repository along with metadata.

A simplified commit relationship is:

```text
Commit
  │
  ├── tree → Tree
  │
  ├── parent → previous Commit
  │
  ├── author
  │
  └── message
```

Commits form a directed history graph.

```text
A → B → C → D
```

Branches can point to commits in this graph.

---

# Hashing

Hashing is one of the fundamental mechanisms behind Mini Git.

The planned object flow is:

```text
Object content
      │
      ▼
   Hashing
      │
      ▼
Object ID
```

The object identifier will be deterministic.

Therefore:

```text
same content
     ↓
same hash
```

while changing the content should produce a different hash.

Hashing will be implemented in a dedicated component rather than scattered throughout the repository code.

---

# Content-Addressable Storage

Mini Git will use content-addressable storage.

Instead of identifying an object primarily by a filename or database ID, the object's content determines its identifier.

Conceptually:

```text
Content
   │
   ▼
Hash
   │
   ▼
Object ID
   │
   ▼
Stored Object
```

This provides an important foundation for deduplication and object identity.

---

# References and History

Once commits exist, the relationship between references and commits will look approximately like:

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

A branch is therefore conceptually a movable reference to a commit.

Moving the branch forward does not require modifying every commit.

Instead:

```text
main → C
```

can become:

```text
main → D
```

while the existing commits remain unchanged.

---

# Complete Conceptual Data Flow

The intended version-control workflow is:

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

The object database sits underneath the object model:

```text
                  Object Database
                         │
          ┌──────────────┼──────────────┐
          ▼              ▼              ▼
        Blob            Tree          Commit
```

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

This is a planned architecture, not the current implementation.

Classes will only be introduced when their responsibilities become necessary.

---

# Design Principles

## Separation of Concerns

Each component should have a clear responsibility.

For example:

```text
Hash
    → generates object identifiers

ObjectDatabase
    → stores and retrieves objects

Repository
    → manages repository-level state

Index
    → manages staged files

Reference
    → manages branch references
```

A component should not unnecessarily take responsibility for unrelated operations.

---

## Standard C++

The project will primarily use the C++ standard library.

External dependencies will only be introduced when they provide a meaningful advantage, particularly for cryptographic functionality.

---

## RAII

Resources should be managed using C++ lifetime semantics wherever practical.

This includes:

* File streams
* Memory
* Locks
* Other resources introduced later

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

The CLI should provide useful error messages instead of silently failing.

---

# Current Limitations

The current architecture intentionally does not implement:

* Repository discovery
* Hashing
* Object storage
* Blobs
* Trees
* Commits
* Index
* Status
* Branches
* Checkout
* Diff
* Merge

These will be introduced incrementally.

---

# Future Architecture

The eventual system should resemble:

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

# Architectural Goal

The most important architectural goal is not to create the largest possible codebase.

The goal is to create a system where the relationship between the components is understandable:

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

Together, these concepts form the foundation of the Mini Git version-control system.
