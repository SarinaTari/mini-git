# Mini Git Architecture

## Overview

Mini Git is a Git-inspired version control system implemented from scratch in C++20.

The project is designed to expose the internal mechanisms behind local version control rather than provide complete Git compatibility.

The architecture is organized around several major subsystems:

```text
CLI
 │
 ├── Repository
 ├── Index
 ├── Object Database
 ├── References / HEAD
 ├── History
 ├── Diff
 ├── Status
 └── Merge
```

Each subsystem has a focused responsibility.

Phase 16 extends the architecture from **three-way merge and conflict detection** into a **persistent merge-state and conflict-resolution system**.

The major architectural progression is:

```text
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
Phase 16
   │
   ▼
Persistent Merge State
   │
   ├── Conflict Markers
   ├── Conflict Tracking
   ├── Conflict Resolution
   ├── Merge Continuation
   ├── Merge Abort
   └── Two-Parent Merge Commit
```

---

# High-Level Architecture

```text
                         Mini Git CLI

                              │

                              ▼

                       Command Layer

                              │

       ┌──────────────────────┼──────────────────────┐
       │                      │                      │
       ▼                      ▼                      ▼
  Repository                Index                 Status
       │                      │                      │
       │                      │                      │
       └──────────────┬───────┴──────────────────────┘
                      │
                      ▼
               Object Database
                      │
             ┌────────┼────────┐
             │        │        │
             ▼        ▼        ▼
           Blob      Tree     Commit
                              │
                              ▼
                           History

       ┌──────────────────────────────────────────┐
       │                                          │
       ▼                                          ▼
     Diff                                       Merge
       │                                          │
       ▼                                          ▼
   Snapshots                              Three-Way Analysis
       │                                          │
       ▼                                  ┌───────┴────────┐
    LCS Diff                               │                │
                                           ▼                ▼
                                         Clean           Conflict
                                           │                │
                                           ▼                ▼
                                    Merged Snapshot    Persistent
                                                       Merge State
                                                            │
                                          ┌─────────────────┼─────────────────┐
                                          │                 │                 │
                                          ▼                 ▼                 ▼
                                    MERGE_HEAD      MERGE_CONFLICTS      MERGE_MSG
                                          │
                                          ▼
                                   Conflict Resolution
                                          │
                                    ┌─────┴─────┐
                                    ▼           ▼
                                  Continue     Abort
                                    │           │
                                    ▼           ▼
                              Merge Commit   Restore State
```

The architecture separates persistent repository objects from temporary repository state.

Persistent objects include:

```text
Blob
Tree
Commit
```

Repository state includes:

```text
HEAD
Branches
Index
Working Tree
Merge State
```

This distinction is especially important in Phase 16 because an unresolved merge is no longer represented simply as an error. It becomes a **persistent repository state** that can be inspected, resolved, continued, or aborted.

---

# Phase 16 Architecture

Phase 16 introduces persistent merge state.

The previous Phase 15 architecture handled conflicts by detecting them and reporting a failure.

Phase 16 changes this behavior.

Instead of:

```text
Merge
 │
 ▼
Conflict
 │
 ▼
Error
```

the architecture now supports:

```text
Merge
 │
 ▼
Conflict
 │
 ▼
Write Conflict Markers
 │
 ▼
Persist Merge State
 │
 ▼
User Resolves Conflicts
 │
 ▼
mini-git add <file>
 │
 ▼
Conflict Removed From Merge State
 │
 ▼
mini-git merge --continue
 │
 ▼
Merge Commit
```

The user may also abandon the merge:

```text
Merge State
 │
 ▼
mini-git merge --abort
 │
 ▼
Restore Original Commit
 │
 ▼
Restore Index
 │
 ▼
Clear Merge State
```

---

# Architectural Layers

## 1. Command Layer

The command layer handles CLI operations.

Examples:

```text
mini-git init

mini-git add file.txt

mini-git commit -m "message"

mini-git branch feature

mini-git checkout feature

mini-git diff

mini-git status

mini-git merge feature

mini-git merge --continue

mini-git merge --abort
```

The command layer coordinates subsystems rather than implementing their internal logic.

For example:

```text
mini-git merge feature

        │

        ▼

   Command Layer

        │

        ▼

      Merge

        │

   ┌────┼────┐
   ▼    ▼    ▼
 Repo Trees Commits
```

For an active merge:

```text
mini-git merge --continue

        │

        ▼

   Command Layer

        │

        ▼

      Merge

        │

        ├── Repository
        ├── Index
        ├── Merge State
        ├── TreeBuilder
        └── Commit
```

The command layer is therefore responsible for selecting the appropriate operation while the underlying classes perform repository logic.

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
* merge-state persistence
* merge-state inspection
* merge conflict tracking
* merge-state cleanup

Conceptually:

```text
Repository

 ├── root
 ├── .mini-git
 ├── HEAD
 ├── branches
 ├── checkout
 ├── current commit
 └── merge state
```

The Repository does not own the actual contents of every object.

That responsibility belongs to `ObjectDatabase`.

Repository-level merge state is stored inside `.mini-git`.

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

The objects form a hierarchy:

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

Merge state is deliberately not represented as a normal object.

Instead, it is repository metadata because merge state describes an **in-progress operation**, not an immutable historical object.

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

Blob objects are immutable after storage.

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

Trees therefore provide deterministic directory snapshots.

---

# Commit

A Commit represents a repository snapshot plus history metadata.

Conceptually:

```text
Commit

 ├── tree
 ├── parent(s)
 ├── author
 └── message
```

A normal commit contains one parent:

```text
Commit C

   │

   ▼

Commit B

   │

   ▼

Commit A
```

A merge commit contains multiple parents:

```text
Commit B ──────┐
               │
               ▼
            Merge Commit
               ▲
               │
Commit C ──────┘
```

The first parent is the commit from the current branch.

The second parent is the commit being merged.

This allows the commit graph to preserve both histories.

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

Object identity is therefore content-based.

This property allows the same object to be reused by multiple commits.

---

# 5. Index

The Index represents staged state.

Conceptually:

```text
path → Blob ID
```

Example:

```text
main.cpp  → abc123...
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

The Index is also important during checkout and merge because it represents the currently staged snapshot.

During an active merge, the Index also represents the user's resolved state.

---

# Index Synchronization

After checkout, the Working Tree and Index must represent the same target snapshot.

Conceptually:

```text
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

```text
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

During merge continuation:

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
Merge Tree
     │
     ▼
Merge Commit
```

The Index becomes the authoritative resolved snapshot used to create the merge commit.

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

Nested directories become nested Tree objects.

TreeBuilder can construct Trees from indexed state rather than directly from the filesystem.

This is especially important during merge continuation because the merge commit must be created from the user's resolved Index state.

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

The contents of each reference are commit IDs.

A reference therefore acts as a movable name pointing to a commit.

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

  │

  ▼

Commit
```

The current implementation primarily operates with symbolic branch HEADs.

During an active merge, HEAD remains associated with the current branch.

The merge operation does not move the branch reference until the merge is successfully continued.

---

# 9. Branches

Branches are references.

They do not contain copies of commits.

Example:

```text
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

During a merge:

```text
main
 │
 ▼
Current Commit

feature
 │
 ▼
Target Commit
```

Neither branch is changed when the merge initially detects conflicts.

The current branch is updated only after a successful merge continuation.

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

    Index

      │

      ▼

HEAD → refs/heads/feature
```

Checkout therefore synchronizes three important pieces of repository state:

```text
HEAD

Index

Working Tree
```

During an active merge, checkout is restricted.

The repository must not allow the user to switch branches while merge state is unresolved.

Conceptually:

```text
Merge in progress
       │
       ▼
checkout
       │
       ▼
Rejected
```

This protects the merge state from being accidentally abandoned or corrupted.

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

```text
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

```text
path → content
```

For example:

```text
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
main.cpp        → content

README.md       → content

src/parser.cpp  → content
```

---

# Index Snapshot

The Index contains Blob IDs rather than raw file contents.

Therefore:

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

Blob Object

  │

  ▼

Blob Content
```

The final representation is:

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

```text
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

```text
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

```text
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

```text
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

The Diff subsystem uses the Longest Common Subsequence concept.

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

representing the longest common subsequence between portions of the two line sequences.

The table is then used to reconstruct the change sequence.

---

# Unified Diff

The final result uses unified-diff style output:

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

# 12. Merge

The Merge subsystem combines two branches or repository histories.

Its primary responsibility is to determine how the current branch and target branch differ from their common ancestor.

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

Three-Way Merge

      │

  ┌───┴────┐
  ▼        ▼
Clean   Conflict
  │        │
  ▼        ▼
Result   Merge State
```

Merge uses existing repository abstractions rather than duplicating object-storage logic.

---

# Three-Way Merge

A three-way merge compares three snapshots:

```text
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

```text
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

```text
hello world
```

Three-way comparison allows the algorithm to identify this safely.

---

# Three-Way Merge Cases

For each path, compare:

```text
Base
Current
Target
```

The major cases are:

## Both Unchanged

```text
Base = Current
Base = Target
```

Result:

```text
Base
```

---

## Only Current Changed

```text
Base = Target
Current != Base
```

Result:

```text
Current
```

---

## Only Target Changed

```text
Base = Current
Target != Base
```

Result:

```text
Target
```

---

## Both Changed Identically

```text
Current = Target
Current != Base
```

Result:

```text
Current
```

No conflict exists because both sides produced the same content.

---

## Both Changed Differently

```text
Current != Base
Target != Base
Current != Target
```

Result:

```text
Conflict
```

The merge cannot automatically choose a version.

Phase 16 now converts this conflict into a persistent merge state rather than simply returning an error.

---

# File-Level Conflict Detection

At the current architectural level, conflicts are detected by comparing snapshot contents for each path.

Conceptually:

```text
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

Phase 16 records the conflicting path so that the user can resolve it manually.

---

# Common Ancestor

The common ancestor is a commit reachable from both the current and target commits.

Conceptually:

```text
                 Base

                /    \

               /      \

          Current     Target
```

The Merge subsystem identifies the appropriate common ancestor before performing the three-way comparison.

This ancestor provides the baseline against which both sides are compared.

---

# Commit Graph Traversal

Merge therefore depends on commit history traversal.

Example:

```text
        A
       / \
      B   C
      |   |
      D   E
```

If the current branch points to D and the target branch points to E:

```text
Current = D

Target  = E

Base    = A
```

The merge compares:

```text
Snapshot(A)

Snapshot(D)

Snapshot(E)
```

---

# Fast-Forward Merge

A fast-forward merge occurs when the current commit is an ancestor of the target commit.

Example:

```text
A → B → C
```

Current:

```text
A
```

Target:

```text
C
```

Because A is already part of C's history, no merge commit is required.

The branch reference can simply move:

```text
main

  │

  ▼

  A
```

becomes:

```text
main

  │

  ▼

  C
```

The Working Tree and Index are then synchronized with the target snapshot.

No merge state is created because no conflict-resolution workflow is required.

---

# Already Up-to-Date

If the target commit is already an ancestor of the current commit:

```text
A → B → C

     ↑
   Target

          ↑
        Current
```

then there is nothing to merge.

The current branch remains unchanged.

No merge state is created.

---

# Non-Conflicting Merge

Suppose:

```text
Base
  │
  ├── Current changes file1.txt
  │
  └── Target changes file2.txt
```

The changes affect different paths.

Therefore:

```text
Base

  │

  ├── Current change

  │

  └── Target change
```

can be combined into a single resulting snapshot.

The merge can then create a merge commit.

No unresolved merge state remains after a successful non-conflicting merge.

---

# Conflicting Merge

Suppose:

```text
Base

  │

  ├── Current changes hello.txt
  │
  └── Target changes hello.txt differently
```

The result is:

```text
Conflict: hello.txt
```

Phase 16 does not silently choose one version.

Instead, the architecture transitions into an active merge state:

```text
Conflict

   │

   ▼

Write Conflict Marker

   │

   ▼

Persist Merge State

   │

   ▼

User Resolution
```

The current branch remains at its original commit while the Working Tree contains the conflict representation.

---

# Conflict Markers

For conflicting file contents, Mini Git writes conflict markers into the Working Tree.

The conceptual format is:

```text
<<<<<<< ours
<current content>
=======
<target content>
>>>>>>> feature
```

The markers identify the two competing versions.

For example:

```text
<<<<<<< ours
hello from main
=======
hello from feature
>>>>>>> feature
```

The conflict marker is a temporary Working Tree representation.

It is not itself stored as a Blob object until the user stages the resolved file.

---

# Conflict Resolution

The user resolves a conflict by editing the affected file and removing the conflict markers.

For example, the user may change:

```text
<<<<<<< ours
hello from main
=======
hello from feature
>>>>>>> feature
```

into:

```text
hello from both branches
```

The resolved file is then staged:

```text
mini-git add hello.txt
```

The `add` operation stores the resolved contents as a Blob and updates the Index.

During an active merge, `add` also removes the resolved path from the unresolved conflict list.

Conceptually:

```text
MERGE_CONFLICTS

hello.txt
main.cpp
```

After:

```text
mini-git add hello.txt
```

the state becomes:

```text
MERGE_CONFLICTS

main.cpp
```

When all conflicts have been staged:

```text
MERGE_CONFLICTS

<empty>
```

the merge can continue.

---

# Persistent Merge State

The most important Phase 16 architectural change is that merge state is persisted inside `.mini-git`.

The merge state consists of:

```text
.mini-git/

├── MERGE_HEAD
├── MERGE_ORIG_HEAD
├── MERGE_CONFLICTS
└── MERGE_MSG
```

Each file has a specific responsibility.

---

# MERGE_HEAD

`MERGE_HEAD` stores the commit being merged into the current branch.

Conceptually:

```text
MERGE_HEAD

     │

     ▼

Target Commit
```

For example:

```text
Current Commit = A
Target Commit  = B

MERGE_HEAD = B
```

The file allows the repository to remember the target side of the merge after the original `merge` command has returned.

---

# MERGE_ORIG_HEAD

`MERGE_ORIG_HEAD` stores the original current commit before the merge started.

Conceptually:

```text
Before Merge

Current Branch
      │
      ▼
Commit A
```

The repository records:

```text
MERGE_ORIG_HEAD = A
```

This information is required for:

* merge continuation
* merge commit parent selection
* merge abort
* restoring the pre-merge repository state

---

# MERGE_CONFLICTS

`MERGE_CONFLICTS` stores unresolved conflict paths.

Example:

```text
hello.txt
src/main.cpp
```

The file represents the current unresolved conflict set.

Conceptually:

```text
Merge State

      │

      ▼

MERGE_CONFLICTS

      │

      ├── hello.txt
      └── src/main.cpp
```

When a conflicted file is staged through `mini-git add`, its path is removed from the unresolved conflict list.

---

# MERGE_MSG

`MERGE_MSG` stores the merge commit message.

For example:

```text
Merge branch 'feature'
```

The message is preserved across commands so that:

```text
mini-git merge feature
```

can enter a merge state and later:

```text
mini-git merge --continue
```

can create the final merge commit using the stored message.

A user-provided continuation message may also be used when supported by the command interface.

---

# Merge State Lifecycle

The complete Phase 16 lifecycle is:

```text
No Merge

   │

   │ mini-git merge feature
   ▼

Merge Analysis

   │

   ├───────────────┐
   │               │
   ▼               ▼
Clean           Conflict
   │               │
   ▼               ▼
Merge Commit   Write Markers
                   │
                   ▼
             Persist State
                   │
                   ▼
             Resolve Files
                   │
                   ▼
              mini-git add
                   │
                   ▼
            Check Conflicts
                   │
             ┌─────┴─────┐
             ▼           ▼
        Still Unresolved  All Resolved
             │                 │
             ▼                 ▼
          Continue?       merge --continue
                                 │
                                 ▼
                           Merge Commit
                                 │
                                 ▼
                           Clear State
```

---

# Merge State Detection

The Repository can determine whether a merge is active by checking whether the merge-state metadata exists.

Conceptually:

```text
.mini-git/MERGE_HEAD exists
            │
            ▼
      Merge in progress
```

The Repository provides access to:

```text
merge_in_progress()

merge_head()

merge_orig_head()

merge_message()

merge_conflicts()

has_merge_conflict(path)
```

This keeps merge-state interpretation inside the Repository abstraction instead of scattering filesystem checks throughout the command layer.

---

# Merge State Creation

When a conflicting merge occurs, the Merge subsystem creates the merge state.

The process is:

```text
Current Commit
      │
      ▼
Target Commit
      │
      ▼
Three-Way Merge
      │
      ▼
Conflict Detection
      │
      ▼
Write Conflict Markers
      │
      ▼
Repository::begin_merge_state()
      │
      ├── MERGE_HEAD
      ├── MERGE_ORIG_HEAD
      ├── MERGE_CONFLICTS
      └── MERGE_MSG
```

The current branch reference is not advanced.

This is critical.

The branch still points to the original current commit.

---

# Merge State and Working Tree

During a conflicting merge, the Working Tree becomes a partially merged state.

For example:

```text
Before Merge:

hello.txt

hello from main
```

After conflict:

```text
hello.txt

<<<<<<< ours
hello from main
=======
hello from feature
>>>>>>> feature
```

The Working Tree therefore represents an unresolved merge state.

The Index still provides the staging boundary.

The user must resolve the file and stage the result before the merge can continue.

---

# Merge State and Index

The Index becomes the mechanism for recording conflict resolution.

Conceptually:

```text
Conflict File
     │
     ▼
User edits file
     │
     ▼
Resolved Working Tree
     │
     ▼
mini-git add file
     │
     ▼
Blob
     │
     ▼
Index
```

The Index therefore transitions from:

```text
pre-merge staged state
```

to:

```text
resolved merge snapshot
```

before merge continuation.

---

# Merge State and Status

Phase 16 extends repository status information during an active merge.

The status system can expose the fact that a merge is in progress and identify unresolved conflicts.

Conceptually:

```text
mini-git status

Merge in progress.

Unresolved conflicts:

  hello.txt
  src/main.cpp
```

After resolving one file:

```text
mini-git add hello.txt
```

status can report:

```text
Merge in progress.

Unresolved conflicts:

  src/main.cpp
```

After all conflicts are resolved:

```text
Merge in progress.

All conflicts resolved.

Run:
  mini-git merge --continue
```

The exact presentation belongs to the command layer, while the Repository owns the merge-state data.

---

# Merge Continuation

`mini-git merge --continue` completes an active merge.

The operation follows this sequence:

```text
merge --continue

      │

      ▼

Check Merge State

      │

      ▼

Read MERGE_HEAD

      │

      ▼

Read MERGE_ORIG_HEAD

      │

      ▼

Read MERGE_CONFLICTS

      │

      ▼

Verify No Unresolved Conflicts

      │

      ▼

Read Index

      │

      ▼

TreeBuilder

      │

      ▼

Merged Tree

      │

      ▼

Create Merge Commit

      │

      ├── Parent 1 = MERGE_ORIG_HEAD
      └── Parent 2 = MERGE_HEAD

      │

      ▼

Update Current Branch

      │

      ▼

Clear Merge State
```

The merge commit is created only after all conflicts have been resolved.

---

# Merge Continuation Safety

A merge cannot be continued while unresolved conflicts remain.

For example:

```text
MERGE_CONFLICTS

hello.txt
```

Then:

```text
mini-git merge --continue
```

must fail.

The repository remains in the active merge state.

After:

```text
mini-git add hello.txt
```

the conflict is removed from the unresolved set.

Only when:

```text
MERGE_CONFLICTS

<empty>
```

can the merge continue.

This prevents Mini Git from creating a merge commit containing unresolved conflict markers.

---

# Merge Commit After Continuation

After all conflicts have been resolved, TreeBuilder constructs the final Tree from the Index.

The resulting merge commit contains:

```text
tree = resolved merge tree

parent 1 = original current commit

parent 2 = target commit

author = merge author

message = merge message
```

Conceptually:

```text
Current Commit ───────────┐
                          │
                          ▼
                       Merge Commit
                          ▲
                          │
Target Commit ────────────┘
```

This preserves both sides of the history.

---

# Merge Abort

`mini-git merge --abort` cancels an active merge.

The operation uses `MERGE_ORIG_HEAD` to restore the repository to the state before the merge.

Conceptually:

```text
Active Merge

     │

     ▼

merge --abort

     │

     ▼

Read MERGE_ORIG_HEAD

     │

     ▼

Restore Original Commit

     │

     ▼

Restore Working Tree

     │

     ▼

Rebuild Index

     │

     ▼

Clear Merge State
```

The current branch reference remains at the original commit.

No merge commit is created.

---

# Merge Abort State Restoration

Suppose:

```text
Before Merge:

main → A
```

The user runs:

```text
mini-git merge feature
```

and a conflict occurs.

The state becomes:

```text
main → A

MERGE_ORIG_HEAD = A
MERGE_HEAD      = B
```

After:

```text
mini-git merge --abort
```

the repository returns to:

```text
main → A
```

The conflict markers are removed as the original snapshot is restored.

The Index is rebuilt from the original commit.

The merge metadata is deleted.

---

# Merge State Cleanup

After successful continuation:

```text
Merge Commit
     │
     ▼
Clear Merge State
```

After abort:

```text
Original Commit Restored
     │
     ▼
Clear Merge State
```

Merge state must not remain after the operation is complete.

Conceptually:

```text
MERGE_HEAD
MERGE_ORIG_HEAD
MERGE_CONFLICTS
MERGE_MSG
```

are removed.

The repository returns to its normal state.

---

# Normal Commit During Merge

A normal:

```text
mini-git commit
```

must not bypass the merge workflow while an active merge exists.

The user must instead use:

```text
mini-git merge --continue
```

This keeps merge-specific parent handling centralized.

Conceptually:

```text
Merge in progress
       │
       ▼
normal commit
       │
       ▼
Rejected
       │
       ▼
Use merge --continue
```

This prevents accidental creation of an ordinary one-parent commit instead of the required two-parent merge commit.

---

# Checkout During Merge

Checkout is also restricted while a merge is active.

Conceptually:

```text
Merge in progress
       │
       ▼
checkout branch
       │
       ▼
Rejected
```

The user must first either:

```text
mini-git merge --continue
```

or:

```text
mini-git merge --abort
```

This keeps the repository state consistent.

---

# Conflict Resolution Model

The Phase 16 conflict-resolution model is:

```text
                Conflict

                   │

                   ▼

          Conflict Marker File

                   │

                   ▼

             User Editing

                   │

                   ▼

          Resolved Working Tree

                   │

                   ▼

             mini-git add

                   │

                   ▼

                 Blob

                   │

                   ▼

                 Index

                   │

                   ▼

          Remove From Conflicts

                   │

                   ▼

          merge --continue
```

This is intentionally simple and educational.

The current architecture focuses on file-level content conflicts.

More advanced conflict types can be added later.

---

# Current Conflict Scope

Phase 16 focuses primarily on conflicting file contents.

The current architecture does not claim complete Git-compatible handling of every possible conflict type.

Future improvements may include:

* delete/modify conflicts
* modify/delete conflicts
* file/directory conflicts
* rename detection
* rename/modify conflicts
* directory restructuring conflicts
* more advanced merge algorithms
* binary file conflict handling

The current implementation should therefore be understood as a focused educational merge-resolution system.

---

# 13. Relationship Between Diff and Merge

Diff and Merge share the Snapshot abstraction.

Diff performs:

```text
Snapshot A

     │

     ▼

  Compare

     │

     ▼

Unified Diff
```

Merge performs:

```text
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

Therefore Diff establishes reusable infrastructure for Merge functionality.

The same path/content representation allows both systems to reason about repository state consistently.

---

# Complete Phase 16 Data Flow

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
      Merged Snapshot       Conflict Markers
            │                     │
            ▼                     ▼
        TreeBuilder        Persistent State
            │                     │
            ▼              ┌──────┼──────┐
       Merge Commit        │      │      │
        ├── Parent 1       ▼      ▼      ▼
        └── Parent 2    HEAD   ORIG   CONFLICTS
                              HEAD
                                   │
                                   ▼
                                  MSG
                                   │
                                   ▼
                              Resolution
                                   │
                                   ▼
                              mini-git add
                                   │
                                   ▼
                              merge --continue
                                   │
                                   ▼
                              Merge Commit
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

Merge observes repository history and snapshots:

```text
Current Commit ───────┐
                      │
Target Commit ────────┼──► Merge
                      │
Common Ancestor ──────┘
                      │
                      ▼
                Three-Way Merge
```

During conflicts, Merge additionally interacts with persistent Repository state:

```text
Merge
 │
 ├── Repository
 │      │
 │      ├── MERGE_HEAD
 │      ├── MERGE_ORIG_HEAD
 │      ├── MERGE_CONFLICTS
 │      └── MERGE_MSG
 │
 ├── Working Tree
 │
 └── Index
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
* merge-state persistence
* merge-state inspection
* unresolved conflict tracking
* merge-state cleanup

Important merge-state operations include:

```text
merge_in_progress()

merge_head()

merge_orig_head()

merge_message()

merge_conflicts()

has_merge_conflict(path)

begin_merge_state(...)

resolve_merge_conflict(path)

clear_merge_state()
```

---

# `Reference`

Responsible for:

* reference representation
* reference paths
* reference values
* reading references
* writing references

---

# `Object`

Base abstraction for persistent objects.

---

# `Blob`

Responsible for:

* file content
* Blob serialization
* Blob deserialization

---

# `Tree`

Responsible for:

* Tree entries
* Tree serialization
* Tree deserialization
* directory snapshot representation

---

# `Commit`

Responsible for:

* Tree reference
* parent references
* author
* message
* commit serialization
* commit deserialization

A merge commit may contain multiple parents.

---

# `ObjectDatabase`

Responsible for:

* storing serialized objects
* retrieving objects
* object paths
* content-addressable persistence

---

# `Index`

Responsible for:

* staged paths
* Blob IDs
* staging-state persistence
* index synchronization with checked-out snapshots
* recording resolved merge files

---

# `TreeBuilder`

Responsible for:

* converting Index state into Tree objects
* recursively creating directory Trees
* constructing a snapshot Tree from staged entries
* constructing the final Tree used by merge continuation

---

# `Status`

Responsible for:

* examining Working Tree state
* detecting modified files
* detecting deleted files
* detecting untracked files
* exposing repository status information

During Phase 16, status also participates in reporting merge-state information such as unresolved conflict paths.

Status remains an analysis subsystem and does not create commits or modify branch references.

---

# `Diff`

Responsible for:

* constructing state snapshots
* comparing snapshots
* detecting changes
* generating unified diff output

Diff is read-only.

---

# `Merge`

Responsible for:

* resolving current and target commits
* identifying common ancestry
* constructing snapshots
* performing three-way comparison
* detecting conflicts
* handling fast-forward merges
* handling already-up-to-date states
* creating merged snapshots
* writing conflict markers
* creating persistent merge state
* continuing resolved merges
* creating two-parent merge commits
* aborting active merges

Merge coordinates existing subsystems rather than duplicating their storage logic.

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
    ├── index
    │
    ├── objects/
    │
    ├── refs/
    │   └── heads/
    │       ├── main
    │       └── feature
    │
    ├── MERGE_HEAD
    ├── MERGE_ORIG_HEAD
    ├── MERGE_CONFLICTS
    └── MERGE_MSG
```

The merge files exist only while a merge is active.

A normal repository may therefore contain:

```text
.mini-git/

├── HEAD
├── index
├── objects/
└── refs/
```

An active merge adds:

```text
MERGE_HEAD
MERGE_ORIG_HEAD
MERGE_CONFLICTS
MERGE_MSG
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

## Index Synchronization

After checkout, the Index and Working Tree must correspond to the checked-out commit.

After merge continuation, the Index represents the resolved merge snapshot from which the merge Tree is created.

---

## Diff Read-Only Behavior

Diff must not modify repository state.

---

## Merge Safety

A failed or conflicting merge must not silently advance the current branch.

---

## Merge Commit Parents

A non-fast-forward merge commit must preserve both histories:

```text
parent 1 = current branch commit

parent 2 = target branch commit
```

---

## Snapshot Consistency

Snapshots used by Diff and Merge must represent the same path/content abstraction:

```text
path → content
```

This allows the same comparison infrastructure to be reused across multiple repository operations.

---

## Persistent Merge State

If a merge is active, the repository must retain enough information to continue or abort it.

The essential state is:

```text
MERGE_HEAD
MERGE_ORIG_HEAD
MERGE_CONFLICTS
MERGE_MSG
```

---

## No Merge Commit With Unresolved Conflicts

A merge commit must not be created while unresolved conflict paths remain.

Conceptually:

```text
MERGE_CONFLICTS != empty

        ↓

merge --continue

        ↓

Rejected
```

Only:

```text
MERGE_CONFLICTS = empty
```

allows merge continuation.

---

## Merge Abort Restores Original State

After:

```text
mini-git merge --abort
```

the repository must return to the original merge starting point.

Conceptually:

```text
Original Commit
      │
      ▼
MERGE_ORIG_HEAD
      │
      ▼
Restore Working Tree
      │
      ▼
Restore Index
```

---

## Merge State Cleanup

After either successful continuation or abort:

```text
MERGE_HEAD
MERGE_ORIG_HEAD
MERGE_CONFLICTS
MERGE_MSG
```

must be cleared.

---

## Commit Restrictions During Merge

Normal commit creation must not bypass an active merge.

The merge must be completed through:

```text
mini-git merge --continue
```

---

## Checkout Restrictions During Merge

The repository must not switch branches while an active merge is unresolved.

The user must either continue or abort the merge first.

---

# Current Architecture

At Phase 16:

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
       Merged Snapshot      Conflict Markers
              │                   │
              ▼                   ▼
          TreeBuilder        Merge State
              │                   │
              ▼          ┌────────┼────────┐
         Merge Tree       │        │        │
              │           ▼        ▼        ▼
              │        HEAD      ORIG    CONFLICTS
              │                  HEAD
              │                       │
              │                       ▼
              │                      MSG
              │                       │
              │                       ▼
              │                  User Resolution
              │                       │
              │                       ▼
              │                  mini-git add
              │                       │
              │                       ▼
              │                merge --continue
              │                       │
              └───────────────────────┘
                                      │
                                      ▼
                                Merge Commit
                                ├── Parent 1
                                └── Parent 2
```

---

# Phase 16 Architectural Principles

Mini Git follows these principles:

### Separation of Concerns

Storage, repository state, objects, staging, diffing, status, and merging remain separate.

### Immutable Objects

Persistent objects are treated as immutable.

### Deterministic Serialization

Object identity depends on deterministic serialized data.

### Explicit State

Working Tree, Index, HEAD, branches, commits, snapshots, and merge state are represented explicitly.

### Reusable Abstractions

Snapshots introduced for Diff are reused by Merge.

### History as a Graph

Commits form a directed history graph rather than a simple linear sequence.

### References as Pointers

Branches identify commit tips rather than owning histories.

### Read-Only Analysis

Diff and status observe repository state without performing history-changing operations.

### Persistent Operations

An operation that cannot be completed in a single command can persist its state and resume later.

### Safe State Transitions

Checkout, merge, continuation, and abort must avoid silently destroying repository state.

### Explicit Conflict Resolution

Conflicts are represented explicitly and must be resolved before a merge commit can be created.

### Testability

Each subsystem should be testable independently.

### Incremental Development

New phases build on previous abstractions rather than replacing them unnecessarily.

---

# Phase 16 Merge Data Flow

The complete conflict-resolution architecture can be summarized as:

```text
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
       ┌─────┴─────┐
       │           │
       ▼           ▼
     Clean      Conflict
       │           │
       ▼           ▼
Merged Snapshot  Conflict
       │         Markers
       │           │
       ▼           ▼
 TreeBuilder   Merge State
       │           │
       │     ┌─────┼─────┐
       │     │     │     │
       │     ▼     ▼     ▼
       │   HEAD  ORIG  CONFLICTS
       │         HEAD
       │                 │
       │                 ▼
       │                MSG
       │                 │
       │                 ▼
       │          User Resolution
       │                 │
       │                 ▼
       │            mini-git add
       │                 │
       │                 ▼
       │          Conflicts Updated
       │                 │
       │                 ▼
       │        merge --continue
       │                 │
       ▼                 ▼
    Merge Tree      Resolved Index
       │                 │
       └────────┬────────┘
                ▼
          Merge Commit
          ├── Parent 1
          └── Parent 2
                │
                ▼
        Clear Merge State
```

---

# Phase 16 Abort Data Flow

```text
Active Merge

     │

     ▼

MERGE_ORIG_HEAD

     │

     ▼

Original Commit

     │

     ├───────────────┐
     │               │
     ▼               ▼
Working Tree       Index
     │               │
     └───────┬───────┘
             ▼
       Restore State
             │
             ▼
       Clear Merge State
             │
             ▼
        Normal Repository
```

---

# Phase 16 Result

Phase 16 extends Mini Git from a system that can detect merge conflicts into a system that can **persist, inspect, resolve, continue, and abort an interrupted merge**.

The architecture now contains the complete conceptual pipeline:

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
                   │                   │
                   ▼                   ▼
            Merged Snapshot      Conflict Markers
                   │                   │
                   ▼                   ▼
                TreeBuilder      Persistent State
                   │                   │
                   ▼             ┌─────┼─────┐
              Merge Commit       │     │     │
              ├── Parent 1       ▼     ▼     ▼
              └── Parent 2     HEAD  ORIG  CONFLICTS
                                     HEAD
                                           │
                                           ▼
                                          MSG
                                           │
                                           ▼
                                      Resolution
                                           │
                                           ▼
                                      mini-git add
                                           │
                                           ▼
                                   merge --continue
                                           │
                                           ▼
                                     Merge Commit
                                           │
                                           ▼
                                    Clear State
```

Phase 16 also introduces the alternative recovery path:

```text
Conflict

   │

   ▼

Persistent Merge State

   │

   ▼

merge --abort

   │

   ▼

Restore Original Commit

   │

   ▼

Restore Working Tree + Index

   │

   ▼

Clear Merge State
```

The architectural progression is now:

```text
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

Phase 16

   │

   ▼

Persistent Merge State

   │

   ├── Conflict Markers
   ├── Conflict Tracking
   ├── Status During Merge
   ├── Conflict Resolution
   ├── Merge Continuation
   ├── Merge Abort
   └── Two-Parent Merge Commit

   │

   ▼

Future

   │

   ├── Delete/Modify Conflicts
   ├── Rename Detection
   ├── Advanced Merge Algorithms
   ├── Tags
   ├── Repository Integrity Checking
   ├── Object Garbage Collection
   └── Advanced History Visualization
```

Phase 16 therefore establishes a substantially more complete merge architecture while preserving the project's original goals:

* understand version-control internals
* use explicit repository state
* keep objects immutable
* separate storage from repository operations
* make state transitions safe
* make conflicts visible rather than silently resolving them
* allow interrupted operations to resume
* allow destructive operations to be safely aborted
* preserve both histories in merge commits
* keep the implementation modular and testable

The resulting architecture is no longer limited to:

```text
detect conflict → fail
```

It now supports:

```text
detect conflict
      ↓
persist state
      ↓
inspect state
      ↓
resolve conflict
      ↓
stage resolution
      ↓
continue merge
      ↓
create two-parent commit
```

or:

```text
detect conflict
      ↓
persist state
      ↓
abort merge
      ↓
restore original state
```

This forms the architectural foundation for more advanced version-control features in future phases.


# Phase 17 — Tags

Phase 17 extends the reference architecture with immutable commit tags.

A tag is a named reference to a specific commit. Unlike a branch, it does not move automatically when new commits are created.

```text
.mini-git/
└── refs/
    ├── heads/
    │   ├── main
    │   └── feature
    │
    └── tags/
        ├── v1.0
        └── v1.1
```

Each tag file contains the commit ID it references. The commit remains in the object database independently of the tag.

## Tag Data Flow

```text
mini-git tag v1.0
        │
        ▼
    Repository
        │
        ▼
    resolve HEAD
        │
        ▼
  verify Commit object
        │
        ▼
 Reference("refs/tags/v1.0")
        │
        ▼
 .mini-git/refs/tags/v1.0
        │
        ▼
      Commit ID
```

Creating a tag at an explicit commit follows the same path, except the supplied commit ID is validated instead of resolving `HEAD`.

## Branches and Tags

Branches and tags both use references, but they represent different semantics:

```text
Branch:

main ───────────────► Commit D
                       │
                       │ new commit
                       ▼
main ───────────────► Commit E


Tag:

v1.0 ───────────────► Commit D
                       │
                       │ new commit
                       ▼
v1.0 ───────────────► Commit D
```

The branch reference moves because a new commit updates it. The tag remains fixed at the commit it originally identified.

## Tag Validation

Before creating a tag, Mini Git verifies:

1. the tag name is non-empty
2. the tag reference does not already exist
3. the target commit exists in the object database
4. the target object can be deserialized as a `Commit`
5. the tag name cannot escape the `refs/tags` directory

The existing `Reference` validation provides the path-safety boundary.

## Tag Deletion

Deleting a tag removes only its reference file:

```text
refs/tags/v1.0
        │
        ▼
     delete
        │
        ▼
 reference removed
        │
        ▼
 commit object remains
```

This demonstrates an important property of content-addressable storage: names and references can change without mutating or deleting the underlying immutable object.

## Tag Architecture

The Phase 17 architecture is:

```text
                         CLI
                          │
                          ▼
                      Repository
                          │
             ┌────────────┴────────────┐
             │                         │
             ▼                         ▼
        refs/heads/                refs/tags/
             │                         │
             ▼                         ▼
          Branches                    Tags
             │                         │
             └────────────┬────────────┘
                          │
                          ▼
                       Commit ID
                          │
                          ▼
                   Object Database
                          │
                          ▼
                        Commit
```

This keeps branch and tag references conceptually consistent and avoids introducing a second persistence mechanism.

# Phase 17 Result

Mini Git can now give stable human-readable names to important commits while preserving the existing immutable object model.

The repository progression is now:

```text
Repository
   │
   ▼
Objects
   │
   ▼
Index
   │
   ▼
Commits
   │
   ▼
History
   │
   ├───────────────┐
   ▼               ▼
Branches          Tags
   │               │
   ▼               ▼
Checkout        Stable Names
   │               │
   └───────┬───────┘
           ▼
          Diff
           │
           ▼
         Merge
           │
           ▼
 Persistent Merge State
           │
           ▼
   Conflict Resolution
           │
           ▼
Future Integrity / Maintenance
```

Phase 17 therefore reinforces the central architectural idea of Mini Git: **references provide names for immutable objects, while the objects themselves remain independent of those names.**
