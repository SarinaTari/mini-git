# Mini Git Architecture

## Overview

Mini Git is a Git-inspired version control system implemented from scratch in C++20.

The project is designed to expose the internal mechanisms behind local version control rather than provide complete Git compatibility.

The architecture is organized around several major subsystems:

```text
CLI
 │
 ├── Repository
 │
 ├── Index
 │
 ├── Object Database
 │
 ├── References / HEAD
 │
 ├── History
 │
 └── Diff
```

Each subsystem has a focused responsibility.

---

# High-Level Architecture

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
```

Phase 14 adds:

```text
Working Tree ─────┐
                  │
Index ────────────┼──► Snapshot ──► Diff
                  │
Commit ───────────┘
```

---

# Architectural Layers

## 1. Command Layer

The command layer handles CLI operations.

Examples:

```bash
mini-git init
mini-git add file.txt
mini-git commit -m "message"
mini-git branch feature
mini-git checkout feature
mini-git diff
```

The command layer should coordinate subsystems rather than implement their internal logic.

For example:

```text
mini-git diff
      │
      ▼
Command Layer
      │
      ▼
Diff
```

The Diff class performs the actual comparison.

---

# 2. Repository

`Repository` represents repository-level state.

Responsibilities include:

* repository location
* `.mini-git` location
* initialization
* HEAD handling
* branch references
* current branch
* commit resolution
* checkout

Conceptually:

```text
Repository
 ├── root
 ├── .mini-git
 ├── HEAD
 ├── branches
 └── checkout
```

The Repository does not own the actual contents of every object.

That responsibility belongs to `ObjectDatabase`.

---

# 3. Object Model

Mini Git uses three primary persistent object types:

```text
Blob
Tree
Commit
```

They represent:

```text
Blob
 └── file contents

Tree
 └── directory snapshot

Commit
 └── snapshot + history metadata
```

---

# Blob

A Blob represents file contents.

```text
Blob
 └── content
```

The serialized Blob is hashed.

```text
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
```

The object ID identifies the content.

---

# Tree

A Tree represents a directory.

A Tree contains entries:

```text
Tree
 ├── Blob
 ├── Blob
 └── Tree
```

Example:

```text
Tree
├── main.cpp → Blob
├── README.md → Blob
└── src → Tree
    ├── parser.cpp → Blob
    └── lexer.cpp → Blob
```

Tree serialization is deterministic.

---

# Commit

A Commit represents a repository snapshot plus history metadata.

Conceptually:

```text
Commit
 ├── tree
 ├── parent
 ├── author
 └── message
```

A commit can therefore point to a parent:

```text
Commit C
   │
   ▼
Commit B
   │
   ▼
Commit A
```

This forms the commit history graph.

---

# 4. Object Database

The Object Database provides persistent storage.

Its responsibilities are:

* storing objects
* reading objects
* determining object paths
* retrieving serialized contents

The relationship is:

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
   │
   ▼
Object Database
```

Objects are treated as immutable after storage.

---

# Content-Addressable Storage

The object ID is derived from object content.

Therefore:

```text
same object content
       ↓
same serialization
       ↓
same SHA-256
       ↓
same object ID
```

This means object identity is content-based.

---

# 5. Index

The Index represents staged state.

Conceptually:

```text
path → Blob ID
```

Example:

```text
main.cpp → abc123...
README.md → 98de...
```

The Index sits between the Working Tree and committed history.

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
```

---

# 6. TreeBuilder

`TreeBuilder` converts staged Index entries into Tree objects.

Conceptually:

```text
Index
 │
 ├── src/main.cpp
 ├── src/parser.cpp
 └── README.md
 │
 ▼
TreeBuilder
 │
 ▼
Root Tree
 ├── README.md
 └── src
     ├── main.cpp
     └── parser.cpp
```

Nested directories therefore become nested Tree objects.

---

# 7. References

References map names to commit IDs.

Branch references are stored under:

```text
.mini-git/refs/heads/
```

For example:

```text
.mini-git/
└── refs/
    └── heads/
        ├── main
        └── feature
```

The contents are commit IDs.

---

# 8. HEAD

`HEAD` identifies the current repository position.

Symbolic HEAD:

```text
ref: refs/heads/main
```

Resolution:

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

Detached HEAD:

```text
HEAD
 │
 ▼
Commit ID
```

---

# 9. Branches

Branches are references.

They do not contain copies of commits.

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

The branch names simply identify different commit tips.

---

# 10. Checkout

Checkout changes the current branch and restores its snapshot.

The process is:

```text
checkout feature
       │
       ▼
resolve refs/heads/feature
       │
       ▼
commit ID
       │
       ▼
Commit
       │
       ▼
Tree
       │
       ▼
recursive Tree traversal
       │
       ▼
Working Tree
       │
       ▼
HEAD → refs/heads/feature
```

The current implementation performs basic checkout.

Future work includes:

* complete Index synchronization
* dirty-tree detection
* deletion of files absent from target snapshots
* stronger overwrite protection

---

# 11. Diff

Phase 14 introduces the Diff subsystem.

Its purpose is to compare repository states.

The Diff subsystem does not modify repository state.

It should not:

* create commits
* update branches
* modify HEAD
* modify the Index
* create repository objects

It is a read-only analysis subsystem.

---

# Diff Architecture

```text
                       Diff
                        │
        ┌───────────────┼───────────────┐
        │               │               │
        ▼               ▼               ▼
 Working Tree         Index           Commit
        │               │               │
        ▼               ▼               ▼
    Snapshot         Snapshot        Snapshot
        │               │               │
        └───────────────┼───────────────┘
                        ▼
                 Compare Snapshots
                        │
                        ▼
                   LCS Algorithm
                        │
                        ▼
                  Unified Diff
```

---

# Snapshot Abstraction

Different states are normalized into:

```text
path → content
```

For example:

```text
main.cpp → "#include <iostream>\n"
README.md → "# Mini Git\n"
```

This provides a common representation.

---

# Working Tree Snapshot

The Working Tree snapshot is constructed by traversing the repository filesystem.

The `.mini-git` directory is excluded.

Conceptually:

```text
Repository Root
 │
 ├── main.cpp
 ├── README.md
 ├── src/
 │   └── parser.cpp
 │
 └── .mini-git/
```

becomes:

```text
main.cpp → content
README.md → content
src/parser.cpp → content
```

---

# Index Snapshot

The Index contains Blob IDs rather than raw file contents.

Therefore the Diff subsystem:

```text
Index
 │
 ▼
Blob ID
 │
 ▼
Object Database
 │
 ▼
Blob object
 │
 ▼
Blob content
```

and constructs:

```text
path → content
```

---

# Commit Snapshot

A commit snapshot is reconstructed through:

```text
Commit
 │
 ▼
Tree
 │
 ├── Blob
 ├── Blob
 └── Tree
      │
      └── Blob
```

The Tree is recursively traversed.

Every Blob is read from the Object Database.

The final representation is:

```text
path → content
```

---

# Diff Modes

## Working Tree vs Index

```bash
mini-git diff
```

Architecture:

```text
Working Tree
      │
      ▼
Snapshot A

Index
      │
      ▼
Snapshot B

Snapshot A ↔ Snapshot B
```

This represents unstaged changes.

---

# Index vs HEAD

```bash
mini-git diff --cached
```

Architecture:

```text
HEAD
 │
 ▼
Commit
 │
 ▼
Snapshot A

Index
 │
 ▼
Snapshot B

Snapshot A ↔ Snapshot B
```

This represents staged changes.

---

# Commit vs Working Tree

```bash
mini-git diff <commit>
```

Architecture:

```text
Commit
 │
 ▼
Snapshot A

Working Tree
 │
 ▼
Snapshot B

Snapshot A ↔ Snapshot B
```

---

# Commit vs Commit

```bash
mini-git diff <commit1> <commit2>
```

Architecture:

```text
Commit 1
   │
   ▼
Snapshot A

Commit 2
   │
   ▼
Snapshot B

Snapshot A ↔ Snapshot B
```

---

# Change Classification

For every path:

```text
path ∈ old
path ∈ new
```

there are four cases.

## Unchanged

```text
exists_old = true
exists_new = true
content equal
```

Result:

```text
no output
```

---

## Modified

```text
exists_old = true
exists_new = true
content different
```

Result:

```text
diff
```

---

## Added

```text
exists_old = false
exists_new = true
```

Result:

```text
all new lines are additions
```

---

## Deleted

```text
exists_old = true
exists_new = false
```

Result:

```text
all old lines are deletions
```

---

# LCS Diff Algorithm

Phase 14 uses the Longest Common Subsequence concept.

Suppose:

```text
Old:

A
B
C
D
```

and:

```text
New:

A
X
C
D
```

The common subsequence is:

```text
A
C
D
```

Therefore the change is:

```text
-B
+X
```

---

# LCS Table

The implementation creates a table:

```text
LCS[i][j]
```

representing the longest common subsequence between suffixes of the two line sequences.

The table is used to reconstruct the change sequence.

---

# Unified Diff

The final result uses:

```text
--- a/file
+++ b/file
```

followed by changed lines.

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

---

# Data Flow

The full Phase 14 data flow is:

```text
                 ┌─────────────────┐
                 │  Working Tree   │
                 └────────┬────────┘
                          │
                          ▼
                     File Read
                          │
                          ▼
                      Snapshot
                          │
                          │
                 ┌────────┴────────┐
                 │                 │
                 ▼                 ▼
              Compare           Index
                 ▲                 │
                 │                 ▼
                 │             Blob IDs
                 │                 │
                 │                 ▼
                 │          Object Database
                 │
                 │
                 │             Commit
                 │                │
                 │                ▼
                 │               Tree
                 │                │
                 │                ▼
                 └──────────── Snapshot
                                  │
                                  ▼
                             LCS Diff
                                  │
                                  ▼
                           Unified Output
```

---

# Relationship Between Subsystems

The architecture can be viewed as:

```text
                 Repository
                      │
          ┌───────────┼───────────┐
          │           │           │
          ▼           ▼           ▼
         HEAD      References    Index
          │                       │
          ▼                       ▼
       Commit                 Blob IDs
          │                       │
          ▼                       ▼
        Tree                Object Database
          │
          ▼
        Blobs
```

Diff observes these components:

```text
Working Tree ───────┐
                    │
Index ──────────────┼──► Diff
                    │
Commit ─────────────┘
```

---

# Class Responsibilities

## `Repository`

Responsible for:

* repository path
* initialization
* HEAD
* branches
* checkout
* current commit resolution

---

## `Reference`

Responsible for:

* reference representation
* reference paths
* reference values

---

## `Object`

Base abstraction for persistent objects.

---

## `Blob`

Responsible for:

* file content
* Blob serialization
* Blob deserialization

---

## `Tree`

Responsible for:

* Tree entries
* Tree serialization
* Tree deserialization

---

## `Commit`

Responsible for:

* Tree reference
* parent reference
* author
* message
* commit serialization
* commit deserialization

---

## `ObjectDatabase`

Responsible for:

* storing serialized objects
* retrieving objects
* object paths
* content-addressable persistence

---

## `Index`

Responsible for:

* staged paths
* Blob IDs
* persistence of staging state

---

## `TreeBuilder`

Responsible for:

* converting Index state into Tree objects
* recursively creating directory Trees

---

## `Diff`

Responsible for:

* constructing state snapshots
* comparing snapshots
* detecting changes
* generating unified diff output

---

# Repository Layout

A Mini Git repository has approximately this structure:

```text
project/
│
├── source files
│
└── .mini-git/
    ├── HEAD
    │
    ├── objects/
    │
    ├── refs/
    │   └── heads/
    │       ├── main
    │       └── feature
    │
    └── index
```

The Working Tree exists outside `.mini-git`.

The `.mini-git` directory stores repository metadata and persistent objects.

---

# Important Invariants

## Object Identity

Object ID must be determined by serialized content.

```text
same serialized content
        ↓
same hash
        ↓
same object ID
```

---

## Tree Determinism

Equivalent Trees must serialize deterministically.

---

## Commit Immutability

Once stored, a Commit should not be modified.

A changed snapshot creates a new Commit.

---

## Branch Identity

A branch is a reference to a Commit.

It is not a separate history database.

---

## HEAD Resolution

Symbolic HEAD must resolve through its referenced branch.

---

## Diff Read-Only Behavior

Diff must not modify repository state.

---

# Future Architecture

After Diff, the next major subsystem is Merge.

The architecture will eventually become:

```text
                 Commit A
                /        \
               /          \
          Commit B       Commit C
               \          /
                \        /
                 Merge
                   │
                   ▼
                Commit D
```

Merge will reuse concepts already developed:

* commit traversal
* Tree reconstruction
* snapshots
* diff
* ancestry

This is why the snapshot abstraction introduced in Phase 14 is important.

---

# Future Merge Pipeline

Conceptually:

```text
Current Branch
      │
      ▼
Current Commit

Target Branch
      │
      ▼
Target Commit

      │
      ▼

Find Common Ancestor

      │
      ▼

Three Snapshots

      │
      ├──────────────┐
      ▼              ▼
   Current          Target
      │              │
      └──────┬───────┘
             ▼
        Three-Way Merge
             │
       ┌─────┴─────┐
       ▼           ▼
    Clean        Conflict
    Merge         State
```

---

# Architectural Principles

Mini Git follows these principles:

### Separation of Concerns

Storage, repository state, objects, staging, and diffing remain separate.

### Immutable Objects

Persistent objects are treated as immutable.

### Deterministic Serialization

Object identity depends on deterministic serialized data.

### Explicit State

Working Tree, Index, HEAD, branches, and commits are represented explicitly.

### Reusable Abstractions

Snapshots introduced for Diff can later be reused by Merge and educational tools.

### Testability

Each subsystem should be testable independently.

### Incremental Development

New phases build on previous abstractions rather than replacing them unnecessarily.

---

# Current Architecture

At Phase 14:

```text
                           CLI
                            │
                            ▼
                       Repository
                            │
          ┌─────────────────┼──────────────────┐
          │                 │                  │
          ▼                 ▼                  ▼
        HEAD             Branches             Index
          │                                    │
          ▼                                    ▼
       Commit                              Blob IDs
          │                                    │
          ▼                                    │
         Tree ◄──────── Object Database ───────┘
          │
          ▼
        Blobs


                    Diff
                     │
       ┌─────────────┼─────────────┐
       │             │             │
       ▼             ▼             ▼
 Working Tree      Index        Commit
       │             │             │
       └─────────────┼─────────────┘
                     ▼
                  Snapshot
                     │
                     ▼
                 LCS Diff
                     │
                     ▼
                Unified Output
```

This is the architecture that Phase 14 establishes before moving into Merge and conflict handling.
