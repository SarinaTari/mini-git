# Mini Git

A Git-inspired version control system built from scratch in C++.

Mini Git is an educational systems-programming project designed to explore how modern version control systems work internally.

The project focuses on understanding and implementing core concepts behind Git rather than simply using Git's commands or libraries.

> **Note:** Mini Git is an educational project inspired by Git. It is not intended to be a replacement for Git and does not aim for full Git compatibility.

---

## Goals

The main goals of Mini Git are to understand and implement:

* Content-addressable storage
* Cryptographic hashing
* Object databases
* Blobs
* Trees
* Commits
* Staging areas
* References
* `HEAD`
* Branches
* Commit history
* Diffs
* Merging
* Repository integrity
* Basic version-control workflows

The project also emphasizes professional C++ development practices such as:

* C++20
* CMake
* Modular architecture
* Separation of concerns
* Error handling
* Automated testing
* Documentation
* Git-based development workflow

---

## Current Status

Mini Git is being developed incrementally in phases.

### Implemented

### Implemented

- C++20 project structure
- CMake build system
- Command-line executable
- `mini-git --version`
- `mini-git init`
- Repository initialization
- `.mini-git/` metadata directory
- `objects/` directory
- `refs/heads/` directory
- Initial `HEAD` reference
- `HEAD` pointing to `main`
- Basic repository abstraction
- SHA-256 hashing
- OpenSSL-based cryptographic hashing
- Automated hash tests
- CTest integration

### Planned

* Repository discovery
* Cryptographic hashing
* Object database
* Blob objects
* Tree objects
* Index / staging area
* `mini-git add`
* `mini-git status`
* Commits
* `mini-git commit`
* `mini-git log`
* Branches
* `HEAD` management
* Checkout
* Diff
* Merge
* Conflict handling
* Tags
* Repository integrity checking
* Garbage collection concepts
* Performance testing
* Extensive automated tests

### Educational Features

Mini Git will also contain features designed specifically to make its internal behavior easier to understand:

* `mini-git inspect`
* `mini-git graph`
* `mini-git explain`
* `mini-git stats`
* `mini-git fsck`

These commands are intended to expose the internal state and behavior of the system rather than simply imitate Git's user interface.

---

## Architecture

The project is being developed as a layered version-control system.

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

The architecture will evolve as new features are implemented.

---

## Repository Structure

A Mini Git repository contains a hidden `.mini-git` directory.

The initial repository structure is:

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

## Repository Initialization

A repository can be initialized with:

```bash
mini-git init
```

This creates a `.mini-git` directory in the current working directory.

The initial structure is:

```text
.mini-git/
├── objects/
├── refs/
│   └── heads/
└── HEAD
```

The initial `HEAD` file contains:

```text
ref: refs/heads/main
```

This means that `HEAD` symbolically refers to the `main` branch.

At this stage, the `main` branch does not yet contain a commit.

The `objects/` directory is also empty because object storage has not yet been implemented.

---

## Build

Mini Git uses CMake as its build system.

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

The executable will be created inside:

```text
build/mini-git
```

---

## Run

Run Mini Git with:

```bash
./build/mini-git
```

Expected output:

```text
Mini Git
```

---

## Check Version

```bash
./build/mini-git --version
```

Expected output:

```text
mini-git version 0.1.0
```

---

## Initialize a Repository

Create a test directory:

```bash
mkdir mini-git-test
cd mini-git-test
```

Run:

```bash
/path/to/mini-git/build/mini-git init
```

Mini Git should create:

```text
mini-git-test/
└── .mini-git/
    ├── objects/
    ├── refs/
    │   └── heads/
    └── HEAD
```

Running `init` again should report that a repository already exists.

---

## Development

Mini Git itself is version-controlled using Git.

The development workflow is:

```text
Modify code
    ↓
Build
    ↓
Run tests
    ↓
Inspect behavior
    ↓
Commit changes
    ↓
Push to GitHub
```

Git is used to manage the Mini Git source code.

Mini Git will later be used to manage separate test repositories.

---

## Testing

Automated tests will be added progressively as the implementation grows.

The project will eventually test:

* Repository initialization
* Hash generation
* Object creation
* Object storage
* Object reading
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

Testing will include both unit tests and integration tests.

---

## Design Philosophy

Mini Git is intentionally built incrementally.

Instead of immediately implementing a large number of commands, the project first establishes the internal concepts required to understand them.

For example:

```text
Working Tree
      │
      ▼
     add
      │
      ▼
    Index
      │
      ▼
   commit
      │
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

Each layer will be implemented and tested before building more complex functionality on top of it.

---

## Why Build Mini Git?

Git is commonly used as a command-line tool without requiring users to understand its internal implementation.

Building a simplified version from scratch provides an opportunity to understand:

* How files become versioned objects
* How content-addressable storage works
* How hashes identify data
* How commits reference previous commits
* How directory structures are represented
* How branches are implemented
* How `HEAD` works
* How staging works
* How history is represented as a graph
* How version-control operations manipulate repository state

This makes Mini Git both a software-engineering project and a systems-programming learning project.

---

## Technology

### Language

C++20

### Build System

CMake

### Development Tools

* Git
* GitHub
* Linux / Unix concepts
* CMake
* C++ standard library

### Planned Concepts

* `std::filesystem`
* Cryptographic hashing
* Binary file I/O
* File-system metadata
* Object serialization
* Graph structures
* Repository state management

---

## Project Structure

The source tree currently follows this structure:

```text
mini-git/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── include/
├── src/
│   ├── main.cpp
│   └── Repository.cpp
├── tests/
└── docs/
    └── architecture.md
```

As the project grows, additional modules will be introduced.

---

## Limitations

Mini Git is intentionally smaller than Git.

It will not attempt to reproduce every Git feature.

In particular, compatibility with real Git is not the primary objective.

The priority is:

```text
Understanding
     +
Correct implementation
     +
Clean architecture
     +
Testing
     +
Documentation
```

rather than implementing the maximum number of commands.

---

## Roadmap

### Phase 0 — Git Concepts

Understand the fundamental concepts behind Git.

### Phase 1 — Project Setup

Set up the C++20 project, CMake, Git, testing structure, and documentation.

### Phase 2 — Repository Initialization

Implement:

```bash
mini-git init
```

### Phase 3 — Hashing

Implement deterministic cryptographic object identifiers.

### Phase 4 — Object Model

Design:

* Blob
* Tree
* Commit

### Phase 5 — Blobs

Store file contents as objects.

### Phase 6 — Object Database

Implement object storage and retrieval.

### Phase 7 — Trees

Represent directory structures as tree objects.

### Phase 8 — Index

Implement the staging area.

### Phase 9 — Status

Implement:

```bash
mini-git status
```

### Phase 10 — Commits

Implement commit creation.

### Phase 11 — Log

Implement commit history inspection.

### Phase 12 — References and HEAD

Implement repository references and `HEAD`.

### Phase 13 — Branches

Implement branch creation and management.

### Phase 14 — Checkout

Implement switching between repository states.

### Phase 15 — Diff

Compare repository states.

### Phase 16 — Merge

Implement fast-forward and simplified three-way merging.

### Phase 17 — Conflict Handling

Detect and represent merge conflicts.

### Phase 18 — Tags

Implement lightweight tags.

### Phase 19 — Repository Maintenance

Explore object reachability and garbage collection.

### Phase 20 — Testing

Expand automated tests and edge-case coverage.

### Phase 21 — Robustness

Improve validation and error handling.

### Phase 22 — Performance

Benchmark important operations.

### Phase 23 — Refactoring

Review architecture and modern C++ practices.

### Phase 24 — Documentation

Document implementation decisions and internal mechanisms.

### Phase 25 — Git Comparison

Compare Mini Git's architecture and behavior with real Git.

### Phase 26 — Portfolio

Prepare:

* GitHub repository
* README
* Architecture documentation
* Resume description
* LinkedIn description
* Interview explanation
* Technical discussion points

---

## Status

Mini Git is currently in the early implementation stage.

The repository initialization system has been implemented, and the next major subsystem is cryptographic hashing.

The project will continue to evolve incrementally as each internal component is designed, implemented, tested, and documented.
