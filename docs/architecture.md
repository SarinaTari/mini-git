# Mini Git Architecture

## Overview

Mini Git is a Git-inspired version control system implemented from scratch in C++20.

The project is designed to expose the internal mechanisms behind local version control rather than provide complete Git compatibility.

The architecture is organized around several major subsystems:

```
CLI
 │
 ├── Repository
 ├── Index
 ├── Object Database
 ├── References / HEAD
 ├── History
 ├── Diff
 └── Merge
```

Each subsystem has a focused responsibility.

Phase 15 extends the architecture with **three-way merge and conflict detection**.

---

# High-Level Architecture

```
                     Mini Git CLI
                          │
                          ▼
                   Command Layer
                          │
    ┌─────────────────────┼─────────────────────┐
    │                     │                     │
    ▼                     ▼                     ▼
```

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
                   ┌─────────────┐
                   │    Diff     │
                   └──────┬──────┘
                          │
                          ▼
                      Snapshots
                          │
                          ▼
                     LCS Diff

                   ┌─────────────┐
                   │    Merge    │
                   └──────┬──────┘
                          │
                          ▼
                 Three-Way Analysis
                          │
              ┌───────────┴───────────┐
              │                       │
              ▼                       ▼
         Clean Merge             Conflict
              │                       │
              ▼                       ▼
         Merge Commit          Conflict State
```

---

# Phase 15 Architecture

Phase 15 introduces the Merge subsystem.

The core idea is:

```
Current Branch
      │
      ▼
Current Commit
      │
      │
      ├──────────────┐
      │              │
      ▼              ▼
   Common          Target
   Ancestor        Commit
      │              │
      └──────┬───────┘
             ▼
      Three-Way Merge
             │
    ┌────────┴────────┐
    │                 │
    ▼                 ▼
 Clean              Conflict
 Changes             Detection
    │                 │
    ▼                 ▼
```

Merge Result       Merge Failure

The merge algorithm compares:

```
Base
 │
 ├── Current
 │
 └── Target
```

The Base is the common ancestor of the current and target commits.

---

# Architectural Layers

## 1. Command Layer

The command layer handles CLI operations.

Examples:

```
mini-git init

mini-git add file.txt

mini-git commit -m "message"

mini-git branch feature

mini-git checkout feature

mini-git diff

mini-git merge feature
```

The command layer coordinates subsystems rather than implementing their internal logic.

For example:

```
mini-git merge feature
        │
        ▼
   Command Layer
        │
        ▼
      Merge
        │
  ┌─────┼─────┐
  ▼     ▼     ▼
Repo  Trees  Commits
```

The Merge subsystem performs the actual merge analysis.

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
* working-tree restoration
* index synchronization

Conceptually:

```
Repository
 ├── root
 ├── .mini-git
 ├── HEAD
 ├── branches
 ├── checkout
 └── current commit
```

The Repository does not own the actual contents of every object.

That responsibility belongs to `ObjectDatabase`.

---

# 3. Object Model

Mini Git uses three primary persistent object types:

```
Blob
Tree
Commit
```

They represent:

```
Blob
 └── file contents

Tree
 └── directory snapshot

Commit
 └── snapshot + history metadata
```

The objects form a hierarchy:

```
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

---

# Blob

A Blob represents file contents.

```
Blob
 └── content
```

The serialized Blob is hashed.

```
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

Blob objects are immutable after storage.

---

# Tree

A Tree represents a directory.

A Tree contains entries:

```
Tree
 ├── Blob
 ├── Blob
 └── Tree
```

Example:

```
Tree
├── main.cpp → Blob
├── README.md → Blob
└── src → Tree
    ├── parser.cpp → Blob
    └── lexer.cpp → Blob
```

Tree serialization is deterministic.

Trees therefore provide deterministic directory snapshots.

---

# Commit

A Commit represents a repository snapshot plus history metadata.

Conceptually:

```
Commit
 ├── tree
 ├── parent(s)
 ├── author
 └── message
```

A normal commit contains one parent:

```
Commit C
   │
   ▼
Commit B
   │
   ▼
Commit A
```

A merge commit contains multiple parents:

```
Commit B ──────┐
               │
               ▼
            Merge Commit
               ▲
               │
Commit C ──────┘
```

This allows the commit graph to represent merged histories.

---

# 4. Object Database

The Object Database provides persistent storage.

Its responsibilities are:

* storing objects
* reading objects
* determining object paths
* retrieving serialized contents
* providing persistent object lookup

The relationship is:

```
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

```
same object content
       ↓
same serialization
       ↓
same SHA-256
       ↓
same object ID
```

Object identity is therefore content-based.

This property allows the same object to be reused by multiple commits.

---

# 5. Index

The Index represents staged state.

Conceptually:

```
path → Blob ID
```

Example:

```
main.cpp  → abc123...
README.md → 98de...
```

The Index sits between the Working Tree and committed history.

```
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

The Index is also important during checkout and merge because it represents the currently staged snapshot.

---

# Index Synchronization

After checkout, the Working Tree and Index must represent the same target snapshot.

Conceptually:

```
Target Commit
      │
      ▼
     Tree
      │
   ┌──┴──┐
   ▼     ▼
Working Index
  Tree
```

This prevents stale staged state from being interpreted as user changes.

For example:

```
Branch A
   │
   ▼
Commit A
   │
   ▼
checkout
   │
   ├── Working Tree = A
   └── Index = A
```

The Index therefore remains synchronized with the checked-out commit.

---

# 6. TreeBuilder

`TreeBuilder` converts staged Index entries into Tree objects.

Conceptually:

```
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

Nested directories become nested Tree objects.

TreeBuilder can also construct Trees from indexed state rather than directly from the filesystem.

---

# 7. References

References map names to commit IDs.

Branch references are stored under:

```
.mini-git/refs/heads/
```

For example:

```
.mini-git/
└── refs/
    └── heads/
        ├── main
        └── feature
```

The contents of each reference are commit IDs.

A reference therefore acts as a movable name pointing to a commit.

---

# 8. HEAD

`HEAD` identifies the current repository position.

Symbolic HEAD:

```
ref: refs/heads/main
```

Resolution:

```
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

```
HEAD
  │
  ▼
Commit ID
  │
  ▼
Commit
```

The current implementation primarily operates with symbolic branch HEADs.

---

# 9. Branches

Branches are references.

They do not contain copies of commits.

Example:

```
                 Commit A
                /        \
               /          \
            main          feature
              │              │
              ▼              ▼
          Commit B        Commit C
```

The branch names simply identify different commit tips.

Branch creation therefore only creates a new reference pointing to the current commit.

---

# 10. Checkout

Checkout changes the current branch and restores its snapshot.

The process is:

```
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
    Index
      │
      ▼
HEAD → refs/heads/feature
```

Checkout therefore synchronizes three important pieces of repository state:

```
HEAD
Index
Working Tree
```

Future improvements include:

* complete dirty-tree detection
* stronger overwrite protection
* more detailed checkout safety
* handling all filesystem edge cases

---

# 11. Diff

The Diff subsystem compares repository states.

It is a read-only analysis subsystem.

It must not:

* create commits
* update branches
* modify HEAD
* modify the Index
* create repository objects
* modify the Working Tree

Its architecture is:

```
                       Diff
                        │
         ┌──────────────┼──────────────┐
         │              │              │
         ▼              ▼              ▼
    Working Tree       Index         Commit
         │              │              │
         ▼              ▼              ▼
     Snapshot        Snapshot      Snapshot
         │              │              │
         └──────────────┼──────────────┘
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

Different repository states are normalized into:

```
path → content
```

For example:

```
main.cpp  → "#include <iostream>\n"
README.md → "# Mini Git\n"
```

This common representation allows Diff and Merge to operate independently of how the original state was stored.

Snapshots are therefore a reusable architectural abstraction.

---

# Working Tree Snapshot

The Working Tree snapshot is constructed by traversing the repository filesystem.

The `.mini-git` directory is excluded.

Conceptually:

```
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

```
main.cpp        → content
README.md       → content
src/parser.cpp  → content
```

---

# Index Snapshot

The Index contains Blob IDs rather than raw file contents.

Therefore:

```
Index
  │
  ▼
Blob ID
  │
  ▼
Object Database
  │
  ▼
Blob Object
  │
  ▼
Blob Content
```

The final representation is:

```
path → content
```

---

# Commit Snapshot

A commit snapshot is reconstructed through:

```
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

```
path → content
```

---

# Diff Modes

## Working Tree vs Index

```
mini-git diff
```

Architecture:

```
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

```
mini-git diff --cached
```

Architecture:

```
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

```
mini-git diff <commit>
```

Architecture:

```
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

```
mini-git diff <commit1> <commit2>
```

Architecture:

```
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

```
path ∈ old
path ∈ new
```

there are four cases.

## Unchanged

```
exists_old = true
exists_new = true
content equal
```

Result:

```
no output
```

---

## Modified

```
exists_old = true
exists_new = true
content different
```

Result:

```
diff
```

---

## Added

```
exists_old = false
exists_new = true
```

Result:

```
all new lines are additions
```

---

## Deleted

```
exists_old = true
exists_new = false
```

Result:

```
all old lines are deletions
```

---

# LCS Diff Algorithm

The Diff subsystem uses the Longest Common Subsequence concept.

Suppose:

```
Old:

A
B
C
D
```

and:

```
New:

A
X
C
D
```

The common subsequence is:

```
A
C
D
```

Therefore the change is:

```
-B
+X
```

---

# LCS Table

The implementation creates a table:

```
LCS[i][j]
```

representing the longest common subsequence between portions of the two line sequences.

The table is then used to reconstruct the change sequence.

---

# Unified Diff

The final result uses unified-diff style output:

```
--- a/file
+++ b/file
```

followed by changed lines.

Example:

```
diff -- mini-git hello.txt
--- a/hello.txt
+++ b/hello.txt
@@ -1,2 +1,2 @@
 hello
-world
+Mini Git
```

---

# 12. Merge

The Merge subsystem combines two branches or repository histories.

Its primary responsibility is to determine how the current branch and target branch differ from their common ancestor.

Conceptually:

```
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
Three-Way Merge
      │
  ┌───┴────┐
  ▼        ▼
Clean   Conflict
  │        │
  ▼        ▼
```

Result   Error State

Merge uses existing repository abstractions rather than duplicating object-storage logic.

---

# Three-Way Merge

A three-way merge compares three snapshots:

```
Base
 │
 ├── Current
 │
 └── Target
```

Where:

* Base is the common ancestor.
* Current is the checked-out branch.
* Target is the branch being merged.

For every path, the merge determines how each side changed relative to Base.

---

# Why Three Snapshots Are Required

Comparing only Current and Target cannot determine whether a change was independently introduced.

For example:

```
Base:
hello

Current:
hello world

Target:
hello
```

Current changed while Target did not.

This is not a conflict.

The correct result is:

```
hello world
```

Three-way comparison allows the algorithm to identify this safely.

---

# Three-Way Merge Cases

For each path, compare:

```
Base
Current
Target
```

The major cases are:

## Both Unchanged

```
Base = Current
Base = Target
```

Result:

```
Base
```

---

## Only Current Changed

```
Base = Target
Current != Base
```

Result:

```
Current
```

---

## Only Target Changed

```
Base = Current
Target != Base
```

Result:

```
Target
```

---

## Both Changed Identically

```
Current = Target
Current != Base
```

Result:

```
Current
```

No conflict exists because both sides produced the same content.

---

## Both Changed Differently

```
Current != Base
Target != Base
Current != Target
```

Result:

```
Conflict
```

The merge cannot automatically choose a version.

---

# File-Level Conflict Detection

At the current architectural level, conflicts are detected by comparing snapshot contents for each path.

Conceptually:

```
Base
  │
  ├──────────────┐
  ▼              ▼
Current        Target
  │              │
  └──────┬───────┘
         ▼
   Compare Changes
         │
   ┌─────┴─────┐
   ▼           ▼
 Same        Different
   │           │
   ▼           ▼
 Clean      Conflict
```

A conflict is reported when both branches modify the same path differently relative to the common ancestor.

---

# Common Ancestor

The common ancestor is a commit reachable from both the current and target commits.

Conceptually:

```
                 Base
                /    \
               /      \
          Current     Target
```

The Merge subsystem must identify the appropriate common ancestor before performing the three-way comparison.

This ancestor provides the baseline against which both sides are compared.

---

# Commit Graph Traversal

Merge therefore depends on commit history traversal.

Example:

```
A
```

/ 
B   C
|   |
D   E

If the current branch points to D and the target branch points to E:

```
Current = D
Target  = E
Base    = A
```

The merge compares:

```
Snapshot(A)
Snapshot(D)
Snapshot(E)
```

---

# Fast-Forward Merge

A fast-forward merge occurs when the current commit is an ancestor of the target commit.

Example:

```
A → B → C
```

Current:

```
A
```

Target:

```
C
```

Because A is already part of C's history, no merge commit is required.

The branch reference can simply move:

```
main
  │
  ▼
  A
```

becomes:

```
main
  │
  ▼
  C
```

The Working Tree and Index are then synchronized with the target snapshot.

---

# Already Up-to-Date

If the target commit is already an ancestor of the current commit:

```
A → B → C
     ↑
   Target

          ↑
        Current
```

then there is nothing to merge.

The current branch remains unchanged.

---

# Non-Conflicting Merge

Suppose:

```
Base
/  \
```

A    B

Current changes `file1.txt`.

Target changes `file2.txt`.

The changes affect different paths.

Therefore:

```
Base
  │
  ├── Current change
  │
  └── Target change
```

can be combined into a single resulting snapshot.

The merge can then create a merge commit.

---

# Conflicting Merge

Suppose:

```
Base
  │
  ├── Current changes hello.txt
  │
  └── Target changes hello.txt differently
```

The result is:

```
Conflict: hello.txt
```

The merge must not silently choose one version.

At the current implementation level, the conflict is reported as a merge failure.

The current branch should remain unchanged when the merge fails.

---

# Merge Commit

A successful non-fast-forward merge produces a commit with multiple parents.

Conceptually:

```
Current Commit ─────┐
                    │
                    ▼
                Merge Commit
                    ▲
                    │
Target Commit ─────┘
```

The merge commit contains:

```
tree
parent 1
parent 2
author
message
```

The first parent represents the current branch history.

The second parent represents the merged branch history.

---

# Merge Commit Data Flow

```
Current Commit
      │
      ▼
   Snapshot A

Target Commit
      │
      ▼
   Snapshot B

Common Ancestor
      │
      ▼
   Snapshot Base

      │
      ▼
Three-Way Analysis
      │
      ▼
Merged Snapshot
      │
      ▼
   TreeBuilder
      │
      ▼
   Merge Tree
      │
      ▼
  Merge Commit
      │
      ├── Parent 1 = Current Commit
      └── Parent 2 = Target Commit
```

---

# Merge and Index

The Index is important because a merge must not accidentally overwrite staged user changes.

Before merging, the repository verifies that the Index does not contain changes relative to the current HEAD.

Conceptually:

```
HEAD Snapshot
      │
      ▼
    Compare
      ▲
      │
Index Snapshot
```

If they differ:

```
Cannot merge with staged changes
```

This prevents the merge operation from silently destroying staged work.

---

# Merge and Working Tree

The current implementation also treats staged state as a safety boundary.

The Working Tree may later receive more sophisticated dirty-state detection.

Future improvements include:

* detecting unstaged modifications
* detecting untracked files that would be overwritten
* preserving local modifications
* conflict markers
* manual conflict resolution
* merge continuation
* merge abort

---

# Merge State Safety

A failed merge should not silently advance repository history.

Important invariant:

```
failed merge
     ↓
current branch unchanged
     ↓
HEAD unchanged
     ↓
current commit unchanged
```

For conflicts, the current implementation reports the conflict instead of automatically creating a partially merged commit.

---

# 13. Relationship Between Diff and Merge

Diff and Merge share the Snapshot abstraction.

Diff performs:

```
Snapshot A
     │
     ▼
  Compare
     │
     ▼
Unified Diff
```

Merge performs:

```
Base Snapshot
     │
     ├──────────────┐
     ▼              ▼
Current Snapshot  Target Snapshot
     │              │
     └──────┬───────┘
            ▼
    Three-Way Analysis
            │
            ▼
       Merge Result
```

Therefore Diff establishes reusable infrastructure for future merge functionality.

---

# Complete Phase 15 Data Flow

```
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
      ▼                                    ▼
     Tree ◄──────── Object Database ──────┘
      │
      ▼
    Blobs


                ┌─────────────┐
                │    Diff     │
                └──────┬──────┘
                       │
      ┌────────────────┼────────────────┐
      │                │                │
      ▼                ▼                ▼
Working Tree         Index           Commit
      │                │                │
      └────────────────┼────────────────┘
                       ▼
                    Snapshot
                       │
                       ▼
                   LCS Diff
                       │
                       ▼
                Unified Output


                ┌─────────────┐
                │    Merge    │
                └──────┬──────┘
                       │
      ┌────────────────┼────────────────┐
      │                │                │
      ▼                ▼                ▼
     Base           Current           Target
   Snapshot         Snapshot         Snapshot
      │                │                │
      └────────────────┼────────────────┘
                       ▼
              Three-Way Analysis
                       │
            ┌──────────┴──────────┐
            │                     │
            ▼                     ▼
         Clean                 Conflict
            │                     │
            ▼                     ▼
      Merged Snapshot         Error State
            │
            ▼
         TreeBuilder
            │
            ▼
       Merge Tree
            │
            ▼
      Merge Commit
      ├── Parent 1
      └── Parent 2
```

---

# Relationship Between Subsystems

The architecture can be viewed as:

```
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

```
Working Tree ───────┐
                    │
Index ──────────────┼──► Diff
                    │
Commit ─────────────┘
```

Merge observes repository history and snapshots:

```
Current Commit ───────┐
                      │
Target Commit ────────┼──► Merge
                      │
Common Ancestor ──────┘
                      │
                      ▼
                Three-Way Merge
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
* working-tree restoration
* index synchronization

---

## `Reference`

Responsible for:

* reference representation
* reference paths
* reference values
* reading references
* writing references

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
* directory snapshot representation

---

## `Commit`

Responsible for:

* Tree reference
* parent references
* author
* message
* commit serialization
* commit deserialization

A merge commit may contain multiple parents.

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
* staging-state persistence
* index synchronization with checked-out snapshots

---

## `TreeBuilder`

Responsible for:

* converting Index state into Tree objects
* recursively creating directory Trees
* constructing a snapshot Tree from staged entries

---

## `Diff`

Responsible for:

* constructing state snapshots
* comparing snapshots
* detecting changes
* generating unified diff output

Diff is read-only.

---

## `Merge`

Responsible for:

* resolving current and target commits
* identifying common ancestry
* constructing snapshots
* performing three-way comparison
* detecting conflicts
* handling fast-forward merges
* handling already-up-to-date states
* creating merge commits for successful non-fast-forward merges

Merge should coordinate existing subsystems rather than duplicate their storage logic.

---

# Repository Layout

A Mini Git repository has approximately this structure:

```
project/
│
├── source files
│
└── .mini-git/
    ├── HEAD
    ├── index
    ├── objects/
    │
    └── refs/
        └── heads/
            ├── main
            └── feature
```

The Working Tree exists outside `.mini-git`.

The `.mini-git` directory stores repository metadata and persistent objects.

---

# Important Invariants

## Object Identity

Object ID must be determined by serialized content.

```
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

## Index Synchronization

After checkout, the Index and Working Tree must correspond to the checked-out commit.

---

## Diff Read-Only Behavior

Diff must not modify repository state.

---

## Merge Safety

A failed or conflicting merge must not silently advance the current branch.

---

## Merge Commit Parents

A non-fast-forward merge commit must preserve both histories:

```
parent 1 = current branch commit
parent 2 = target branch commit
```

---

## Snapshot Consistency

Snapshots used by Diff and Merge must represent the same path/content abstraction:

```
path → content
```

This allows the same comparison infrastructure to be reused across multiple repository operations.

---

# Current Architecture

At Phase 15:

```
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
      ▼                                    ▼
     Tree ◄──────── Object Database ──────┘
      │
      ▼
    Blobs


                 ┌──────────────┐
                 │     Diff     │
                 └──────┬───────┘
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


                 ┌──────────────┐
                 │    Merge     │
                 └──────┬───────┘
                        │
         ┌──────────────┼──────────────┐
         │              │              │
         ▼              ▼              ▼
        Base         Current         Target
      Snapshot       Snapshot       Snapshot
         │              │              │
         └──────────────┼──────────────┘
                        ▼
               Three-Way Analysis
                        │
              ┌─────────┴─────────┐
              │                   │
              ▼                   ▼
            Clean              Conflict
              │                   │
              ▼                   ▼
       Merged Snapshot         Error
              │
              ▼
          TreeBuilder
              │
              ▼
         Merge Tree
              │
              ▼
        Merge Commit
         ├── Parent 1
         └── Parent 2
```

---

# Phase 15 Architectural Principles

Mini Git follows these principles:

### Separation of Concerns

Storage, repository state, objects, staging, diffing, and merging remain separate.

### Immutable Objects

Persistent objects are treated as immutable.

### Deterministic Serialization

Object identity depends on deterministic serialized data.

### Explicit State

Working Tree, Index, HEAD, branches, commits, and snapshots are represented explicitly.

### Reusable Abstractions

Snapshots introduced for Diff are reused by Merge.

### History as a Graph

Commits form a directed history graph rather than a simple linear sequence.

### References as Pointers

Branches identify commit tips rather than owning histories.

### Read-Only Analysis

Diff observes repository state without modifying it.

### Safe State Transitions

Checkout and Merge must avoid silently destroying repository state.

### Testability

Each subsystem should be testable independently.

### Incremental Development

New phases build on previous abstractions rather than replacing them unnecessarily.

---

# Phase 15 Result

Phase 15 establishes the first complete merge architecture for Mini Git.

The project now contains the conceptual pipeline:

```
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
   History
      │
      ├──────────────────────┐
      │                      │
      ▼                      ▼
    Diff                   Merge
      │                      │
      ▼                      ▼
  Snapshots            Common Ancestor
      │                      │
      ▼                      ▼
   LCS Diff          Three-Way Analysis
                             │
                   ┌─────────┴─────────┐
                   │                   │
                   ▼                   ▼
                 Clean              Conflict
                   │
                   ▼
              Merge Snapshot
                   │
                   ▼
                TreeBuilder
                   │
                   ▼
              Merge Commit
              ├── Parent 1
              └── Parent 2
```

This architecture provides the foundation for future work such as:

* detailed conflict markers
* manual conflict resolution
* merge continuation
* merge abort
* recursive ancestry analysis
* tags
* repository integrity checking
* object garbage collection
* more advanced history visualization

The important architectural progression is:

```
Phase 13
   │
   ▼
Branches + Checkout
   │
   ▼
Phase 14
   │
   ▼
Diff + Snapshots
   │
   ▼
Phase 15
   │
   ▼
Three-Way Merge + Conflict Detection
   │
   ▼
Future
   │
   ▼
Conflict Resolution + Advanced History
```
