# 🧠 Mini Git

A **Git-inspired educational version-control system built from scratch in C++20**, designed to explore how version control works internally — from **SHA-256 object storage and staging** to **commits, branches, merges, reachability, integrity checking, and repository analysis**.

Mini Git is not a Git replacement and does not aim for compatibility with the real Git implementation. Instead, it provides a self-contained implementation of core version-control concepts with a strong focus on **systems programming, data structures, filesystem operations, algorithms, and software architecture**.

---

## ✨ Why Mini Git?

Most developers use Git every day without needing to understand what happens underneath commands such as:

```bash
git add
git commit
git branch
git checkout
git merge
```

Mini Git was built to answer those questions by implementing the underlying ideas directly.

Instead of treating version control as a collection of commands, Mini Git models it as a system of:

```text
Working Tree
     │
     ▼
   Index
     │
     ▼
   Objects
     │
     ▼
 References
     │
     ▼
Repository History
```

The project therefore provides a practical way to study:

* 🧩 Object-oriented design
* 📁 Filesystem programming
* 🔐 SHA-256 hashing
* 💾 Content-addressed storage
* 🌳 Tree structures
* 📝 Staging and indexing
* 🔗 References and `HEAD`
* 📜 Commit graphs
* 🌿 Branching
* 🔀 Three-way merging
* 🧭 Ancestry and merge-base discovery
* 🔍 Repository integrity
* ♻️ Object reachability
* 📊 Repository analysis
* 🧪 Automated testing
* 🏗️ C++20 architecture

---

# 🚀 Features

## 📦 Repository & Object Storage

* Initialize Mini Git repositories
* Filesystem-based repository storage
* SHA-256 object identification
* Content-addressed object database
* Persistent object storage
* Object existence and retrieval
* Object enumeration
* Blob, Tree, and Commit objects
* Object type detection
* Object inspection

---

## 📝 Staging & Commits

* File staging through an index
* Working-tree status analysis
* Tree construction from staged state
* Commit creation
* Parent commit relationships
* Commit history traversal
* Commit inspection

The commit pipeline is conceptually:

```text
📁 Working Tree
       │
       │ add
       ▼
   📋 Index
       │
       │ TreeBuilder
       ▼
    🌳 Tree
       │
       ▼
   📝 Commit
       │
       ▼
 💾 Object Database
       │
       ▼
🌿 Branch Reference
```

---

## 🌿 Branches & References

* Branch creation
* Branch listing
* Branch references
* `HEAD` management
* Branch checkout
* Commit restoration
* Detached-HEAD state detection
* Tag references
* Tag management

Branches are represented as references rather than copies of repository history:

```text
.mini-git/refs/heads/main
            │
            ▼
        commit ID
```

---

## 🔀 Merge System

Mini Git implements a three-way merge model based on:

```text
          Base
         /    \
        /      \
   Current    Target
        \      /
         \    /
         Merge
```

The merge system includes:

* Merge-base discovery
* Ancestor analysis
* Fast-forward handling
* Already-up-to-date handling
* Three-way snapshot comparison
* Conflict detection
* Conflict markers
* Persistent merge state
* `merge --continue`
* `merge --abort`
* Merge commits with multiple parents

Example conflict representation:

```text
<<<<<<< ours
current version
=======
target version
>>>>>>> feature
```

---

## 📊 Diff System

Mini Git can compare multiple repository states:

```text
Working Tree ↔ Index
Index         ↔ HEAD
Commit        ↔ Working Tree
Commit        ↔ Commit
```

Supported diff modes include:

```bash
mini-git diff
mini-git diff --cached
mini-git diff <commit>
mini-git diff <commit> <commit>
```

Internally, repository states can be represented as:

```text
path → content
```

which provides a common basis for comparison.

---

## 🔍 Repository Intelligence

Mini Git goes beyond basic version-control operations with several educational and diagnostic tools.

### 🔎 Inspector

Inspect individual repository objects.

### 💡 Explainer

Explain supported commands and concepts.

### 🌐 Graph

Visualize commit relationships.

### 📊 Stats

Display repository statistics.

### 🧠 Analyzer

Perform repository-level analysis.

### 💥 Impact

Analyze information associated with a commit.

### 🩺 Doctor

Inspect repository health.

### 💾 Storage

Analyze repository storage.

### ⚡ Benchmark

Run repository-related benchmark functionality.

These tools reuse the same underlying repository and object model rather than maintaining a separate representation of repository state.

---

# 🛡️ Integrity & Recovery

Mini Git includes several systems for understanding repository integrity.

## 🔐 Integrity Checker

The integrity system can identify information such as:

* Corrupted objects
* Missing objects
* Invalid references
* Invalid object relationships
* Unreachable objects
* Object counts
* Valid objects

---

## 🧭 Reachability

Objects are traversed through repository relationships:

```text
References
    │
    ▼
 Commits
    │
    ├──────────► Parents
    │
    ▼
  Trees
    │
    ├──────────► Trees
    │
    └──────────► Blobs
```

Objects that cannot be reached through the relevant repository graph can be identified as unreachable.

---

## ♻️ Garbage Collection Analysis

Mini Git includes a garbage-collection analysis component that can report:

* Unreachable objects
* Estimated reclaimable storage

The current implementation is intentionally conservative and provides **analysis/preview behavior rather than automatically deleting objects**.

---

# 🏗️ Architecture

Mini Git is organized into several focused components.

```text
                    ┌──────────────────────┐
                    │     CLI / main.cpp   │
                    └──────────┬───────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
        ┌───────────┐    ┌───────────┐    ┌───────────┐
        │ Repository│    │   Status  │    │   Diff    │
        └─────┬─────┘    └─────┬─────┘    └───────────┘
              │                │
              ▼                ▼
        ┌───────────┐    ┌───────────┐
        │ Reference │    │   Index   │
        └───────────┘    └─────┬─────┘
                               │
                               ▼
                       ┌──────────────┐
                       │ TreeBuilder  │
                       └──────┬───────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │ ObjectDatabase   │
                    └────────┬─────────┘
                             │
                  ┌──────────┼──────────┐
                  │          │          │
                  ▼          ▼          ▼
                Blob       Tree      Commit
```

Additional systems operate on this model:

```text
Repository
    │
    ├── Merge
    ├── Ancestry
    ├── Reachability
    ├── IntegrityChecker
    ├── GarbageCollector
    └── Repository Analysis
          ├── Inspector
          ├── Graph
          ├── Stats
          ├── Analyzer
          ├── Impact
          ├── Doctor
          ├── StorageAnalyzer
          └── Benchmark
```

---

# 🧩 Core Object Model

Mini Git uses three primary object types:

```text
                 Object
                /  |  \
               /   |   \
            Blob  Tree  Commit
                   │      │
                   │      ├── Tree
                   │      └── Parents
                   │
                   └── Entries
```

## 📄 Blob

Stores file content.

```text
File
 │
 ▼
Blob
 │
 ▼
SHA-256
 │
 ▼
Object ID
```

---

## 🌳 Tree

Represents directory structure.

A tree entry contains:

```text
name
object ID
is_tree
```

Trees can therefore reference:

* Blobs
* Nested Trees

---

## 📝 Commit

A commit contains:

```text
tree ID
parent IDs
author
message
```

A normal history:

```text
A → B → C
```

A merge commit can have multiple parents:

```text
      B
     / \
    A   D
     \ /
      C
```

---

# 💾 Repository Storage

Mini Git stores repository metadata inside:

```text
.mini-git/
```

A typical repository looks like:

```text
project/
├── source files
├── README.md
└── .mini-git/
    ├── HEAD
    ├── index
    ├── objects/
    └── refs/
        ├── heads/
        └── tags/
```

### `HEAD`

Represents the current repository state.

### `index`

Contains staged path/object information.

### `objects/`

Contains serialized repository objects.

### `refs/heads/`

Contains branch references.

### `refs/tags/`

Contains tag references.

---

# 🔐 Content-Addressed Storage

Objects are identified using SHA-256-derived IDs.

```text
Serialized Object
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

This object model makes it possible for commits and trees to reference other objects by ID.

OpenSSL is used for SHA-256 hashing.

---

# 📁 Project Structure

```text
mini-git/
│
├── CMakeLists.txt
├── README.md
│
├── include/
│   ├── Ancestry.hpp
│   ├── Analyzer.hpp
│   ├── Benchmark.hpp
│   ├── Blob.hpp
│   ├── Commit.hpp
│   ├── Diff.hpp
│   ├── Doctor.hpp
│   ├── Explainer.hpp
│   ├── FileReader.hpp
│   ├── GarbageCollector.hpp
│   ├── Graph.hpp
│   ├── Hash.hpp
│   ├── Impact.hpp
│   ├── Index.hpp
│   ├── Inspector.hpp
│   ├── IntegrityChecker.hpp
│   ├── Merge.hpp
│   ├── Object.hpp
│   ├── ObjectDatabase.hpp
│   ├── ObjectType.hpp
│   ├── Reachability.hpp
│   ├── Reference.hpp
│   ├── Repository.hpp
│   ├── Stats.hpp
│   ├── Status.hpp
│   ├── StorageAnalyzer.hpp
│   ├── Tree.hpp
│   └── TreeBuilder.hpp
│
├── src/
│   ├── main.cpp
│   ├── Repository.cpp
│   ├── Reference.cpp
│   ├── Hash.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   ├── Diff.cpp
│   ├── Merge.cpp
│   ├── FileReader.cpp
│   ├── ObjectDatabase.cpp
│   ├── TreeBuilder.cpp
│   ├── Index.cpp
│   ├── Status.cpp
│   ├── Inspector.cpp
│   ├── Explainer.cpp
│   ├── Graph.cpp
│   ├── Stats.cpp
│   ├── Analyzer.cpp
│   ├── Impact.cpp
│   ├── Doctor.cpp
│   ├── StorageAnalyzer.cpp
│   ├── Benchmark.cpp
│   ├── ObjectType.cpp
│   ├── Reachability.cpp
│   ├── IntegrityChecker.cpp
│   ├── Ancestry.cpp
│   └── GarbageCollector.cpp
│
├── tests/
│   ├── AnalyzerTests.cpp
│   ├── AncestryTests.cpp
│   ├── BenchmarkTests.cpp
│   ├── BlobTests.cpp
│   ├── BranchTests.cpp
│   ├── CheckoutTests.cpp
│   ├── CommitTests.cpp
│   ├── DiffTests.cpp
│   ├── DoctorTests.cpp
│   ├── ExplainerTests.cpp
│   ├── FileReaderTests.cpp
│   ├── FsckTests.cpp
│   ├── GarbageCollectionTests.cpp
│   ├── GraphTests.cpp
│   ├── HashTests.cpp
│   ├── ImpactTests.cpp
│   ├── IndexTests.cpp
│   ├── InspectorTests.cpp
│   ├── IntegrityTests.cpp
│   ├── LogTests.cpp
│   ├── MergeStateTests.cpp
│   ├── MergeTests.cpp
│   ├── ObjectTests.cpp
│   ├── ObjectTypeTests.cpp
│   ├── ReachabilityTests.cpp
│   ├── ReferenceTests.cpp
│   ├── RepositoryTests.cpp
│   ├── StatsTests.cpp
│   ├── StatusTests.cpp
│   ├── StorageAnalyzerTests.cpp
│   ├── TagTests.cpp
│   └── TreeBuilderTests.cpp
│
└── docs/
    ├── ARCHITECTURE.md
    ├── DESIGN.md
    ├── COMMANDS.md
    ├── OBJECT_MODEL.md
    ├── STORAGE.md
    ├── BRANCHING_AND_MERGING.md
    ├── INTEGRITY_AND_RECOVERY.md
    ├── TESTING.md
    ├── DEVELOPMENT.md
    └── LIMITATIONS.md
```

---

# 🛠️ Technologies

| Technology     | Purpose                                   |
| -------------- | ----------------------------------------- |
| **C++20**      | Core implementation                       |
| **CMake**      | Build system                              |
| **CTest**      | Automated testing                         |
| **OpenSSL**    | SHA-256 hashing                           |
| **C++ STL**    | Data structures and filesystem operations |
| **Git/GitHub** | Project version control and hosting       |

---

# 💻 Building

## 1. Clone the Repository

```bash
git clone https://github.com/SarinaTari/mini-git.git
cd mini-git
```

## 2. Configure

```bash
cmake -S . -B build
```

## 3. Build

```bash
cmake --build build
```

The executable will be available at:

```text
build/mini-git
```

---

# 🧪 Testing

Run the complete test suite:

```bash
ctest --test-dir build --output-on-failure
```

The project contains **32 dedicated test executables** covering the major implemented subsystems.

The test suite covers areas including:

* 🔐 hashing
* 📄 blobs
* 🌳 trees
* 📝 commits
* 💾 object storage
* 📋 index
* 📊 status
* 📜 history
* 🌿 branches
* 🔄 checkout
* 🔍 diff
* 🔀 merge
* 🏷️ tags
* 🔎 object inspection
* 💡 explanations
* 🌐 commit graphs
* 📊 statistics
* 🧠 repository analysis
* 💥 impact analysis
* 🩺 repository health
* 💾 storage analysis
* ⚡ benchmarking
* 🧭 reachability
* 🛡️ integrity checking
* 🔗 ancestry
* ♻️ garbage-collection analysis

---

# 🖥️ Command Overview

## 📦 Repository

```bash
mini-git init
mini-git --version
```

## 🔐 Hashing

```bash
mini-git hash-file <file>
mini-git hash-object <file>
```

## 📋 Staging & Status

```bash
mini-git add <file>
mini-git add .
mini-git status
```

## 📝 History

```bash
mini-git commit -m "message"
mini-git log
```

## 🌿 Branches

```bash
mini-git branch
mini-git branch <name>
mini-git checkout <branch>
```

## 🔍 Diff

```bash
mini-git diff
mini-git diff --cached
mini-git diff <commit>
mini-git diff <commit> <commit>
```

## 🔀 Merge

```bash
mini-git merge <branch>
mini-git merge --continue
mini-git merge --abort
```

## 🏷️ Tags

```bash
mini-git tag
mini-git tag <name>
```

## 🔎 Inspection & Education

```bash
mini-git inspect <object-id>
mini-git explain <command>
```

## 🧠 Repository Intelligence

```bash
mini-git graph
mini-git stats
mini-git analyze
mini-git impact <commit>
```

## 🩺 Repository Health

```bash
mini-git doctor
mini-git fsck
```

## 💾 Storage & Performance

```bash
mini-git storage
mini-git benchmark
```

For the detailed command reference, see:

📖 `docs/COMMANDS.md`

---

# 🎯 Example Workflow

## Initialize

```bash
./build/mini-git init
```

## Stage files

```bash
./build/mini-git add README.md
./build/mini-git add src/main.cpp
```

## Check status

```bash
./build/mini-git status
```

## Create a commit

```bash
./build/mini-git commit -m "Initial commit"
```

## Inspect history

```bash
./build/mini-git log
```

## Create a feature branch

```bash
./build/mini-git branch feature
./build/mini-git checkout feature
```

## Work and commit

```bash
./build/mini-git add .
./build/mini-git commit -m "Implement feature"
```

## Return to main

```bash
./build/mini-git checkout main
```

## Merge

```bash
./build/mini-git merge feature
```

---

# 🧠 What This Project Demonstrates

Mini Git was designed to be more than a CLI application.

It demonstrates practical understanding of:

### 💻 Systems Programming

* filesystem operations
* persistent state
* object storage
* command-line interfaces

### 🔐 Algorithms & Data Structures

* SHA-256 hashing
* graph traversal
* ancestry analysis
* reachability
* tree structures
* snapshot comparison
* three-way merging

### 🏗️ Software Architecture

* modular C++ design
* separation of concerns
* reusable abstractions
* repository modeling
* object-oriented design
* RAII
* const correctness
* error handling

### 🧪 Software Engineering

* CMake
* CTest
* unit testing
* repository-level testing
* regression testing
* documentation
* Git-based development workflow

---

# 📚 Documentation

The project includes detailed technical documentation.

| Document                                                      | Description                                  |
| ------------------------------------------------------------- | -------------------------------------------- |
| [`ARCHITECTURE.md`](docs/ARCHITECTURE.md)                     | Overall system architecture                  |
| [`DESIGN.md`](docs/DESIGN.md)                                 | Design philosophy and engineering decisions  |
| [`COMMANDS.md`](docs/COMMANDS.md)                             | CLI command reference                        |
| [`OBJECT_MODEL.md`](docs/OBJECT_MODEL.md)                     | Blob, Tree, Commit, and object relationships |
| [`STORAGE.md`](docs/STORAGE.md)                               | Repository and object storage                |
| [`BRANCHING_AND_MERGING.md`](docs/BRANCHING_AND_MERGING.md)   | Branch, ancestry, and merge internals        |
| [`INTEGRITY_AND_RECOVERY.md`](docs/INTEGRITY_AND_RECOVERY.md) | Integrity, reachability, and recovery        |
| [`TESTING.md`](docs/TESTING.md)                               | Testing architecture and workflow            |
| [`DEVELOPMENT.md`](docs/DEVELOPMENT.md)                       | Development and contribution workflow        |
| [`LIMITATIONS.md`](docs/LIMITATIONS.md)                       | Explicit project scope and limitations       |

---

# ⚠️ Limitations

Mini Git is intentionally smaller than production Git.

It currently does **not** aim to implement the complete Git ecosystem, including:

* Git repository compatibility
* Git's native `.git` format
* Git's native object serialization
* remote repositories
* `push`
* `pull`
* `fetch`
* `clone`
* remote tracking
* Git hosting protocols
* production-scale object packing
* production-grade garbage collection
* every Git merge edge case
* every Git command and option

For the complete scope, see:

📖 `docs/LIMITATIONS.md`

---

# 🔬 Educational Scope

The purpose of Mini Git is to make the internals of version control understandable.

A simplified view of the system is:

```text
                 ┌──────────────┐
                 │ Working Tree │
                 └──────┬───────┘
                        │
                       add
                        │
                        ▼
                 ┌──────────────┐
                 │    Index     │
                 └──────┬───────┘
                        │
                    TreeBuilder
                        │
                        ▼
                 ┌──────────────┐
                 │     Tree     │
                 └──────┬───────┘
                        │
                        ▼
                 ┌──────────────┐
                 │    Commit    │
                 └──────┬───────┘
                        │
                        ▼
                 ┌──────────────┐
                 │    Object    │
                 │   Database   │
                 └──────┬───────┘
                        │
                        ▼
                 ┌──────────────┐
                 │   Branch /   │
                 │     HEAD     │
                 └──────────────┘
```

From this foundation, the project builds:

```text
        ┌───────────────┐
        │   Branching   │
        └───────┬───────┘
                │
        ┌───────▼───────┐
        │    Merging    │
        └───────┬───────┘
                │
        ┌───────▼───────┐
        │   Ancestry    │
        └───────┬───────┘
                │
        ┌───────▼───────┐
        │ Reachability  │
        └───────┬───────┘
                │
        ┌───────▼───────┐
        │   Integrity   │
        └───────┬───────┘
                │
        ┌───────▼───────┐
        │  Repository   │
        │ Intelligence  │
        └───────────────┘
```

---

# 🏆 Project Goals

The main goals of Mini Git are:

* 🧠 Understand how version-control systems work internally
* 💻 Practice advanced C++ programming
* 📁 Work directly with filesystem state
* 🔐 Implement content-addressed storage
* 🌳 Build object graphs
* 🌿 Implement branches and references
* 🔀 Understand three-way merging
* 🧭 Implement ancestry and reachability algorithms
* 🛡️ Analyze repository integrity
* 🧪 Build a serious automated test suite
* 🏗️ Practice modular software architecture
* 📚 Produce documentation that explains the implementation

---

# 📈 Project Evolution

Mini Git was developed incrementally through multiple phases.

The project evolved from basic repository initialization and object hashing into a larger system containing:

```text
Phase 1
  │
  ├── Repository initialization
  ├── SHA-256 hashing
  └── Object foundations
  │
  ▼
Object Model
  │
  ├── Blob
  ├── Tree
  └── Commit
  │
  ▼
Repository State
  │
  ├── Object database
  ├── Index
  ├── Status
  └── References
  │
  ▼
History
  │
  ├── Commits
  ├── Branches
  ├── Checkout
  └── Diff
  │
  ▼
Merging
  │
  ├── Ancestry
  ├── Merge base
  ├── Three-way merge
  ├── Conflicts
  └── Merge state
  │
  ▼
Repository Intelligence
  │
  ├── Inspect
  ├── Explain
  ├── Graph
  ├── Stats
  ├── Analyze
  ├── Impact
  ├── Doctor
  ├── Storage
  └── Benchmark
  │
  ▼
Integrity
  │
  ├── Reachability
  ├── Integrity checking
  ├── fsck
  └── Garbage-collection analysis
  │
  ▼
Final Release
```

The phased approach allowed the project to grow while keeping each major concept understandable and testable.

---

# 🤝 Development Philosophy

Mini Git follows a few core principles:

### 🧩 Separation of Concerns

Repository state, objects, storage, indexing, merging, and diagnostics are represented by separate components.

### 🔐 Explicit Validation

Invalid repository state should be reported rather than silently interpreted.

### 🧪 Testability

Important components have dedicated tests.

### 📖 Understandability

The implementation prioritizes learning and clarity over reproducing the full complexity of production Git.

### 🏗️ Incremental Design

Features were developed in phases, with testing and architectural cleanup performed throughout development.

---

# 📜 License

See [`LICENSE`](LICENSE) for the project's license.

---

# 👩🏻‍💻 Author

**Sarina Tari**

Computer Engineering student focused on:

* C/C++
* Python
* Linux
* Git/GitHub
* Systems programming
* Software architecture
* Algorithms & data structures

---

# ⭐ Final Note

Mini Git started with a simple question:

> **What actually happens when you run `git add` or `git commit`?**

The project became an exploration of the answer.

From files and hashes:

```text
File
 │
 ▼
Blob
 │
 ▼
SHA-256
```

to repository history:

```text
Branch
  │
  ▼
Commit
  │
  ├── Tree
  │    ├── Blob
  │    └── Tree
  │
  └── Parent
```

and eventually to:

```text
🌿 Branching
🔀 Merging
🧭 Ancestry
🔍 Reachability
🛡️ Integrity
📊 Repository Intelligence
🧪 Automated Testing
```

Mini Git is ultimately a practical exercise in understanding how a version-control system can be built from fundamental systems-programming concepts.

**Built from scratch. Built to understand. 🧠💻**
