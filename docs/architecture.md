# Mini Git Architecture

## Overview

Mini Git is an educational version control system written in modern C++.

The project is designed to demonstrate how a Git-like version control system can be built from fundamental concepts such as:

* filesystem operations
* content-addressable storage
* hashing
* objects
* repositories
* staging
* commits
* references
* branches
* history

Mini Git is **not intended to be a drop-in replacement for Git** and does not aim for compatibility with Git's complete internal format.

Instead, the project focuses on understanding the architecture and engineering principles behind a distributed version control system.

---

# High-Level Architecture

The planned architecture is:

```text
                         mini-git CLI
                              │
                              ▼
                       Command Parser
                              │
                              ▼
                        Command Layer
                              │
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
         Working Tree       Index         Repository
                              │               │
                              │               ▼
                              │          Object Database
                              │               │
                              │        ┌──────┼──────┐
                              │        ▼      ▼      ▼
                              │      Blob    Tree   Commit
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

The architecture is being implemented incrementally.

Only the components required for the current development phase are considered implemented.

---

# Repository Structure

Mini Git currently follows this structure:

```text
mini-git/
│
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
│   └── FileReader.hpp
│
├── src/
│   ├── main.cpp
│   ├── Repository.cpp
│   ├── Hash.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   └── FileReader.cpp
│
├── tests/
│   ├── HashTests.cpp
│   ├── ObjectTests.cpp
│   ├── FileReaderTests.cpp
│   └── BlobTests.cpp
│
└── docs/
    └── architecture.md
```

The `include/` directory contains public class declarations.

The `src/` directory contains implementations.

The `tests/` directory contains automated tests for individual components and their interactions.

The `docs/` directory contains project documentation and architectural information.

---

# Repository

The `Repository` class represents a Mini Git repository.

Currently, repository initialization creates:

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

The repository component is responsible for locating and creating repository-specific filesystem structures.

---

# Working Tree

The working tree is the collection of actual files and directories being managed by Mini Git.

For example:

```text
project/
├── main.cpp
├── README.md
└── src/
    └── app.cpp
```

These files exist outside `.mini-git/`.

The working tree represents the user's current filesystem state.

Later phases will compare the working tree against the staging area and the latest commit.

---

# Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The hashing layer is implemented by the `Hash` class.

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

The hash depends entirely on the input data.

Therefore:

```text
same input
    ↓
same hash
```

while:

```text
different input
    ↓
different hash
```

The hash function is provided through OpenSSL's cryptographic library.

---

# Object Model

Mini Git uses an object-oriented representation for repository objects.

The base abstraction is:

```text
Object
  │
  ├── Blob
  │
  ├── Tree
  │
  └── Commit
```

The base `Object` class defines the common serialization interface:

```cpp
virtual std::string serialize() const = 0;
```

Every object must therefore provide a serialized representation.

This creates a common pipeline:

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
Hash
   │
   ▼
Object ID
```

---

# Blob

A Blob represents file contents.

Conceptually:

```text
File
 │
 ▼
Blob
 │
 └── file contents
```

A Blob does not need to know the original filename.

For example:

```text
main.cpp
```

might contain:

```cpp
int main() {
    return 0;
}
```

The Blob represents the content:

```text
int main() {
    return 0;
}
```

The filename is stored separately by Tree objects.

This separation is important because the same content can be referenced by multiple filenames without requiring duplicate object data.

---

# File Reading and Blob Objects

Mini Git separates filesystem file reading from object representation.

The `FileReader` component is responsible for reading the exact bytes of a filesystem file.

The current workflow is:

```text
File
 │
 ▼
FileReader
 │
 ▼
File Contents
 │
 ▼
Blob::from_file()
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

This separation keeps responsibilities clear:

```text
FileReader
    → reads bytes from the filesystem

Blob
    → represents file content as an object

Hash
    → generates the object identifier
```

---

# FileReader

`FileReader` provides a simple abstraction for reading files.

Its interface is:

```cpp
class FileReader {
public:
    static std::string read(const std::filesystem::path& path);
};
```

Files are opened in binary mode.

This is important because Mini Git must be able to represent arbitrary files, not only text files.

For example, a file may contain:

```text
00 01 7F FF
```

A binary-safe reader must preserve those bytes exactly.

The implementation therefore uses:

```cpp
std::ifstream file(path, std::ios::binary);
```

The contents are then stored in a `std::string`.

Although `std::string` is commonly associated with text, it can safely contain arbitrary byte sequences, including null bytes.

Therefore:

```text
binary file
    │
    ▼
FileReader
    │
    ▼
exact byte sequence
```

No text conversion is intentionally performed.

---

# Blob::from_file()

A Blob can be created directly from a filesystem file using:

```cpp
Blob::from_file()
```

The internal flow is:

```text
Blob::from_file(path)
        │
        ▼
FileReader::read(path)
        │
        ▼
File contents
        │
        ▼
Blob
```

This keeps the responsibilities separated.

`FileReader` handles filesystem input.

`Blob` handles object representation.

The Blob class therefore does not need to implement file-reading logic itself.

---

# Blob Serialization

Mini Git currently serializes Blob objects using the following simplified format:

```text
blob <size>\0<content>
```

For example, the content:

```text
Hello Mini Git!
```

has 15 bytes.

Its serialized representation is conceptually:

```text
blob 15\0Hello Mini Git!
```

The `\0` represents an actual null byte.

The size is calculated from the byte count of the content:

```cpp
content_.size()
```

This is important because the object format must describe the exact number of bytes contained in the object.

The serialization is intentionally inspired by Git's object model but is **not Git-compatible**.

Mini Git uses this simplified representation to make the object model easier to understand.

---

# File → Object ID Pipeline

The complete Phase 5 workflow is:

```text
                  Filesystem
                      │
                      ▼
                  FileReader
                      │
                      ▼
                  File Bytes
                      │
                      ▼
                     Blob
                      │
                      ▼
                  serialize()
                      │
                      ▼
              Serialized Blob
                      │
                      ▼
                   SHA-256
                      │
                      ▼
                  Object ID
```

For example:

```text
hello.txt
    │
    │ contains
    ▼
"Hello Mini Git!"
    │
    ▼
Blob
    │
    ▼
"blob 15\0Hello Mini Git!"
    │
    ▼
SHA-256
    │
    ▼
object identifier
```

The object identifier is therefore based on the serialized object rather than simply the raw file contents.

This distinction becomes important when the object model expands to Trees and Commits.

---

# Tree

A Tree represents a directory-like structure.

A Tree contains entries describing other objects.

Each entry currently contains:

```text
name
object_id
is_tree
```

Conceptually:

```text
Tree
├── main.cpp → Blob
├── README.md → Blob
└── src/ → Tree
```

The relationship can be visualized as:

```text
                    Tree
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       Blob       Blob         Tree
      main.cpp   README.md      │
                                ├── Blob
                                └── Blob
```

Trees therefore connect individual objects into a filesystem hierarchy.

The filename belongs to the Tree entry, not to the Blob.

---

# Commit

A Commit represents a snapshot of repository state.

The current Commit contains:

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

The commit points to a Tree.

The Tree points to Blobs and other Trees.

Therefore the object graph becomes:

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

A commit may also point to a previous commit:

```text
Commit C
   │
   └── parent → Commit B
                    │
                    └── parent → Commit A
```

This creates the history chain.

---

# Content-Addressable Storage

Mini Git is designed around the idea of content-addressable storage.

Instead of identifying an object using a human-generated numeric ID, the object identifier is derived from its contents.

Conceptually:

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

For example:

```text
Serialized Object
       │
       ▼
SHA-256
       │
       ▼
a7c9...e21f
```

The resulting identifier can later be used to retrieve the object.

This creates an important property:

```text
same object content
        ↓
same object ID
```

and therefore enables object deduplication.

---

# Object Database

The Object Database is responsible for persistent storage of repository objects.

It is not yet implemented.

The planned structure is:

```text
.mini-git/
└── objects/
    ├── ...
    ├── ...
    └── ...
```

The target workflow is:

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
   │
   ▼
.mini-git/objects/
```

The Object Database will eventually provide functionality for:

* storing objects
* checking whether an object already exists
* retrieving objects
* addressing objects by their hash
* separating object persistence from object representation

This subsystem will be implemented in **Phase 6**.

---

# Index / Staging Area

The Index is the staging area between the working tree and the repository.

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

The Index will eventually record which object IDs correspond to staged paths.

For example:

```text
Index
├── main.cpp  → Blob A
├── README.md → Blob B
└── src/app.cpp → Blob C
```

The Index is not yet implemented.

It will be introduced in a later phase.

---

# HEAD

`HEAD` identifies the currently checked-out reference.

During repository initialization, Mini Git creates:

```text
HEAD
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

This allows Mini Git to determine the current branch and, eventually, the current commit.

Branch management will be implemented in later phases.

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

A branch reference will contain an object ID pointing to a commit.

Conceptually:

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

This makes branches lightweight pointers into the commit graph.

---

# Commit History

Once commits and references are fully implemented, history will form a directed graph.

A simple linear history:

```text
A ← B ← C
          ↑
         main
```

A branching history:

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

Later phases will implement history traversal and visualization.

---

# Testing Architecture

Mini Git uses automated tests for individual subsystems.

Current test programs include:

```text
HashTests.cpp
    │
    ├── empty input
    ├── known SHA-256 values
    ├── deterministic hashing
    ├── different inputs
    └── binary data

ObjectTests.cpp
    │
    ├── Blob serialization
    ├── Tree serialization
    ├── Commit serialization
    └── initial commit serialization

FileReaderTests.cpp
    │
    ├── text-file reading
    ├── binary-file reading
    ├── null-byte preservation
    └── missing-file errors

BlobTests.cpp
    │
    ├── Blob creation from files
    ├── Blob serialization
    └── binary-file Blob handling
```

The tests are registered with CTest.

The complete test suite can be executed using:

```bash
ctest --test-dir build
```

---

# Component Relationships

The current component relationships are:

```text
                 Repository
                      │
                      │
                      ▼
               .mini-git/
                      │
                      │
       ┌──────────────┴──────────────┐
       │                             │
       ▼                             ▼
    Objects                        Refs
       │                             │
       │                             ▼
       │                           HEAD
       │
       └── future Object Database


Filesystem File
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
      Hash
       │
       ▼
   Object ID
```

The Tree and Commit layers build on the same object abstraction:

```text
Object
 │
 ├── Blob
 │     └── file contents
 │
 ├── Tree
 │     └── object references
 │
 └── Commit
       └── snapshot metadata
```

---

# Current Data Flow

At the end of Phase 5, Mini Git can perform the following conceptual operation:

```text
Real File
    │
    ▼
FileReader
    │
    ▼
Raw File Bytes
    │
    ▼
Blob
    │
    ▼
Blob Serialization
    │
    ▼
SHA-256
    │
    ▼
Object ID
```

The resulting object ID can be calculated deterministically.

However, the object is **not yet persisted in `.mini-git/objects/`**.

Persistent storage is the responsibility of the Object Database subsystem introduced in Phase 6.

---

# Current Implementation Status

At the end of Phase 5:

| Component                 | Status          |
| ------------------------- | --------------- |
| Project structure         | Implemented     |
| CMake build system        | Implemented     |
| Git/GitHub integration    | Implemented     |
| Repository initialization | Implemented     |
| `.mini-git/` directory    | Implemented     |
| `HEAD` initialization     | Implemented     |
| SHA-256 hashing           | Implemented     |
| Object abstraction        | Implemented     |
| Blob                      | Implemented     |
| Tree                      | Implemented     |
| Commit                    | Implemented     |
| FileReader                | Implemented     |
| Binary-safe file reading  | Implemented     |
| `Blob::from_file()`       | Implemented     |
| Blob serialization        | Implemented     |
| File → Blob pipeline      | Implemented     |
| File → Object ID pipeline | Implemented     |
| Hash tests                | Implemented     |
| Object tests              | Implemented     |
| FileReader tests          | Implemented     |
| Blob tests                | Implemented     |
| Object database           | Not implemented |
| Persistent object storage | Not implemented |
| Object retrieval          | Not implemented |
| Index / staging area      | Not implemented |
| `add`                     | Not implemented |
| `status`                  | Not implemented |
| Real commit command       | Not implemented |
| Branch management         | Not implemented |
| Checkout                  | Not implemented |
| Diff                      | Not implemented |
| Merge                     | Not implemented |
| Tags                      | Not implemented |

---

# Planned Architecture

The final architecture is expected to evolve toward:

```text
                            CLI
                             │
                             ▼
                       Command Parser
                             │
                             ▼
                       Command Layer
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
          ▼                  ▼                  ▼
    Working Tree           Index            Repository
          │                  │                  │
          │                  │                  ▼
          │                  │            Object Database
          │                  │                  │
          │                  │          ┌───────┼───────┐
          │                  │          ▼       ▼       ▼
          │                  │        Blob     Tree   Commit
          │                  │
          │                  ▼
          │                Status
          │
          ▼
        Diff
                             │
                             ▼
                        References
                             │
                      ┌──────┴──────┐
                      ▼             ▼
                    HEAD          Branches
                                      │
                                      ▼
                                   Commits
```

Additional educational tools are planned:

```text
mini-git inspect
mini-git graph
mini-git explain
mini-git stats
mini-git fsck
```

These commands are intended to make Mini Git's internal behavior visible and easier to understand.

---

# Design Principles

Mini Git follows several architectural principles.

## 1. Separation of Responsibilities

Each component should have a focused responsibility.

For example:

```text
FileReader
    → reads files

Blob
    → represents file content

Hash
    → calculates object IDs

Repository
    → manages repository structure
```

Components should not unnecessarily duplicate each other's responsibilities.

---

## 2. Deterministic Behavior

Given identical input, Mini Git should produce identical results.

For example:

```text
same serialized object
        ↓
same SHA-256
        ↓
same object ID
```

This property is fundamental to content-addressable storage.

---

## 3. Binary Safety

Filesystem data must be treated as bytes rather than assuming everything is text.

This is why file reading uses binary mode.

---

## 4. Testability

Core functionality should be testable independently.

For example:

```text
Hash
FileReader
Blob
Tree
Commit
```

can each be tested without requiring the entire application to run.

---

## 5. Incremental Development

Mini Git is implemented subsystem by subsystem.

Each phase introduces a small amount of functionality while building on previously implemented components.

This reduces complexity and makes failures easier to isolate.

---

# Phase Roadmap

The major development stages are:

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

# End of Phase 5

At the end of Phase 5, Mini Git has established the complete conceptual pipeline from a real filesystem file to a deterministic object identifier:

```text
Filesystem
    │
    ▼
FileReader
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

The next major subsystem is **persistent object storage**.

Phase 6 will introduce the Object Database and connect object identifiers to actual stored objects inside:

```text
.mini-git/objects/
```

This will transform Mini Git from a system that can calculate object IDs into one that can actually **store and retrieve repository objects**.
