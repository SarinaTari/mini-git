# Mini Git Merge

## Overview

Phase 15 introduces the **Merge subsystem**.

Merge combines changes from one branch into another while preserving the repository's history.

The Merge subsystem builds upon the snapshot and comparison architecture introduced in Phase 14.

At a high level:

```text
Current Branch
      │
      ▼
   Current
   Snapshot
      │
      │
      ├──────────────┐
      │              │
      ▼              ▼
Merge Base        Target Branch
      │              │
      ▼              ▼
 Base Snapshot    Target Snapshot
          \        /
           \      /
            ▼    ▼
        Three-Way
          Merge
             │
             ▼
       Result / Conflict
```

Phase 15 supports the fundamental merge cases:

* fast-forward merge
* already-up-to-date merge
* non-conflicting merge
* conflicting merge
* merge commits with multiple parents

The Merge subsystem uses repository snapshots to reason about changes rather than directly comparing arbitrary filesystem states.

---

# Why Merge?

Branches allow development histories to diverge.

For example:

```text
A
│
B
│
C
│
└── main
```

A new branch can be created:

```text
A
│
B
├── C ── D ── feature
│
└── main
```

The branches now contain different histories.

Eventually the changes from `feature` may need to be incorporated into `main`.

That operation is a **merge**.

Conceptually:

```text
main
  \
   \
    Merge
   /
feature
```

The purpose of merge is therefore:

> Combine the changes represented by two histories into a new repository state.

---

# Branch Divergence

Consider:

```text
        C ── D
       /       \
A ── B          ?
       \       /
        E ── F
```

The branches share history until commit `B`.

After that point they developed independently.

The shared commit:

```text
B
```

is the **common ancestor** or **merge base**.

The merge needs to determine:

```text
What changed from B → D?

What changed from B → F?

Can those changes be combined?
```

This is the foundation of three-way merging.

---

# Three-Way Merge

Mini Git uses three repository states when reasoning about a merge:

```text
Base
 │
 ├── Current
 │
 └── Target
```

Where:

```text
Base
```

is the common ancestor,

```text
Current
```

is the commit currently checked out,

and:

```text
Target
```

is the commit being merged.

Each state can be normalized into a snapshot:

```text
Base Commit
     │
     ▼
Base Snapshot


Current Commit
     │
     ▼
Current Snapshot


Target Commit
     │
     ▼
Target Snapshot
```

The merge then compares the changes relative to the common base.

---

# Why Three-Way Merge?

A simple two-way comparison is insufficient.

Suppose:

```text
Base:

hello
world
```

Current branch changes it to:

```text
hello
Mini Git
```

while the target branch changes it to:

```text
hello
Git
```

The merge needs to know that both branches changed the same original content differently.

Without the base:

```text
Current ↔ Target
```

it is difficult to determine whether the differences are independent changes or competing modifications.

With the base:

```text
          Base
         /    \
   Current    Target
```

the merge can reason about:

```text
Base → Current
Base → Target
```

---

# Snapshot-Based Merge

Phase 14 introduced the normalized snapshot representation:

```text
path → content
```

Phase 15 reuses this abstraction.

A commit can therefore be transformed into:

```text
Commit
  │
  ▼
Tree
  │
  ▼
Snapshot
```

For example:

```text
main.cpp     → content
README.md    → content
src/main.cpp → content
```

The merge subsystem operates on these normalized snapshots.

This keeps merge logic independent from the internal Tree representation.

---

# Merge Inputs

A merge conceptually receives:

```text
Current Commit
Target Commit
Merge Base
```

These are transformed into:

```text
Current Snapshot
Target Snapshot
Base Snapshot
```

The merge algorithm then evaluates each path.

Conceptually:

```text
Base Snapshot
       │
       ├──────────────┐
       ▼              ▼
Current Snapshot   Target Snapshot
       │              │
       └──────┬───────┘
              ▼
            Merge
              │
       ┌──────┴──────┐
       ▼             ▼
     Result        Conflict
```

---

# Merge Cases

Phase 15 distinguishes several important merge situations.

---

# Fast-Forward Merge

A fast-forward merge occurs when the current branch is an ancestor of the target branch.

For example:

```text
A ── B ── C
     │
     └── main

A ── B ── C
          │
          └── feature
```

The current branch has no unique commits.

Therefore no new merge commit is necessary.

The branch reference can simply move forward:

```text
main: B → C
```

The resulting history is:

```text
A ── B ── C
          │
       main/feature
```

The working tree and index are updated to match the target commit.

Conceptually:

```text
Target Commit
      │
      ▼
Checkout Target Tree
      │
      ▼
Rebuild Index
      │
      ▼
Update Current Branch
```

---

# Already Up-to-Date

If the target branch is already an ancestor of the current branch:

```text
A ── B ── C
     │
     └── feature

A ── B ── C
          │
          └── main
```

there is nothing to merge.

The current branch already contains the target history.

Therefore:

```text
No new commit
No tree changes
No branch movement
```

The operation reports that the repository is already up to date.

---

# Non-Conflicting Merge

Suppose the branches diverged:

```text
        C ── D
       /
A ── B
       \
        E ── F
```

Assume:

```text
Current branch:
changes main.cpp

Target branch:
changes README.md
```

Because the branches modified different paths, their changes can be combined.

The resulting tree contains both changes:

```text
main.cpp  → current version
README.md → target version
```

A new merge commit is created.

---

# Merge Commit

A normal merge commit has two parents.

For example:

```text
        C
       / \
      /   \
     B     M
      \   /
       D
```

More conventionally:

```text
        C
       / \
      /   \
     B     M
      \   /
       D
```

The important property is that:

```text
M
├── parent 1 → Current Commit
└── parent 2 → Target Commit
```

The merge commit records that two histories were combined.

Conceptually:

```text
Current Commit
       │
       ├──────────┐
       │          │
       ▼          ▼
     Parent 1   Merge Commit
                  ▲
                  │
               Parent 2
                  ▲
                  │
             Target Commit
```

The resulting commit points to the newly constructed merged Tree.

---

# Merge Commit Structure

A merge commit contains the same basic information as a normal Commit:

```text
tree
author
message
parents
```

A normal commit has:

```text
parent
```

A merge commit has:

```text
parent 1
parent 2
```

This allows history traversal to preserve both branches.

---

# Conflict Detection

A conflict occurs when both branches make incompatible changes to the same logical state.

For example:

```text
Base:

hello
world
```

Current:

```text
hello
Mini Git
```

Target:

```text
hello
Git
```

Both branches modified the same original content differently.

The merge cannot safely choose one automatically.

Therefore:

```text
Conflict
```

is reported.

---

# Conflict Classification

For each path, the merge compares:

```text
Base
Current
Target
```

Conceptually:

```text
                 Base
                   │
          ┌────────┴────────┐
          ▼                 ▼
      Current             Target
          │                 │
          └────────┬────────┘
                   ▼
                Decision
```

Possible outcomes include:

```text
unchanged
current-only change
target-only change
same change
conflict
added
deleted
```

The exact result depends on how the path differs across the three snapshots.

---

# Unchanged Path

If:

```text
Base == Current == Target
```

there is no change.

The result remains:

```text
Base
```

Example:

```text
Base:    hello
Current: hello
Target:  hello
```

Result:

```text
hello
```

---

# Current-Only Change

If:

```text
Base == Target
```

but:

```text
Current != Base
```

only the current branch changed the path.

Therefore the current version can be retained.

Example:

```text
Base:

hello

Current:

Mini Git

Target:

hello
```

Result:

```text
Mini Git
```

---

# Target-Only Change

If:

```text
Base == Current
```

but:

```text
Target != Base
```

only the target branch changed the path.

Therefore the target version can be incorporated.

Example:

```text
Base:

hello

Current:

hello

Target:

Git
```

Result:

```text
Git
```

---

# Same Change

If:

```text
Current == Target
```

and both differ from the base:

```text
Base    = A
Current = B
Target  = B
```

both branches made the same effective change.

There is no conflict.

The merged result is:

```text
B
```

---

# Conflicting Change

A conflict occurs when:

```text
Base != Current
Base != Target
Current != Target
```

and the changes cannot be safely combined.

Example:

```text
Base:

hello

Current:

Mini Git

Target:

Git
```

The merge cannot determine whether the result should be:

```text
Mini Git
```

or:

```text
Git
```

Therefore:

```text
Conflict
```

is reported.

---

# Added Files

A file can also be introduced independently by both branches.

For example:

```text
Base:

(no file)
```

Current:

```text
new.txt → hello
```

Target:

```text
new.txt → hello
```

If both branches added the same content, the result can safely contain:

```text
new.txt → hello
```

However, if they added different contents:

```text
Current:

new.txt → hello

Target:

new.txt → Mini Git
```

the addition conflicts.

---

# Deleted Files

Deletion also participates in three-way merge reasoning.

For example:

```text
Base:

file.txt → hello
```

If Current deletes the file:

```text
Current:

file.txt → missing
```

while Target leaves it unchanged:

```text
Target:

file.txt → hello
```

the deletion can be treated as a current-only change.

If one branch deletes a file while the other modifies it, the situation requires conflict handling.

---

# Merge Result

A successful non-conflicting merge produces a new snapshot:

```text
Base
 │
 ├── Current changes
 │
 └── Target changes
          │
          ▼
     Merged Snapshot
```

The merged snapshot is then converted back into repository objects:

```text
Merged Snapshot
      │
      ▼
TreeBuilder
      │
      ▼
Merged Tree
      │
      ▼
Object Database
      │
      ▼
Merge Commit
```

The merge commit then becomes the new tip of the current branch.

---

# Merge Pipeline

The complete conceptual merge pipeline is:

```text
mini-git merge <branch>
            │
            ▼
      Resolve target
            │
            ▼
      Find target commit
            │
            ▼
      Find common ancestor
            │
            ▼
     Detect fast-forward
            │
            ▼
      Load snapshots
            │
            ▼
       Three-way merge
            │
       ┌────┴────┐
       ▼         ▼
     Clean     Conflict
       │         │
       ▼         ▼
 Build Tree   Report conflict
       │
       ▼
 Create merge commit
       │
       ▼
 Update branch
       │
       ▼
 Update working tree
       │
       ▼
 Rebuild index
```

---

# Repository Responsibilities

The `Repository` subsystem coordinates the merge operation.

It is responsible for operations such as:

```text
resolve branch
resolve HEAD
find commits
update branch
restore trees
rebuild index
```

The merge logic should not directly manipulate branch files or HEAD when merely analyzing a merge.

Instead, the Repository coordinates the state transitions.

---

# Object Database Responsibilities

The Object Database remains responsible for persistent object storage.

Merge may need to read:

```text
Commit
Tree
Blob
```

objects.

After a successful merge, new objects may be written:

```text
Blob objects
Tree objects
Merge Commit
```

Therefore, unlike Diff, Merge is not read-only.

---

# Index Responsibilities

The Index represents the staging state:

```text
path → Blob ID
```

After a successful merge, the Index must correspond to the resulting merged tree.

Conceptually:

```text
Merged Tree
     │
     ▼
Index
```

This keeps:

```text
HEAD
Working Tree
Index
```

consistent after the merge operation.

---

# Working Tree Responsibilities

The Working Tree represents the actual filesystem state.

After a successful merge:

```text
Merged Tree
     │
     ▼
Working Tree
```

The working files should represent the merged result.

The Index is then rebuilt from that resulting tree.

The final state is conceptually:

```text
HEAD
 │
 ▼
Merge Commit
 │
 ▼
Merged Tree
 │
 ├── Working Tree
 │
 └── Index
```

---

# Conflict State

A conflicting merge must not silently create a successful merge commit.

Instead:

```text
Merge
 │
 ▼
Conflict detected
 │
 ▼
No successful merge commit
```

The important invariant is:

```text
Current branch history remains unchanged
```

until the conflict is resolved through a future conflict-resolution mechanism.

Phase 15 focuses on **detecting and reporting conflicts** rather than implementing a complete Git-compatible conflict-resolution workflow.

---

# Merge and Diff

Merge builds directly upon the comparison model introduced by Diff.

Diff answers:

```text
What changed?
```

Merge needs to answer:

```text
What changed from the base on Current?

What changed from the base on Target?

Do those changes conflict?
```

Therefore:

```text
Diff
 │
 ▼
Snapshot Comparison
 │
 ▼
Change Information
 │
 ▼
Merge
```

The snapshot abstraction makes this relationship possible.

---

# Diff vs Merge

Although they are related, they have different responsibilities.

| Subsystem | Purpose             | Mutates Repository |
| --------- | ------------------- | ------------------ |
| Diff      | Analyze differences | No                 |
| Merge     | Combine histories   | Yes                |

Diff:

```text
Snapshot A
     │
     ▼
Compare
     │
     ▼
Output
```

Merge:

```text
Base
 │
 ├── Current
 │
 └── Target
       │
       ▼
 Three-Way Merge
       │
       ▼
 Result / Conflict
```

---

# Merge and Object Graph

Merge operates on the repository's persistent object graph:

```text
Branch
  │
  ▼
Commit
  │
  ▼
Tree
  │
  ▼
Blob
```

Two branches may point to different commits:

```text
main
 │
 ▼
Commit A
 │
 ▼
Tree A


feature
 │
 ▼
Commit B
 │
 ▼
Tree B
```

The merge finds their common history and produces:

```text
Merge Commit
 ├── Parent A
 └── Parent B
```

This creates a new point where the histories converge.

---

# History After Merge

Before merge:

```text
        C
       /
A ── B
       \
        D
```

After a successful merge:

```text
        C
       / \
A ── B   M
       \ /
        D
```

The merge commit `M` preserves both histories.

Therefore merge does not rewrite the existing commits.

Instead:

```text
Existing history
       │
       ▼
Preserved
       │
       ▼
New merge commit
```

---

# Fast-Forward vs Merge Commit

The two successful merge forms can be summarized as:

### Fast-Forward

```text
A ── B ── C
     │
     └── main
```

After:

```text
A ── B ── C
          │
          └── main
```

No new commit is created.

### True Merge

Before:

```text
        C
       /
A ── B
       \
        D
```

After:

```text
        C
       / \
A ── B   M
       \ /
        D
```

A new merge commit is created.

---

# Read and Write Behavior

Unlike Diff, Merge is a state-changing operation.

During analysis, Merge reads:

```text
HEAD
Branches
Commits
Trees
Blobs
Object Database
```

After a successful merge it may modify:

```text
Object Database
Branch Reference
Working Tree
Index
```

It does not modify existing commits.

Existing objects remain immutable.

---

# Immutability

Mini Git's object model remains immutable.

Merge does not modify:

```text
existing Blob
existing Tree
existing Commit
```

Instead, it creates new objects when necessary.

For example:

```text
Existing Trees
      │
      ▼
Merged Tree
      │
      ▼
New Merge Commit
```

This preserves the content-addressable history model.

---

# Merge Invariants

The Merge subsystem should preserve several important invariants.

### Existing objects remain immutable

```text
Blob
Tree
Commit
```

objects already stored in the Object Database are not modified.

### Successful merge produces a valid tree

The resulting Tree must reference valid objects.

### Merge commit has two parents

For a true merge:

```text
parent 1 = current commit
parent 2 = target commit
```

### Fast-forward does not create an unnecessary commit

If the current branch is already an ancestor of the target:

```text
move reference
```

rather than creating a merge commit.

### Conflict does not silently succeed

A conflicting merge must not create a normal successful merge commit.

### Index and Working Tree remain synchronized

After a successful merge:

```text
Merged Tree
    │
    ├── Working Tree
    │
    └── Index
```

---

# Architecture

The Phase 15 architecture can be represented as:

```text
                         Merge
                           │
              ┌────────────┼────────────┐
              │            │            │
              ▼            ▼            ▼
          Current        Target        Base
           Commit        Commit       Commit
              │            │            │
              ▼            ▼            ▼
         Snapshot      Snapshot      Snapshot
              │            │            │
              └────────────┼────────────┘
                           ▼
                    Three-Way Merge
                           │
                 ┌─────────┴─────────┐
                 ▼                   ▼
              Clean                Conflict
                 │                   │
                 ▼                   ▼
          Merged Snapshot        Report
                 │
                 ▼
             TreeBuilder
                 │
                 ▼
             Merged Tree
                 │
                 ▼
           Object Database
                 │
                 ▼
           Merge Commit
             /       \
            /         \
   Current Commit   Target Commit
                 │
                 ▼
          Update Branch
                 │
                 ▼
        Working Tree + Index
```

---

# Relationship Between Major Subsystems

The architecture now connects:

```text
CLI
 │
 ▼
Repository
 │
 ├── References
 │
 ├── HEAD
 │
 ├── Index
 │
 ├── Object Database
 │
 ├── TreeBuilder
 │
 ├── Diff
 │
 └── Merge
```

The Merge subsystem relies on:

```text
Commit
Tree
Blob
ObjectDatabase
Index
TreeBuilder
Repository
Snapshot
```

---

# Phase 14 → Phase 15

Phase 14 introduced:

```text
Repository State
       │
       ▼
    Snapshot
       │
       ▼
   Comparison
       │
       ▼
      Diff
```

Phase 15 extends this model:

```text
Repository History
       │
       ▼
 Common Ancestor
       │
       ├───────────────┐
       ▼               ▼
 Current            Target
 Snapshot           Snapshot
       │               │
       └───────┬───────┘
               ▼
        Three-Way Merge
               │
        ┌──────┴──────┐
        ▼             ▼
      Result        Conflict
        │
        ▼
   Merged Snapshot
        │
        ▼
      Tree
        │
        ▼
   Merge Commit
```

This is the architectural progression from **comparison** to **history integration**.

---

# Current Limitations

Phase 15 intentionally focuses on the core merge architecture.

It does not yet provide:

* full Git-compatible merge behavior
* advanced merge strategies
* rename-aware merging
* copy detection during merge
* recursive merge strategies
* octopus merges
* interactive conflict resolution
* merge continuation
* merge abort
* conflict marker generation
* sophisticated textual conflict presentation
* automatic rename detection
* binary merge strategies

These features are outside the core educational objective of this phase.

---

# Future Improvements

Possible future improvements include:

## Conflict Markers

Generate familiar conflict markers such as:

```text
<<<<<<< HEAD
current version
=======
target version
>>>>>>> feature
```

## Conflict Resolution

Allow users to manually resolve conflicts and continue the merge.

## Merge Abort

Restore the repository to its pre-merge state.

## Merge Continue

Complete a previously interrupted merge after conflicts are resolved.

## Better Common-Ancestor Discovery

Improve ancestry traversal and support more complex histories.

## Rename-Aware Merge

Detect when a file was renamed rather than deleted and recreated.

## Binary Merge Handling

Detect binary conflicts and provide specialized handling.

---

# Design Principle

The most important architectural idea introduced by Phase 15 is:

```text
Multiple Histories
       │
       ▼
 Common Ancestor
       │
       ▼
   Snapshots
       │
       ▼
Three-Way Comparison
       │
       ▼
 Merge / Conflict
```

Merge does not simply combine two final directories.

It reasons about:

```text
what existed before
what Current changed
what Target changed
```

This allows Mini Git to distinguish:

```text
independent changes
same changes
conflicting changes
```

and provides a principled foundation for future conflict-resolution features.

---

# Summary

Phase 15 adds the ability to answer:

```text
How can two divergent histories be combined?
```

The process is:

```text
Current Branch
       │
       ▼
Current Commit
       │
       ▼
Current Snapshot
       │
       │
       ├──────────────┐
       │              │
       ▼              ▼
      Base          Target
       │              │
       ▼              ▼
Base Snapshot    Target Snapshot
       │              │
       └──────┬───────┘
              ▼
       Three-Way Merge
              │
       ┌──────┴──────┐
       ▼             ▼
     Clean         Conflict
       │
       ▼
Merged Snapshot
       │
       ▼
    TreeBuilder
       │
       ▼
   Merged Tree
       │
       ▼
  Merge Commit
       │
       ▼
 Update Branch
       │
       ▼
Working Tree + Index
```

Phase 15 therefore extends Mini Git from a system that can **observe differences** into a system that can **integrate divergent histories**.

The architectural progression is now:

```text
Phase 13
Repository History
       │
       ▼
Phase 14
Snapshot + Diff
       │
       ▼
Phase 15
Three-Way Merge
       │
       ▼
Conflict Detection
       │
       ▼
History Integration
```

The key relationship is:

```text
Diff tells Mini Git what changed.

Merge uses those changes to determine
how two histories can be combined.
```

Phase 15 therefore establishes the foundation for future **conflict resolution, merge workflows, repository inspection, and deeper Git internals**.
