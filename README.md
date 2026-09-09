# Mini Git

A Git-inspired version control system built from scratch in C++20.

Mini Git is an educational systems-programming project designed to explore how modern version control systems work internally.

Rather than simply using Git's commands or libraries, the project progressively implements the fundamental mechanisms behind a version control system, including hashing, content-addressable storage, objects, trees, commits, staging, references, and history.

> **Note:** Mini Git is an educational project inspired by Git. It is not intended to replace Git and does not aim for full Git compatibility.

---

## Goals

The primary goals of Mini Git are to understand and implement the core concepts behind modern version control systems:

* Content-addressable storage
* Cryptographic hashing
* Object databases
* Blob objects
* Tree objects
* Commit objects
* Staging areas
* References
* `HEAD`
* Branches
* Commit history
* Diffs
* Merging
* Repository integrity
* Basic version-control workflows

The project also emphasizes professional C++ development practices:

* C++20
* CMake
* Modular architecture
* Separation of concerns
* RAII
* Const correctness
* Error handling
* Automated testing
* Documentation
* Git-based development workflow

---

# Current Status

Mini Git is being developed incrementally through multiple implementation phases.

## Implemented

### Project Foundation

* C++20 project structure
* CMake build system
* Command-line executable
* Git-based development workflow
* Documentation structure

### Repository

* `mini-git --version`
* `mini-git init`
* Repository initialization
* `.mini-git/` metadata directory
* `objects/` directory
* `refs/heads/` directory
* Initial `HEAD` reference
* `HEAD` pointing to `main`
* Basic `Repository` abstraction

### Hashing

* SHA-256 hashing
* OpenSSL integration
* OpenSSL EVP-based hashing
* Deterministic hash generation
* Hexadecimal hash representation
* Binary-data hashing support

### Object Model

* Common `Object` abstraction
* `Blob` objects
* `Tree` objects
* `Commit` objects
* Object serialization
* Tree entries
* Commit parent relationships
* Initial commits without parents

### Testing

* Hash unit tests
* Object serialization tests
* Known SHA-256 test vectors
* Determinism tests
* Binary-data tests
* CTest integration

---

## Not Yet Implemented

The following major subsystems are planned for future phases:

* Repository discovery
* Persistent object database
* Object storage
* Object retrieval
* `mini-git add`
* Index / staging area
* `mini-git status`
* Repository-level commit creation
* `mini-git commit`
* `mini-git log`
* Reference management
* Branch management
* Checkout
* Diff
* Merge
* Conflict handling
* Tags
* Repository integrity checking
* Garbage collection concepts
* Performance benchmarking
* Extensive integration testing

---

# Educational Features

Mini Git will eventually include commands specifically designed to expose its internal behavior.

Planned educational commands include:

```text
mini-git inspect
mini-git graph
mini-git explain
mini-git stats
mini-git fsck
```

These commands are intended to make the internal architecture observable rather than simply reproducing Git's command-line interface.

For example:

```bash
mini-git explain add main.cpp
```

could eventually explain the internal process:

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
Store Object
     │
     ▼
Update Index
```

This educational layer is an important part of the project's purpose.

---

# Architecture

Mini Git is being developed as a layered version-control system.

The planned high-level architecture is:

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

The architecture will evolve as additional subsystems are introduced.

For a detailed description of the current and planned architecture, see:

```text
docs/architecture.md
```

---

# Core Concepts

Mini Git is built around several fundamental version-control concepts.

## Working Tree

The working tree is the collection of files and directories currently present on disk.

```text
Working Tree
├── main.cpp
├── README.md
└── src/
    └── app.cpp
```

It represents the current state of the project files.

---

## Index

The index is the staging area between the working tree and the repository.

The intended workflow is:

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

The index is planned but has not yet been implemented.

---

## Objects

Mini Git represents repository data using three primary object types:

```text
Object
├── Blob
├── Tree
└── Commit
```

### Blob

A blob represents file contents.

```text
File Contents
      │
      ▼
    Blob
```

A blob does not need to know the filename associated with its contents.

Identical file contents can therefore correspond to the same object.

---

### Tree

A tree represents directory structure.

```text
Tree
├── main.cpp  → Blob
├── README.md → Blob
└── src/      → Tree
                  │
                  └── app.cpp → Blob
```

Trees connect filenames and directory structure to object identifiers.

---

### Commit

A commit represents a repository snapshot together with metadata and history.

A simplified commit contains:

```text
Commit
├── tree
├── parent
├── author
└── message
```

Commits can reference previous commits:

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

# Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The hashing layer is isolated behind the `Hash` abstraction.

The current hashing pipeline is:

```text
Data
 │
 ▼
Hash::sha256()
 │
 ▼
OpenSSL EVP
 │
 ▼
SHA-256
 │
 ▼
64-character hexadecimal string
```

SHA-256 produces:

```text
256 bits
   ↓
32 bytes
   ↓
64 hexadecimal characters
```

For example:

```text
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
```

The resulting hash will eventually serve as an object identifier.

---

# Content-Addressable Storage

Mini Git is designed around content-addressable storage.

The basic concept is:

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

The same content produces the same object identifier.

Therefore:

```text
same content
     ↓
same object ID
```

This provides the foundation for object identity and deduplication.

Persistent object storage has not yet been implemented.

---

# Repository Structure

A Mini Git repository contains a hidden `.mini-git` directory:

```text
project/
├── .mini-git/
│   ├── objects/
│   ├── refs/
│   │   └── heads/
│   └── HEAD
├── source files...
└── other project files...
```

The `.mini-git` directory contains repository metadata and internal version-control information.

Project files remain in the working tree.

---

# Repository Initialization

A repository can currently be initialized with:

```bash
mini-git init
```

This creates:

```text
.mini-git/
├── objects/
├── refs/
│   └── heads/
└── HEAD
```

The initial `HEAD` contains:

```text
ref: refs/heads/main
```

This means that `HEAD` symbolically refers to the `main` branch.

At the current stage, `main` does not yet point to a commit because repository-level commit creation has not been implemented.

The `objects/` directory is currently empty because persistent object storage has not yet been implemented.

---

# Build

Mini Git uses CMake as its build system.

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

The main executable is generated at:

```text
build/mini-git
```

### macOS / Homebrew OpenSSL

The project uses OpenSSL for SHA-256 hashing.

On systems where CMake does not automatically locate the Homebrew installation, configure the project with:

```bash
cmake -S . -B build \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3
```

Then build:

```bash
cmake --build build
```

---

# Run

Run Mini Git with:

```bash
./build/mini-git
```

Expected output:

```text
Mini Git
```

---

# Check Version

```bash
./build/mini-git --version
```

Expected output:

```text
mini-git version 0.1.0
```

---

# Initialize a Test Repository

Create a separate directory for testing:

```bash
mkdir mini-git-test
cd mini-git-test
```

Then run Mini Git:

```bash
/path/to/mini-git/build/mini-git init
```

The result should be:

```text
mini-git-test/
└── .mini-git/
    ├── objects/
    ├── refs/
    │   └── heads/
    └── HEAD
```

The `HEAD` file should contain:

```text
ref: refs/heads/main
```

Running `init` again should report that a repository already exists.

---

# Testing

Mini Git uses automated tests to validate individual components.

Current tests include:

```text
tests/
├── HashTests.cpp
└── ObjectTests.cpp
```

The hash tests verify:

* Known SHA-256 values
* Deterministic hashing
* Different inputs producing different hashes
* Binary data handling

The object tests verify:

* Blob serialization
* Tree serialization
* Commit serialization
* Initial commits without parents

CTest is used to execute the test suite.

Run the tests with:

```bash
ctest --test-dir build
```

The project will eventually include both unit and integration tests covering repository-level behavior.

---

# Development Workflow

Mini Git itself is version-controlled using Git.

The development workflow is:

```text
Modify Code
    ↓
Build
    ↓
Run Tests
    ↓
Inspect Behavior
    ↓
Update Documentation
    ↓
Commit Changes
    ↓
Push to GitHub
```

Git manages the source code of Mini Git.

Mini Git will eventually manage separate test repositories.

---

# Design Philosophy

Mini Git is intentionally built incrementally.

Instead of immediately implementing a large collection of commands, the project first establishes the internal mechanisms required to support those commands.

The fundamental workflow is:

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
      │
      ▼
   Commit
      │
      ▼
   Branch
      │
      ▼
    HEAD
```

The object model underneath this workflow is:

```text
Files
  │
  ▼
Blobs
  │
  ▼
Trees
  │
  ▼
Commits
  │
  ▼
References
  │
  ▼
HEAD
```

Each subsystem is implemented and tested before more complex functionality is built on top of it.

---

# Why Build Mini Git?

Git is commonly used as a command-line tool without requiring users to understand its internal implementation.

Building a simplified version from scratch provides an opportunity to understand:

* How files become versioned objects
* How content-addressable storage works
* How cryptographic hashes identify data
* How objects can be reused
* How directory structures are represented
* How commits reference previous commits
* How branches are represented
* How `HEAD` works
* How staging works
* How commit history forms a graph
* How version-control operations manipulate repository state
* How a systems-oriented C++ application can be designed and tested

Mini Git is therefore both a software-engineering project and a systems-programming learning project.

---

# Technology

## Language

C++20

## Build System

CMake

## Cryptography

OpenSSL 3

The project currently uses OpenSSL's EVP interface for SHA-256 hashing.

## Development Tools

* Git
* GitHub
* CMake
* C++ standard library
* Unix / Linux concepts

## Standard Library Features

The project currently uses and plans to use facilities including:

* `std::filesystem`
* `std::string`
* `std::vector`
* File streams
* Error handling facilities
* Other C++ standard-library components as required

---

# Project Structure

The current source tree is:

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

As the project grows, additional modules will be introduced when their responsibilities become necessary.

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
    → represents file contents

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
```

Components should not unnecessarily take responsibility for unrelated operations.

---

## Standard C++

Mini Git primarily uses the C++ standard library.

External dependencies are introduced only when they provide a meaningful advantage.

OpenSSL is currently used for cryptographic hashing rather than implementing SHA-256 manually.

---

## RAII

Resources should be managed through C++ lifetime semantics wherever practical.

This includes:

* File streams
* Memory
* Locks
* Other resources introduced later

The codebase will be progressively reviewed for safer and clearer resource management.

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

The CLI should provide useful error messages instead of silently failing.

---

# Limitations

Mini Git is intentionally much smaller than Git.

It does not attempt to reproduce every Git feature.

Full compatibility with real Git is not the primary objective.

The priorities are:

```text
Understanding
     +
Correct Implementation
     +
Clean Architecture
     +
Testing
     +
Documentation
```

rather than implementing the maximum possible number of commands.

The project uses simplified internal representations where appropriate for educational purposes.

---

# Roadmap

## Phase 0 — Git Concepts

Understand the fundamental concepts behind version control and Git.

## Phase 1 — Project Setup

Establish the C++20 project, CMake, Git workflow, testing structure, and documentation.

## Phase 2 — Repository Initialization

Implement:

```bash
mini-git init
```

Create the initial `.mini-git` repository structure and `HEAD`.

## Phase 3 — Hashing

Implement deterministic cryptographic hashing using SHA-256 and OpenSSL.

## Phase 4 — Object Model

Implement the foundational object types:

* Blob
* Tree
* Commit

and their serialization interfaces.

## Phase 5 — Blob Objects

Connect blobs to real file contents and establish the first complete object-identity workflow.

## Phase 6 — Object Database

Implement persistent object storage and retrieval.

## Phase 7 — Trees

Construct tree objects from repository directory structures.

## Phase 8 — Index

Implement the staging area and:

```bash
mini-git add
```

## Phase 9 — Status

Implement:

```bash
mini-git status
```

## Phase 10 — Commits

Connect the index, trees, and commit objects to implement:

```bash
mini-git commit
```

## Phase 11 — Log

Implement commit history inspection:

```bash
mini-git log
```

## Phase 12 — References and HEAD

Implement repository references and `HEAD` management.

## Phase 13 — Branches

Implement branch creation and management.

## Phase 14 — Checkout

Implement switching between repository states.

## Phase 15 — Diff

Compare repository states and working-tree changes.

## Phase 16 — Merge

Implement fast-forward merging and a simplified three-way merge.

## Phase 17 — Conflict Handling

Detect and represent merge conflicts.

## Phase 18 — Tags

Implement lightweight tags.

## Phase 19 — Repository Maintenance

Explore object reachability, unused objects, and garbage-collection concepts.

## Phase 20 — Testing

Expand unit, integration, edge-case, and repository-level tests.

## Phase 21 — Robustness

Improve validation, error handling, and failure recovery.

## Phase 22 — Performance

Benchmark important repository operations and identify performance bottlenecks.

## Phase 23 — Refactoring

Review the architecture and modern C++ practices, including:

* RAII
* Const correctness
* Ownership
* Interfaces
* Testability
* Separation of concerns
* Error handling
* Maintainability

## Phase 24 — Documentation

Document implementation details, architectural decisions, internal formats, and important design tradeoffs.

## Phase 25 — Git Comparison

Compare Mini Git with real Git feature-by-feature to identify similarities, differences, simplifications, and limitations.

## Phase 26 — Portfolio

Prepare the project for professional presentation:

* GitHub repository
* README
* Architecture documentation
* Technical documentation
* Resume description
* LinkedIn description
* Interview explanation
* Technical discussion points
* Demonstration workflow

---

# Project Status

**Current phase: Phase 4 — Object Model**

Implemented:

```text
Repository Initialization
        ↓
     SHA-256
        ↓
   Object Model
    ┌───┼───┐
    ▼   ▼   ▼
  Blob Tree Commit
```

The next major subsystem is:

```text
Real File Content
       ↓
      Blob
       ↓
   Object ID
       ↓
Persistent Object Storage
```

This will begin in **Phase 5 — Blob Objects**.

---

# License

This project is provided under the terms of the license included in the repository.
