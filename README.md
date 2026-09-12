# Mini Git

A **Git-inspired version control system built from scratch in C++20** to understand how modern version control systems work internally.

Mini Git is a serious systems-programming and software-engineering project focused on implementing the fundamental mechanisms behind version control from first principles.

The project progressively implements:

- Content-addressable storage
- SHA-256 object hashing
- Blob objects
- Tree objects
- Commit objects
- Object databases
- Serialization and deserialization
- Index / staging
- Repository state
- `HEAD`
- References
- Branches
- Commit history
- Checkout
- Diff
- Three-way merge
- Merge-base discovery
- Fast-forward merging
- Merge commits
- Conflict detection
- Persistent merge state
- Conflict marker generation
- Conflict resolution
- Merge continuation
- Merge abort
- Tags
- Repository integrity
- Educational repository introspection

Mini Git is **not intended to replace Git** and does not aim for full Git compatibility.

The goal is different:

> **Build a version control system from scratch to understand what is happening underneath Git commands.**

---

# Project Motivation

Git is extremely powerful, but most developers interact with it primarily through commands such as:

```bash
git add
git commit
git branch
git checkout
git merge
git log
git diff
````

Mini Git explores what actually happens behind those commands.

For example, staging a file conceptually becomes:

```text
                 Working Tree

                      │

                      ▼

                  FileReader

                      │

                      ▼

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

                      │

                      ▼

               Object Database

                      │

                      ▼

                    Index
```

A commit then builds on that:

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

     │

     ▼

  Commit

     │

     ▼

Object Database

     │

     ▼

Branch Reference

     │

     ▼

    HEAD
```

Branches introduce multiple lines of development:

```text
                 Commit A

                 /      \

                /        \

             main       feature

              │             │

              ▼             ▼

           Commit B      Commit C
```

Diff introduces state comparison:

```text
Working Tree ───────┐

                    │

Index ──────────────┼──► Snapshot ──► Diff

                    │

Commit ─────────────┘
```

Merge integrates divergent histories:

```text
                 Base Commit

                 /         \

                /           \

        Current Branch    Target Branch

                \           /

                 \         /

                  ▼       ▼

                   Merge

                     │

              ┌──────┴──────┐

              ▼             ▼

           Success        Conflict

              │             │

              ▼             ▼

         Merge Commit   Persistent
                         Merge State
                              │
                              ▼
                       Conflict Markers
                              │
                              ▼
                         User Resolves
                              │
                              ▼
                    mini-git add <file>
                              │
                              ▼
                  mini-git merge --continue
                              │
                              ▼
                   Two-Parent Merge Commit
```

Mini Git is therefore both a practical version-control implementation and a way to study:

* filesystem internals
* data structures
* serialization
* hashing
* persistence
* trees
* graphs
* ancestry
* state management
* C++ architecture
* testing
* systems design
* algorithms
* software engineering

---

# Current Status

## Phase 17 — Tags

Mini Git has completed the repository, object, staging, commit, history, reference, branch, checkout, diff, merge, and persistent merge-state layers.

Phase 17 adds lightweight commit tags using the existing reference architecture. Tags are immutable names that point to specific commit objects.

### Implemented in Phase 17

* Tag reference storage under `.mini-git/refs/tags/`
* Tag creation at `HEAD`
* Tag creation at a specific commit
* Tag listing
* Tag inspection
* Tag deletion
* Duplicate-tag protection
* Invalid-commit protection
* Commit-object validation for tag targets
* Tag-name traversal protection through the existing `Reference` validation
* Automated Tag tests

### Tag Commands

```bash
mini-git tag
mini-git tag <name>
mini-git tag <name> <commit>
mini-git tag --show <name>
mini-git tag --delete <name>
```

Tags are stored as references rather than objects:

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

A tag file contains the commit ID it identifies. Deleting a tag therefore does not delete the commit object from the object database.

## Phase 16 — Advanced Merge State & Conflict Resolution

Mini Git has completed the foundational repository, object, staging, commit, history, reference, branch, checkout, and diff layers.

Phase 15 introduced the core Merge subsystem:

* merge-base discovery
* fast-forward merging
* already-up-to-date detection
* three-way merging
* non-conflicting merging
* conflict detection
* merge commits

Phase 16 extends merging from a single operation into a **persistent multi-step workflow**.

When a merge contains conflicts, Mini Git creates a persistent merge state instead of simply terminating with an error.

The user can then:

1. inspect the repository state
2. inspect the conflicted files
3. edit the conflict markers
4. stage the resolved files
5. continue the merge
6. or abort the merge and restore the pre-merge state

### Implemented

* C++20 project
* CMake build system
* Automated testing with CTest
* Repository initialization
* `.mini-git` repository structure
* SHA-256 hashing
* OpenSSL EVP hashing
* Blob objects
* Blob serialization
* Blob deserialization
* Tree objects
* Tree serialization
* Tree deserialization
* Commit objects
* Commit serialization
* Commit deserialization
* Content-addressable object storage
* Object retrieval
* Index / staging
* Tree construction from the Index
* Working Tree state
* Status infrastructure
* Commit creation
* Commit history
* References
* Symbolic `HEAD`
* Detached `HEAD` representation
* Branch creation
* Branch listing
* Branch reference updates
* Branch switching
* Checkout
* Recursive Tree restoration
* Working Tree restoration
* Index synchronization after checkout
* Diff subsystem
* Working Tree vs Index comparison
* Index vs `HEAD` comparison
* Commit vs Working Tree comparison
* Commit vs Commit comparison
* Added-file detection
* Deleted-file detection
* Modified-file detection
* Unchanged-file filtering
* Line-based diff generation
* Longest Common Subsequence based comparison
* Unified diff output
* Three-way merge infrastructure
* Common ancestor / merge-base discovery
* Fast-forward merge
* Already-up-to-date detection
* Non-conflicting merge
* Conflicting merge detection
* Merge commit creation
* Multiple-parent commits
* Branch reference updates after merge
* Working Tree synchronization after merge
* Index synchronization after merge
* Persistent merge state
* `MERGE_HEAD`
* `MERGE_ORIG_HEAD`
* `MERGE_CONFLICTS`
* `MERGE_MSG`
* Conflict marker generation
* Active merge status
* Conflict resolution through `add`
* Merge continuation
* Merge abort
* Protection against normal commit during an active merge
* Protection against checkout during an active merge
* Automated Merge tests
* Automated Merge State tests
* Tag reference storage
* Tag creation at `HEAD`
* Tag creation at a specific commit
* Tag listing
* Tag inspection
* Tag deletion
* Tag target validation
* Tag-name safety validation
* Automated Tag tests

---

# Current Core Pipeline

The fundamental repository pipeline is:

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

      │

      ▼

   Commit

      │

      ▼

Object Database

      │

      ▼

Branch Reference

      │

      ▼

    HEAD
```

The persistent object relationship is:

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

        ├── Blob

        └── Blob
```

---

# Current Diff Pipeline

The Diff subsystem normalizes repository states into snapshots:

```text
Working Tree

      │

      ▼

   Snapshot


Index ─────────► Snapshot


Commit ────────► Snapshot

      │

      ▼

Compare Snapshots

      │

      ▼

LCS-based Diff

      │

      ▼

Unified Output
```

---

# Current Merge Pipeline

Phase 16 builds upon the Phase 15 snapshot architecture and introduces persistent merge state.

For a clean merge:

```text
Current Branch

      │

      ▼

Current Commit

      │

      ├──────────────┐

      │              │

      ▼              ▼

 Merge Base       Target Commit

      │              │

      ▼              ▼

Base Snapshot    Target Snapshot

      │              │

      └──────┬───────┘

             ▼

      Three-Way Merge

             │

             ▼

       Merged Snapshot

             │

             ▼

           Tree

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

For a conflicting merge:

```text
Current Branch

      │

      ▼

Current Commit

      │

      ├──────────────┐

      │              │

      ▼              ▼

 Merge Base       Target Commit

      │              │

      ▼              ▼

Base Snapshot    Target Snapshot

      │              │

      └──────┬───────┘

             ▼

      Three-Way Merge

             │

             ▼

          Conflict

             │

             ├──► Write conflict markers
             │
             ├──► Write MERGE_HEAD
             │
             ├──► Write MERGE_ORIG_HEAD
             │
             ├──► Write MERGE_CONFLICTS
             │
             └──► Write MERGE_MSG

                         │

                         ▼

                  Merge remains active

                         │

                         ▼

                     User edits

                         │

                         ▼

                  mini-git add <file>

                         │

                         ▼

                 Resolve all conflicts

                         │

                         ▼

              mini-git merge --continue

                         │

                         ▼

               Two-parent merge commit

                         │

                         ▼

                   Update branch

                         │

                         ▼

                    Clear merge state
```

---

# Goals

Mini Git is designed around several goals.

## 1. Understand Version Control Internals

The project should explain concepts that are normally hidden behind Git commands.

## 2. Build Everything Incrementally

Each subsystem is implemented independently and then integrated into the larger architecture.

## 3. Use Real Systems Concepts

The implementation uses:

* filesystem APIs
* persistent storage
* hashing
* serialization
* trees
* references
* graph traversal
* ancestry
* state comparison
* merge algorithms
* conflict resolution
* automated testing

## 4. Maintain Clean Architecture

Responsibilities should remain separated.

For example:

```text
Repository

    │

    ├── References

    ├── HEAD

    ├── Working Tree

    ├── Index Coordination

    └── Merge State Coordination


ObjectDatabase

    │

    ├── Blob

    ├── Tree

    └── Commit


Index

    │

    └── Staging State


TreeBuilder

    │

    └── Tree Construction


Diff

    │

    └── State Comparison


Merge

    │

    ├── Ancestry

    ├── Snapshots

    ├── Three-Way Comparison

    ├── Conflict Detection

    └── Merge Workflow
```

## 5. Build Portfolio-Quality Engineering Experience

The project demonstrates:

* C++20
* CMake
* filesystem programming
* cryptographic hashing APIs
* persistence
* data modeling
* testing
* architecture
* Git concepts
* debugging
* documentation
* algorithms
* state management

---

# Core Concepts

## Working Tree

The Working Tree is the actual collection of files on disk.

Example:

```text
project/

├── main.cpp

├── README.md

└── src/

    └── parser.cpp
```

Mini Git does not store the Working Tree itself as a special object.

It reads the files from the filesystem.

---

# Index

The Index represents the staged state.

Conceptually:

```text
path → Blob ID
```

For example:

```text
main.cpp  → a81f...

README.md → 8b23...
```

The Index allows Mini Git to distinguish between:

```text
Working Tree
```

and:

```text
Staged State
```

After operations such as checkout and successful merge, the Index is synchronized with the resulting committed Tree.

During conflict resolution, the Index represents the resolved version after:

```bash
mini-git add <file>
```

---

# Blob

A Blob stores file content.

For example:

```text
hello.txt

hello
world
```

becomes a Blob containing:

```text
hello
world
```

The serialized representation is hashed.

The resulting SHA-256 digest becomes the object's identity.

Conceptually:

```text
content

   │

   ▼

Blob serialization

   │

   ▼

SHA-256

   │

   ▼

Object ID
```

---

# Tree

A Tree represents a directory snapshot.

A Tree contains entries such as:

```text
blob <object-id> hello.txt
tree <object-id> src
```

Trees can therefore reference other Trees.

Example:

```text
Tree

├── hello.txt → Blob

├── main.cpp  → Blob

└── src        → Tree

                ├── parser.cpp → Blob

                └── lexer.cpp  → Blob
```

This produces a hierarchical snapshot.

---

# Commit

A Commit points to a Tree and optionally to one or more parent Commits.

A normal commit conceptually contains:

```text
Commit

 ├── tree

 ├── parent

 ├── author

 └── message
```

A merge commit contains multiple parents:

```text
Merge Commit

 ├── tree

 ├── parent 1

 ├── parent 2

 ├── author

 └── message
```

A history can therefore be traversed through parent relationships:

```text
Commit C

   │

   ▼

Commit B

   │

   ▼

Commit A
```

After a merge, the history becomes a graph:

```text
        Current Commit
          /        \
         /          \
        /            \
Target Commit       Merge Commit
        \            /
         \          /
          \        /
          shared history
```

The important property is that the merge commit preserves both parent histories.

---

# Object Database

The Object Database stores immutable objects.

Conceptually:

```text
.mini-git/

└── objects/

    ├── ab/

    │   └── cdef...

    ├── 91/

    │   └── 2345...

    └── f8/

        └── 1234...
```

The object ID is derived from the object's serialized representation.

This gives Mini Git content-addressable storage.

---

# Content Addressing

Instead of assigning arbitrary IDs:

```text
object1

object2

object3
```

Mini Git derives the ID from content:

```text
serialized object

       │

       ▼

     SHA-256

       │

       ▼

    object ID
```

If two objects have identical serialized contents, they receive the same ID.

---

# Deterministic Trees

Tree serialization must be deterministic.

If the same entries are inserted in different orders:

```text
A

B

C
```

and:

```text
C

A

B
```

the serialized representation should still produce the same logical Tree.

Mini Git therefore sorts Tree entries before serialization.

This ensures:

```text
same contents

      ↓

same serialization

      ↓

same SHA-256

      ↓

same object ID
```

---

# References

References provide human-readable names for commits.

A branch reference can conceptually contain:

```text
refs/heads/main
```

whose content is:

```text
<commit-id>
```

Branches therefore do not duplicate commits.

They are simply names pointing to commit IDs.

---

# HEAD

`HEAD` identifies the current position in the repository.

A symbolic `HEAD` looks like:

```text
ref: refs/heads/main
```

The resolution process becomes:

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

Mini Git also models detached `HEAD`.

In detached mode:

```text
HEAD

 │

 ▼

Commit ID
```

rather than pointing to a branch reference.

---

# Branches

A branch is a named reference to a commit.

For example:

```text
refs/heads/main

refs/heads/feature
```

may point to:

```text
main    → Commit B

feature → Commit C
```

Creating a branch does not copy the history.

It creates another reference.

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

---

# Branch Commands

List branches:

```bash
mini-git branch
```

Create a branch:

```bash
mini-git branch feature
```

The current branch is marked with:

```text
*
```

Example:

```text
* main

  feature
```

---

# Checkout

Checkout changes the current branch and restores its committed Tree into the Working Tree.

Conceptually:

```text
mini-git checkout feature

        │

        ▼

refs/heads/feature

        │

        ▼

    Commit ID

        │

        ▼

      Commit

        │

        ▼

       Tree

        │

        ▼

Restore Working Tree

        │

        ▼

Rebuild Index

        │

        ▼

Update HEAD
```

The current implementation supports:

* branch resolution
* target commit resolution
* recursive Tree restoration
* removal of files absent from the target snapshot
* Working Tree restoration
* Index synchronization
* symbolic `HEAD` switching

---

# Checkout Safety

Checkout is intentionally simpler than Git's complete implementation.

Mini Git does not yet reproduce every checkout safety rule implemented by Git.

Current merge-state handling adds an additional safety rule:

> Checkout is rejected while a merge is in progress.

This prevents the user from abandoning the active merge state by switching branches.

Future improvements may include:

* more sophisticated dirty Working Tree detection
* more advanced overwrite protection
* more complete handling of unusual filesystem states

---

# Diff

The Diff subsystem compares repository states.

Diff answers:

> What changed between two repository states?

The subsystem first converts states into a common representation:

```text
path → content
```

For example:

```text
main.cpp  → "#include <iostream>\n..."

hello.txt → "hello\nworld\n"
```

Once states have the same representation, they can be compared independently of how they were originally stored.

---

# Diff Modes

## Working Tree vs Index

```bash
mini-git diff
```

This compares:

```text
Index

  ↕

Working Tree
```

It answers:

> What has changed in my Working Tree since I staged it?

---

## Index vs HEAD

```bash
mini-git diff --cached
```

This compares:

```text
HEAD

  ↕

Index
```

It answers:

> What changes are staged for the next commit?

---

## Commit vs Working Tree

```bash
mini-git diff <commit>
```

This compares:

```text
Commit

  ↕

Working Tree
```

It answers:

> How does the current Working Tree differ from this commit?

---

## Commit vs Commit

```bash
mini-git diff <commit1> <commit2>
```

This compares:

```text
Commit 1

   ↕

Commit 2
```

It answers:

> What changed between these two snapshots?

---

# Snapshot Representation

The Diff and Merge subsystems normalize repository states into:

```cpp
std::map<std::string, std::string>
```

Conceptually:

```text
"path/to/file" → "file contents"
```

For example:

```text
src/main.cpp → "#include <iostream>\n..."

README.md    → "# Mini Git\n..."
```

This creates a common abstraction for comparison.

---

# Why Snapshots?

Without snapshots, comparison logic would need separate algorithms for:

```text
Working Tree vs Index

Index vs Commit

Commit vs Commit

Commit vs Working Tree
```

Merge would then require even more specialized logic.

Instead, every state is converted into the same representation:

```text
Repository State

       │

       ▼

   Snapshot

       │

       ▼

Compare
```

This simplifies the architecture and allows Diff and Merge to share the same conceptual model.

---

# Diff Algorithm

Mini Git uses a line-based comparison based on the **Longest Common Subsequence (LCS)** concept.

Given:

```text
A

B

C
```

and:

```text
A

X

C
```

the common subsequence is:

```text
A

C
```

Therefore:

```text
-B

+X
```

The algorithm builds an LCS table and reconstructs the changes.

---

# Unified Diff

Mini Git produces output in a familiar diff-like format.

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

The meaning is:

```text
 space = unchanged

 -     = removed

 +     = added
```

---

# Merge

The Merge subsystem combines the histories of two branches.

For example:

```text
        C

       /

A ── B

       \

        D
```

A successful merge can create:

```text
        C

       / \

      /   \

A ── B     M

      \   /

       \ /

        D
```

where `M` is the merge commit.

Phase 16 additionally supports persistent handling of conflicting merges.

---

# Three-Way Merge

Mini Git uses three repository states:

```text
          Base

         /    \

        /      \

   Current    Target
```

Each state is converted into a snapshot:

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

The merge then determines how the changes from the two branches can be combined.

---

# Merge Base

The **merge base** is the common ancestor used as the starting point for three-way comparison.

Conceptually:

```text
        Base

       /    \

      /      \

 Current    Target
```

The merge compares:

```text
Base → Current
```

against:

```text
Base → Target
```

This allows Mini Git to distinguish independent changes from conflicting changes.

---

# Fast-Forward Merge

A fast-forward merge occurs when the current branch is an ancestor of the target branch.

Before:

```text
A ── B ── C

     │

    main
```

and the target points to `C`.

The branch can simply move forward:

```text
A ── B ── C

          │

         main
```

No merge commit is necessary.

The target Tree is restored and the Index is synchronized.

No merge state is created.

---

# Already Up-to-Date

If the target branch is already contained in the current branch history:

```text
A ── B ── C

     │

  target

          │

          current
```

there is nothing to merge.

No new commit is created.

No merge state is created.

No branch history changes.

---

# Non-Conflicting Merge

Suppose:

```text
Base:

main.cpp

README.md
```

The current branch changes:

```text
main.cpp
```

while the target branch changes:

```text
README.md
```

The changes affect different paths.

They can therefore be combined into a merged snapshot:

```text
main.cpp  → Current version

README.md → Target version
```

A new two-parent merge commit is then created.

No persistent conflict state is required.

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

Both branches changed the same original content differently.

Mini Git detects the conflict rather than silently choosing one version.

Instead of creating a merge commit immediately, Phase 16 creates a persistent merge state.

---

# Conflict Classification

For each path, the merge considers:

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

added

deleted

conflict
```

Phase 16 focuses primarily on conflicting file contents.

Deletion and rename conflict handling remains limited compared with Git.

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

The current version can therefore be retained.

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

The target version can therefore be incorporated.

---

# Same Change

If:

```text
Current == Target
```

and both differ from Base:

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

A conflict occurs when both branches changed a path differently:

```text
Base    = A

Current = B

Target  = C
```

where:

```text
B != C
```

Mini Git reports the conflict instead of silently selecting one side.

The conflicting file receives conflict markers.

---

# Conflict Marker Generation

For a conflicting file, Mini Git writes conflict markers into the Working Tree.

The conceptual format is:

```text
<<<<<<< ours
<current content>
=======
<target content>
>>>>>>> feature
```

The markers allow the user to see both competing versions directly inside the file.

The terms represent the two sides of the merge:

```text
ours

    ↓

current branch content
```

and:

```text
theirs

    ↓

target branch content
```

The conflict marker format is an educational implementation and is not intended to provide complete Git-compatible merge behavior.

---

# Persistent Merge State

Phase 16 introduces a major architectural change:

> A merge conflict is now represented as persistent repository state rather than only an in-memory result.

The merge state is stored inside:

```text
.mini-git/
```

The state consists of:

```text
.mini-git/

├── MERGE_HEAD

├── MERGE_ORIG_HEAD

├── MERGE_CONFLICTS

└── MERGE_MSG
```

This means the merge remains active even after the current Mini Git process exits.

The repository can therefore be reopened and the merge workflow can continue later.

---

# MERGE_HEAD

`MERGE_HEAD` stores the commit ID of the target side of the merge.

Conceptually:

```text
MERGE_HEAD

    │

    ▼

Target Commit
```

This commit becomes the second parent of the eventual merge commit.

During an active merge:

```text
current commit → first parent

MERGE_HEAD     → second parent
```

---

# MERGE_ORIG_HEAD

`MERGE_ORIG_HEAD` stores the commit ID that `HEAD` pointed to before the merge began.

Conceptually:

```text
MERGE_ORIG_HEAD

       │

       ▼

Original HEAD
```

This allows Mini Git to restore the repository to its pre-merge state if the user aborts the merge.

It also provides the first parent of the eventual merge commit.

---

# MERGE_CONFLICTS

`MERGE_CONFLICTS` stores the paths that still contain unresolved merge conflicts.

Conceptually:

```text
MERGE_CONFLICTS

    │

    ├── src/main.cpp

    ├── README.md

    └── config.txt
```

Each unresolved path remains in this list until it is resolved and staged.

When a user runs:

```bash
mini-git add src/main.cpp
```

Mini Git stages the resolved file and removes the path from the unresolved conflict list.

---

# MERGE_MSG

`MERGE_MSG` stores the merge message associated with the active merge.

Conceptually:

```text
MERGE_MSG

    │

    ▼

Merge target branch
```

This allows `merge --continue` to create the intended merge commit without losing the merge message between commands or processes.

---

# Active Merge State

An active merge exists when the merge metadata files are present and describe an unfinished merge.

Conceptually:

```text
No merge:

HEAD
 │
 ▼
Branch
 │
 ▼
Commit
```

During a conflicting merge:

```text
HEAD
 │
 ▼
Current Branch
 │
 ▼
Original Commit

      │

      ├── MERGE_ORIG_HEAD
      │
      ├── MERGE_HEAD
      │
      ├── MERGE_CONFLICTS
      │
      └── MERGE_MSG
```

The current branch pointer does not advance while conflicts remain unresolved.

---

# Status During a Merge

`mini-git status` exposes the active merge state.

The repository can therefore distinguish between:

```text
normal repository state
```

and:

```text
merge in progress
```

During an active merge, status can report:

* that a merge is in progress
* the target merge commit
* unresolved conflict paths
* normal Working Tree / Index information

This makes the merge state observable rather than hidden inside the merge implementation.

---

# Conflict Resolution

The conflict-resolution workflow is:

```text
mini-git merge feature

        │

        ▼

      Conflict

        │

        ▼

Conflict markers written

        │

        ▼

User edits file

        │

        ▼

mini-git add <file>

        │

        ▼

Conflict marked resolved
```

For example:

```bash
mini-git merge feature
```

may produce a file containing:

```text
<<<<<<< ours
hello
Mini Git
=======
hello
Git
>>>>>>> feature
```

The user edits the file to the desired final content:

```text
hello
Mini Git
and Git
```

Then stages it:

```bash
mini-git add hello.txt
```

At that point the file is considered resolved.

---

# Merge Continue

Once every conflict has been resolved:

```bash
mini-git merge --continue
```

continues the merge.

The process is:

```text
Active Merge

      │

      ▼

Verify no unresolved conflicts

      │

      ▼

Read Index

      │

      ▼

Build Tree

      │

      ▼

Create Merge Commit

      │

      ├── Parent 1 = MERGE_ORIG_HEAD
      │
      └── Parent 2 = MERGE_HEAD

      │

      ▼

Update Current Branch

      │

      ▼

Synchronize Working Tree + Index

      │

      ▼

Clear Merge State
```

The resulting merge commit has two parents.

---

# Merge Continue Requirements

`mini-git merge --continue` is only valid when:

```text
merge is in progress
```

and:

```text
no unresolved conflicts remain
```

If conflicts remain, the operation is rejected.

This prevents Mini Git from creating a merge commit from an incomplete resolution.

---

# Merge Abort

The user can abandon an active merge with:

```bash
mini-git merge --abort
```

The abort process is:

```text
Active Merge

      │

      ▼

Read MERGE_ORIG_HEAD

      │

      ▼

Restore original commit

      │

      ▼

Restore original Tree

      │

      ▼

Rebuild Index

      │

      ▼

Clear merge state

      │

      ▼

Repository returns to pre-merge state
```

The current branch remains at the original commit.

No merge commit is created.

---

# Merge State Cleanup

After a successful:

```bash
mini-git merge --continue
```

or:

```bash
mini-git merge --abort
```

the merge metadata is removed.

Conceptually:

```text
.mini-git/

├── MERGE_HEAD            ← removed

├── MERGE_ORIG_HEAD      ← removed

├── MERGE_CONFLICTS      ← removed

└── MERGE_MSG            ← removed
```

The repository then returns to a normal non-merge state.

---

# Safety Rules During Active Merge

Phase 16 introduces explicit safety rules.

## Normal Commit

A normal:

```bash
mini-git commit -m "message"
```

is rejected while a merge is active.

The user should instead resolve the conflicts and run:

```bash
mini-git merge --continue
```

This prevents the merge workflow from being bypassed by creating an unrelated normal commit.

---

## Checkout

A branch checkout is rejected while a merge is active.

For example:

```bash
mini-git checkout main
```

is not allowed until the merge is either:

```text
continued
```

or:

```text
aborted
```

This prevents the active merge state from becoming detached from the branch and Working Tree it belongs to.

---

## Continue Without Merge

Running:

```bash
mini-git merge --continue
```

when no merge is active is rejected.

---

## Abort Without Merge

Running:

```bash
mini-git merge --abort
```

when no merge is active is rejected.

---

# Merge and Index

The Index plays a central role in Phase 16.

Before the merge:

```text
Working Tree

      │

      ▼

    Index
```

During conflict resolution:

```text
Working Tree

      │

      ▼

User edits conflict

      │

      ▼

mini-git add <file>

      │

      ▼

    Index
```

The Index therefore becomes the source of the final resolved file contents.

When all conflicts are staged, the Index represents the complete merged state.

`merge --continue` then uses the Index to construct the final Tree.

---

# Merge and Working Tree

The Working Tree has two roles during a conflicting merge.

First, it receives the conflict markers:

```text
Current content

      +

Target content

      ↓

Conflict-marked file
```

Second, it becomes the place where the user resolves the conflict:

```text
Conflict-marked file

        │

        ▼

User edits

        │

        ▼

Resolved file

        │

        ▼

mini-git add
```

The Working Tree therefore acts as the interactive surface of the conflict-resolution workflow.

---

# Merge and Repository

The Repository coordinates the persistent state of the merge.

It provides the relationship between:

```text
HEAD

Branches

Index

Working Tree

Merge State
```

During a conflicting merge, the branch pointer remains unchanged.

The repository instead records:

```text
Original HEAD
Target Commit
Unresolved Paths
Merge Message
```

Once the merge is continued successfully, the branch reference advances to the new merge commit.

---

# Merge and Object Database

Merge analysis reads existing objects from the Object Database:

```text
Commits

Trees

Blobs
```

The merged result creates new objects:

```text
Merged Tree

      │

      ▼

Merge Commit
```

Existing objects remain immutable.

The merge state itself is not stored as an Object Database object. It is repository metadata stored under `.mini-git`.

---

# Merge and Diff

Diff answers:

> What changed?

Merge needs to answer:

> What changed from the base on each branch, and can those changes be combined?

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

The shared snapshot model allows Merge and Diff to reason about repository states using the same underlying representation.

---

# Merge Result

A successful clean merge produces a new snapshot:

```text
Base

 │

 ├── Current changes

 │

 └── Target changes

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

     Object Database

          │

          ▼

      Merge Commit
```

A conflicting merge instead produces:

```text
Conflict

   │

   ├── Conflict markers
   │
   └── Persistent merge state

          │

          ▼

      User resolution

          │

          ▼

       Index update

          │

          ▼

   merge --continue

          │

          ▼

      Merged Tree

          │

          ▼

    Merge Commit
```

---

# Merge Invariants

The Merge subsystem preserves several important invariants.

### Existing objects remain immutable

Stored Blobs, Trees, and Commits are never modified.

### Successful merge produces a valid Tree

The resulting Tree must reference valid objects.

### True merge commit has two parents

```text
parent 1 = original current commit

parent 2 = target merge commit
```

### Fast-forward does not create an unnecessary commit

If the current branch is an ancestor of the target, the branch reference simply moves forward.

### Already-up-to-date does not create a commit

If the target is already contained in the current history, no operation is necessary.

### Clean merge completes immediately

A non-conflicting merge creates its merge commit without requiring a separate continuation step.

### Conflicting merge does not silently succeed

A detected conflict does not result in an incorrect automatic merge commit.

### Conflicting merge creates persistent state

An unresolved merge records:

```text
MERGE_HEAD
MERGE_ORIG_HEAD
MERGE_CONFLICTS
MERGE_MSG
```

### Branch pointer does not advance during unresolved conflicts

The current branch remains at the original commit until the merge is successfully continued.

### `merge --continue` requires all conflicts to be resolved

A merge commit cannot be created while unresolved conflict paths remain.

### `merge --abort` restores the pre-merge state

Aborting restores the original commit, Working Tree, and Index state represented by the pre-merge repository state.

### Working Tree and Index remain synchronized after successful completion

After a successful merge:

```text
Merged Tree

    │

    ├── Working Tree

    │

    └── Index
```

---

# Repository State Model

The repository contains several forms of state:

```text
                         HEAD

                          │

                          ▼

                       Branch

                          │

                          ▼

                        Commit

                          │

                          ▼

                         Tree

                          │

                          ▼

                        Blobs
```

Alongside persistent history:

```text
Working Tree ─────────────┐

                          │

Index ────────────────────┼──► Diff

                          │

Commit ───────────────────┘
```

During an active merge, another state layer appears:

```text
                    Merge State

                         │

        ┌────────────────┼────────────────┐
        │                │                │
        ▼                ▼                ▼
 MERGE_HEAD      MERGE_ORIG_HEAD   MERGE_CONFLICTS
                         │
                         ▼
                     MERGE_MSG
```

The complete repository state therefore combines:

```text
History

+

Working Tree

+

Index

+

HEAD / References

+

Merge State
```

---

# Persistent Merge State Model

The Phase 16 merge state can be represented conceptually as:

```text
                 Current Branch

                       │

                       ▼

                Original Commit
                       │
                       │
              MERGE_ORIG_HEAD
                       │
                       │
                       ▼
                   Merge State
                  /     |      \
                 /      |       \
                ▼       ▼        ▼
        MERGE_HEAD   CONFLICTS   MSG
             │          │         │
             ▼          ▼         ▼
        Target Commit  Paths   Merge Message
```

The important architectural distinction is:

```text
Repository history
```

is immutable object-based state, while:

```text
Merge state
```

is temporary persistent repository metadata.

---

# Phase 16 Data Flow

A complete conflicting merge workflow is:

```text
1. User runs:

   mini-git merge feature


2. Repository resolves:

   Current Commit
   Target Commit
   Merge Base


3. Merge creates:

   Base Snapshot
   Current Snapshot
   Target Snapshot


4. Three-way merge classifies paths.


5. Conflicting paths are detected.


6. Conflict markers are written.


7. Merge state is persisted:

   MERGE_HEAD
   MERGE_ORIG_HEAD
   MERGE_CONFLICTS
   MERGE_MSG


8. Branch reference remains unchanged.


9. User checks:

   mini-git status


10. User edits conflicted files.


11. User stages each resolved file:

    mini-git add <file>


12. Resolved paths are removed from:

    MERGE_CONFLICTS


13. User runs:

    mini-git merge --continue


14. Mini Git verifies:

    no unresolved conflicts


15. Index is converted into a Tree.


16. A merge commit is created:

    parent 1 = original HEAD
    parent 2 = MERGE_HEAD


17. Current branch is updated.


18. Working Tree and Index are synchronized.


19. Merge state is cleared.
```

---

# Merge Abort Data Flow

The abort workflow is:

```text
Active Merge

      │

      ▼

mini-git merge --abort

      │

      ▼

Read MERGE_ORIG_HEAD

      │

      ▼

Restore original commit

      │

      ▼

Restore original Tree

      │

      ▼

Rebuild Index

      │

      ▼

Clear merge metadata

      │

      ▼

Pre-Merge Repository State
```

---

# Relationship Between Major Subsystems

```text
                         CLI

                          │

                          ▼

                      Repository

              ┌───────────┼───────────┐
              │           │           │
              ▼           ▼           ▼
             HEAD      References    Index
              │                       │
              │                       │
              ▼                       ▼
          Commits                 TreeBuilder
              │                       │
              ▼                       ▼
            Trees                    Tree
              │                       │
              ▼                       ▼
            Blobs ◄──────────── Object Database
              │
              │
              ▼
         Working Tree


Repository
     │
     ├── Diff
     │      │
     │      └── Snapshots
     │
     └── Merge
            │
            ├── Snapshots
            ├── Ancestry
            ├── Merge Base
            ├── Conflict Detection
            └── Merge State
                    │
                    ├── MERGE_HEAD
                    ├── MERGE_ORIG_HEAD
                    ├── MERGE_CONFLICTS
                    └── MERGE_MSG
```

---

# Command Reference

| Command                           | Status        | Purpose                                         |
| --------------------------------- | ------------- | ----------------------------------------------- |
| `mini-git --version`              | ✅ Implemented | Show version                                    |
| `mini-git init`                   | ✅ Implemented | Initialize a repository                         |
| `mini-git hash-file <file>`       | ✅ Implemented | Hash file content                               |
| `mini-git hash-object <file>`     | ✅ Implemented | Store a Blob object                             |
| `mini-git add <file>`             | ✅ Implemented | Stage a file / resolve a merge conflict         |
| `mini-git status`                 | ✅ Implemented | Inspect repository state and active merge state |
| `mini-git commit -m "<message>"`  | ✅ Implemented | Create a normal commit                          |
| `mini-git log`                    | ✅ Implemented | Display commit history                          |
| `mini-git branch`                 | ✅ Implemented | List branches                                   |
| `mini-git branch <name>`          | ✅ Implemented | Create a branch                                 |
| `mini-git checkout <branch>`      | ✅ Implemented | Switch branches                                 |
| `mini-git diff`                   | ✅ Implemented | Working Tree vs Index                           |
| `mini-git diff --cached`          | ✅ Implemented | Index vs HEAD                                   |
| `mini-git diff <commit>`          | ✅ Implemented | Commit vs Working Tree                          |
| `mini-git diff <commit> <commit>` | ✅ Implemented | Commit vs Commit                                |
| `mini-git merge <branch>`         | ✅ Implemented | Merge histories                                 |
| `mini-git merge --continue`       | ✅ Implemented | Complete an active merge                        |
| `mini-git merge --abort`          | ✅ Implemented | Abort an active merge                           |
| `mini-git tag`                    | ⏳ Planned     | Create tags                                     |
| `mini-git inspect`                | ⏳ Planned     | Inspect objects                                 |
| `mini-git explain`                | ⏳ Planned     | Explain internal operations                     |
| `mini-git graph`                  | ⏳ Planned     | Visualize history                               |
| `mini-git stats`                  | ⏳ Planned     | Repository statistics                           |
| `mini-git fsck`                   | ⏳ Planned     | Repository integrity                            |

---

# Example Workflow

Initialize a repository:

```bash
mini-git init
```

Create a file:

```bash
printf 'hello\nworld\n' > hello.txt
```

Stage it:

```bash
mini-git add hello.txt
```

Create the first commit:

```bash
mini-git commit -m "Initial commit"
```

Create a feature branch:

```bash
mini-git branch feature
```

Switch to the feature branch:

```bash
mini-git checkout feature
```

Modify the file:

```bash
printf 'hello\nMini Git\n' > hello.txt
```

Inspect the unstaged changes:

```bash
mini-git diff
```

Stage the change:

```bash
mini-git add hello.txt
```

Commit:

```bash
mini-git commit -m "Update hello"
```

Switch back:

```bash
mini-git checkout main
```

Merge the feature:

```bash
mini-git merge feature
```

View the resulting history:

```bash
mini-git log
```

---

# Example Conflict Workflow

Create divergent changes on two branches.

Attempt the merge:

```bash
mini-git merge feature
```

If a conflict occurs, Mini Git creates conflict markers such as:

```text
<<<<<<< ours
hello
Mini Git
=======
hello
Git
>>>>>>> feature
```

Inspect the repository:

```bash
mini-git status
```

Edit the file to the desired final version.

Then stage the resolved file:

```bash
mini-git add hello.txt
```

If multiple conflicts exist, resolve and stage each one:

```bash
mini-git add file1.txt
mini-git add file2.txt
```

Continue the merge:

```bash
mini-git merge --continue
```

Mini Git then creates a two-parent merge commit.

---

# Example Merge Abort

If the user decides not to complete the merge:

```bash
mini-git merge --abort
```

Mini Git restores the repository to the pre-merge state.

The branch remains at the original commit and the temporary merge metadata is removed.

---

# Testing

Mini Git uses CTest and separate test executables for individual subsystems.

Current tests include:

```text
HashTests

ObjectTests

FileReaderTests

BlobTests

TreeBuilderTests

IndexTests

StatusTests

LogTests

CommitTests

ReferenceTests

RepositoryTests

BranchTests

CheckoutTests

DiffTests

MergeTests

MergeStateTests
```

---

# Diff Tests

Phase 14 specifically tests:

* Working Tree vs Index
* Index vs HEAD
* Commit vs Working Tree
* Commit vs Commit
* identical states
* modified files
* added files
* deleted files
* Blob reconstruction
* Tree reconstruction
* snapshot construction
* unified diff generation
* line-based differences
* LCS comparison

---

# Merge Tests

Phase 15 specifically tests:

* fast-forward merge
* already-up-to-date merge
* non-conflicting merge
* conflicting merge
* missing branch handling
* merge commit creation
* branch reference updates
* resulting Working Tree state
* resulting Index synchronization

---

# Merge State Tests

Phase 16 specifically tests:

* conflict marker creation
* persistent merge metadata
* `MERGE_HEAD`
* `MERGE_ORIG_HEAD`
* `MERGE_CONFLICTS`
* `MERGE_MSG`
* active merge detection
* merge state reporting through `status`
* resolving conflicts through `add`
* unresolved conflict rejection
* `merge --continue`
* two-parent merge commit creation
* merge state cleanup after continuation
* `merge --abort`
* restoration of the original repository state
* merge state cleanup after abort
* invalid `merge --continue`
* invalid `merge --abort`
* rejection of normal commit during an active merge
* rejection of checkout during an active merge
* regression of fast-forward behavior
* regression of already-up-to-date behavior
* regression of non-conflicting merge behavior
* regression of conflict detection

---

# Build

From the project root:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

Run the executable:

```bash
./build/mini-git --version
```

---

# Development Environment

The project is developed using:

* C++20
* Apple Silicon macOS
* Apple Clang
* CMake
* OpenSSL 3
* CTest
* Git
* Unix filesystem APIs

The project is designed around Unix-like filesystem behavior.

---

# Design Decisions

## C++20

C++20 provides:

* modern STL
* `std::filesystem`
* stronger language features
* cleaner abstractions
* modern compile-time facilities

---

## CMake

CMake separates:

```text
source code
```

from:

```text
build configuration
```

and makes the project easier to build and test.

---

## OpenSSL EVP

SHA-256 hashing is implemented using OpenSSL's EVP interface.

Conceptually:

```text
Object

  │

  ▼

Serialized bytes

  │

  ▼

EVP SHA-256

  │

  ▼

Digest

  │

  ▼

Hexadecimal Object ID
```

---

## Immutable Objects

Once an object is stored:

```text
object ID → object contents
```

the object is treated as immutable.

A new content state produces a new object ID.

This is one of the central ideas behind content-addressable storage.

---

## Separation of Concerns

Each class has a focused responsibility.

```text
Hash

    hashing


FileReader

    filesystem content reading


Blob

    file content representation


Tree

    directory snapshot


Commit

    history metadata


ObjectDatabase

    persistent object storage


Index

    staging state


TreeBuilder

    construct Trees from staged state


Reference

    named repository references


Repository

    repository-level state and operations


Diff

    state comparison


Merge

    history integration and conflict handling


Merge State

    persistent merge workflow metadata
```

---

# Known Simplifications

Mini Git intentionally does not reproduce every detail of Git.

## Object Format

Mini Git uses its own educational object format.

It is not byte-compatible with Git's object format.

---

## Index

The Index is a custom educational representation.

It is not Git's binary index format.

---

## Checkout

Checkout implements the core concepts of branch switching, Tree restoration, Working Tree synchronization, and Index synchronization.

It does not yet reproduce all of Git's sophisticated checkout safety behavior.

---

## Diff

The current Diff implementation is intentionally simple.

It provides:

* line-based comparison
* basic unified output
* LCS-based change detection

It does not yet implement:

* sophisticated Git-style hunks
* rename detection
* copy detection
* binary diffs
* word-level diffs
* colorized output
* patch application

---

## Merge

The current Merge implementation focuses on the core educational model.

It provides:

* ancestry-based merge decisions
* merge-base discovery
* fast-forward merging
* already-up-to-date detection
* non-conflicting merges
* conflict detection
* conflict marker generation
* persistent merge state
* conflict resolution through staging
* merge continuation
* merge abort
* two-parent merge commits

It does not yet implement:

* complete Git-compatible merge strategies
* rename-aware merging
* binary merge strategies
* octopus merges
* advanced deletion conflict handling
* full Git-compatible conflict resolution semantics

---

# Roadmap

## Phase 0 — Git Concepts

Understand:

* version control
* snapshots
* commits
* branches
* references
* content addressing

**Status: Completed**

---

## Phase 1 — Project Foundation

Build:

* C++20 project
* CMake
* source/include separation
* testing infrastructure

**Status: Completed**

---

## Phase 2 — Repository Initialization

Implement:

* repository detection
* `.mini-git`
* objects directory
* refs directory
* `HEAD`

**Status: Completed**

---

## Phase 3 — Hashing

Implement:

* SHA-256
* hexadecimal object IDs
* OpenSSL EVP

**Status: Completed**

---

## Phase 4 — Object Model

Implement:

* Object abstraction
* Blob
* Tree
* Commit
* serialization

**Status: Completed**

---

## Phase 5 — Blob Objects

Implement:

* file reading
* Blob creation
* Blob serialization
* Blob storage

**Status: Completed**

---

## Phase 6 — Object Database

Implement:

* object storage
* object retrieval
* object existence
* content-addressable persistence

**Status: Completed**

---

## Phase 7 — Trees

Implement:

* Tree entries
* nested Trees
* deterministic serialization
* TreeBuilder

**Status: Completed**

---

## Phase 8 — Index / Staging

Implement:

* Index
* staging
* Blob IDs in the Index
* persistence

**Status: Completed**

---

## Phase 9 — Status

Implement repository state inspection.

**Status: Completed**

---

## Phase 10 — Commits

Implement:

* commit creation
* parent relationships
* commit references
* commit persistence

**Status: Completed**

---

## Phase 11 — Log / History

Implement:

* commit traversal
* parent traversal
* history display

**Status: Completed**

---

## Phase 12 — References & HEAD

Implement:

* references
* symbolic HEAD
* detached HEAD
* HEAD resolution

**Status: Completed**

---

## Phase 13 — Branches & Checkout

Implement:

* branch creation
* branch listing
* branch references
* current branch detection
* checkout
* target commit resolution
* Tree restoration
* Working Tree synchronization
* Index synchronization
* symbolic HEAD switching

**Status: Completed**

---

## Phase 14 — Diff & Change Inspection

Implement:

* state snapshots
* Working Tree comparison
* Index comparison
* commit comparison
* added-file detection
* deleted-file detection
* modified-file detection
* LCS-based comparison
* unified diff output

**Status: Completed**

---

## Phase 15 — Merge & Conflict Detection

Implement:

* ancestor discovery
* merge-base discovery
* fast-forward merge
* already-up-to-date detection
* three-way merge
* branch merging
* non-conflicting merge
* conflict detection
* merge commit creation
* multiple-parent commits
* Working Tree synchronization
* Index synchronization

**Status: Completed**

---

## Phase 16 — Advanced Merge State & Conflict Resolution

Implement:

* persistent merge state
* `MERGE_HEAD`
* `MERGE_ORIG_HEAD`
* `MERGE_CONFLICTS`
* `MERGE_MSG`
* conflict marker generation
* active merge detection
* merge state reporting
* conflict resolution through `add`
* `merge --continue`
* `merge --abort`
* two-parent merge commit creation after resolution
* restoration of pre-merge state
* merge safety rules
* regression testing of previous merge behavior

**Status: Completed**

---

## Phase 17 — Tags

Planned:

* lightweight tags
* annotated-style tags
* tag references
* tag listing

**Status: Future**

---

## Phase 18 — Repository Integrity & Maintenance

Planned:

* `fsck`
* object reachability
* dangling objects
* corruption detection
* repository consistency
* garbage collection concepts

**Status: Future**

---

## Phase 19 — Educational Intelligence

Planned:

```bash
mini-git inspect
mini-git explain
mini-git graph
mini-git stats
```

The goal is to make internal repository behavior inspectable and explainable.

**Status: Future**

---

## Phase 20 — Portfolio Finalization

Planned:

* architectural cleanup
* documentation refinement
* performance improvements
* benchmarking
* stronger testing
* final README
* GitHub presentation
* interview explanations

**Status: Future**

---

# Roadmap at a Glance

```text
 0  Git Concepts                         ✅

 │

 1  Project Foundation                  ✅

 │

 2  Repository Initialization           ✅

 │

 3  Hashing                             ✅

 │

 4  Object Model                        ✅

 │

 5  Blob Objects                        ✅

 │

 6  Object Database                     ✅

 │

 7  Trees                               ✅

 │

 8  Index / Staging                     ✅

 │

 9  Status                              ✅

 │

10  Commits                             ✅

 │

11  Log / History                       ✅

 │

12  References & HEAD                   ✅

 │

13  Branches & Checkout                 ✅

 │

14  Diff & Change Inspection            ✅

 │

15  Merge & Conflict Detection          ✅

 │

16  Advanced Merge State                ✅

 │

17  Tags                                ⏳

 │

18  Repository Integrity & Maintenance ⏳

 │

19  Educational Intelligence            ⏳

 │

20  Portfolio Finalization              ⏳
```

---

# What Makes Mini Git Different?

Mini Git is intentionally designed as an educational systems project rather than a Git clone.

The project focuses on exposing the internal mechanisms.

For example:

```bash
mini-git add file.cpp
```

is not treated as a magical command.

The internal process is:

```text
file.cpp

   │

   ▼

FileReader

   │

   ▼

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

   │

   ▼

Object Database

   │

   ▼

Index
```

Similarly:

```text
mini-git commit
```

becomes:

```text
Index

  │

  ▼

TreeBuilder

  │

  ▼

Tree

  │

  ▼

Commit

  │

  ▼

Object Database

  │

  ▼

Branch Reference

  │

  ▼

HEAD
```

And:

```text
mini-git diff
```

becomes:

```text
Working Tree

      │

      ▼

Snapshot

      │

      │

Index ───────► Snapshot

      │

      ▼

Compare

      │

      ▼

LCS

      │

      ▼

Unified Diff
```

Finally:

```text
mini-git merge feature
```

becomes:

```text
Current Commit

      │

      ▼

Current Snapshot

      │

      │

      ├──────────────┐

      │              │

      ▼              ▼

 Merge Base      Target Commit

      │              │

      ▼              ▼

Base Snapshot   Target Snapshot

      │              │

      └──────┬───────┘

             ▼

      Three-Way Merge

             │

       ┌─────┴─────┐

       ▼           ▼

     Clean       Conflict

       │           │

       │           ├──► Conflict Markers
       │           │
       │           ├──► MERGE_HEAD
       │           │
       │           ├──► MERGE_ORIG_HEAD
       │           │
       │           ├──► MERGE_CONFLICTS
       │           │
       │           └──► MERGE_MSG
       │
       │                 │
       │                 ▼
       │             User Resolves
       │                 │
       │                 ▼
       │          mini-git add <file>
       │                 │
       │                 ▼
       │          merge --continue
       │                 │
       └─────────┬───────┘
                 │
                 ▼
          Merged Snapshot
                 │
                 ▼
               Tree
                 │
                 ▼
          Merge Commit
                 │
                 ▼
          Branch Reference
```

This makes Mini Git useful for understanding both **software architecture** and **version-control internals**.

---

# Engineering Principles

The project follows several engineering principles.

## Single Responsibility

Each class should have a clear purpose.

## RAII

Resources should be managed automatically where appropriate.

## Const Correctness

Read-only operations should be marked `const` where possible.

## Explicit Error Handling

Invalid repository states and invalid commands should produce meaningful errors.

## Deterministic Serialization

Equivalent objects should produce stable serialized representations.

## Immutable Stored Objects

Stored objects should not be modified after creation.

## Separation of Storage and Logic

Object storage should not contain repository command logic.

## Testability

Subsystems should be independently testable.

## Incremental Architecture

Each phase should build naturally on the previous phases.

## Persistent State Where Workflow Requires It

Multi-step operations such as conflict resolution must store enough state to continue safely across separate command invocations.

---

# Technology Stack

| Technology        | Purpose                        |
| ----------------- | ------------------------------ |
| C++20             | Main implementation language   |
| CMake             | Build system                   |
| OpenSSL 3         | SHA-256 hashing                |
| CTest             | Automated testing              |
| `std::filesystem` | Filesystem operations          |
| STL               | Data structures and algorithms |
| Git               | Project version control        |

---

# Project Structure

```text
mini-git/

│

├── CMakeLists.txt

├── README.md

├── LICENSE

├── .gitignore

│

├── include/

│   ├── Object.hpp

│   ├── Hash.hpp

│   ├── FileReader.hpp

│   ├── Blob.hpp

│   ├── Tree.hpp

│   ├── Commit.hpp

│   ├── ObjectDatabase.hpp

│   ├── Repository.hpp

│   ├── Reference.hpp

│   ├── TreeBuilder.hpp

│   ├── Index.hpp

│   ├── Status.hpp

│   ├── Diff.hpp

│   └── Merge.hpp

│

├── src/

│   ├── main.cpp

│   ├── Hash.cpp

│   ├── FileReader.cpp

│   ├── Blob.cpp

│   ├── Tree.cpp

│   ├── Commit.cpp

│   ├── ObjectDatabase.cpp

│   ├── Repository.cpp

│   ├── Reference.cpp

│   ├── TreeBuilder.cpp

│   ├── Index.cpp

│   ├── Status.cpp

│   ├── Diff.cpp

│   └── Merge.cpp

│

├── tests/

│   ├── HashTests.cpp

│   ├── ObjectTests.cpp

│   ├── FileReaderTests.cpp

│   ├── BlobTests.cpp

│   ├── TreeBuilderTests.cpp

│   ├── IndexTests.cpp

│   ├── StatusTests.cpp

│   ├── LogTests.cpp

│   ├── CommitTests.cpp

│   ├── ReferenceTests.cpp

│   ├── RepositoryTests.cpp

│   ├── BranchTests.cpp

│   ├── CheckoutTests.cpp

│   ├── DiffTests.cpp

│   ├── MergeTests.cpp

│   └── MergeStateTests.cpp

│

└── docs/

    ├── architecture.md

    ├── diff.md

    └── merge.md
```

---

# Repository Structure

A typical initialized Mini Git repository contains:

```text
project/

├── .mini-git/

│   ├── HEAD

│   ├── index

│   ├── objects/

│   │   └── ...

│   │

│   └── refs/

│       └── heads/

│           ├── main

│           └── feature

│

├── source files...

└── README.md
```

During an unresolved merge, additional metadata is present:

```text
.mini-git/

├── HEAD

├── index

├── MERGE_HEAD

├── MERGE_ORIG_HEAD

├── MERGE_CONFLICTS

├── MERGE_MSG

├── objects/

│   └── ...

└── refs/

    └── heads/
```

After the merge is continued or aborted, the temporary merge metadata is removed.

---

# Limitations

Mini Git is not Git.

It currently does not provide:

* Git repository compatibility
* Git's exact object format
* Git's binary Index format
* complete Git-compatible checkout semantics
* complete Git-compatible conflict resolution
* rename-aware merging
* advanced merge strategies
* binary merge strategies
* octopus merges
* advanced deletion conflict handling
* tags
* remotes
* networking
* push/pull
* GitHub integration
* complete repository maintenance
* complete Git-compatible diff behavior

These features are either intentionally simplified or planned for future phases.

The project's first priority is understanding **local version-control internals**.

---

# Learning Outcomes

By completing Mini Git, the project should provide practical understanding of:

## C++

* C++20
* classes
* interfaces
* RAII
* const correctness
* STL containers
* filesystem APIs
* exception handling
* modular design

## Systems Programming

* filesystem representation
* persistent storage
* file I/O
* object databases
* serialization
* repository state
* references
* immutable objects
* persistent workflow state

## Algorithms & Data Structures

* hashing
* trees
* recursion
* graph traversal
* ancestry
* state comparison
* LCS
* three-way merge
* conflict detection

## Software Engineering

* CMake
* testing
* Git workflows
* architecture
* documentation
* incremental development
* debugging
* refactoring
* state-machine style workflow design

## Version Control

* Working Tree
* staging
* Index
* snapshots
* objects
* Blobs
* Trees
* commits
* references
* HEAD
* branches
* history
* checkout
* diff
* merge
* merge bases
* three-way merging
* conflicts
* conflict markers
* conflict resolution
* merge continuation
* merge abort
* tags
* integrity

---

# Final Architecture Goal

When the project is complete, the architecture should look approximately like this:

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

                                 │

                   ┌─────────────┼─────────────┐

                   │             │             │

                   ▼             ▼             ▼

                Branch         Merge          Tags

                                 │

                       ┌─────────┴─────────┐

                       │                   │

                       ▼                   ▼

                   Snapshots          Merge State

                       │                   │

                       ▼             ┌─────┼─────┐

                Three-Way Merge       │     │     │

                       │              ▼     ▼     ▼

                       ▼           HEAD  ORIG  CONFLICTS

                  Conflict              │
                       │                ▼
                       ▼              MSG
                Conflict Markers
                       │
                       ▼
                  User Resolution
                       │
                       ▼
                    Index
                       │
                       ▼
                Merge -- Continue
                       │
                       ▼
                 Merged Tree
                       │
                       ▼
                 Merge Commit
                       │
                       ▼
                Repository State
                       │
                       ▼
                Integrity / FSCK
                       │
                       ▼
               Educational Layer
                       │
              ┌────────┼────────┐
              ▼        ▼        ▼
           Inspect  Explain   Graph
              │        │        │
              └────────┼────────┘
                       ▼
                     Stats
```

---

# Final Vision

The final Mini Git should not simply imitate Git's command names.

It should provide a clear, inspectable implementation of the fundamental ideas behind version control.

The intended progression is:

```text
create repository

      │

      ▼

stage files

      │

      ▼

create objects

      │

      ▼

build Trees

      │

      ▼

create commits

      │

      ▼

traverse history

      │

      ▼

create branches

      │

      ▼

switch branches

      │

      ▼

compare states

      │

      ▼

find common ancestors

      │

      ▼

merge histories

      │

      ▼

detect conflicts

      │

      ▼

persist merge state

      │

      ▼

resolve conflicts

      │

      ▼

continue or abort merge

      │

      ▼

inspect repository

      │

      ▼

analyze repository

      │

      ▼

verify repository integrity
```

The deeper goal is to make the system understandable enough that a developer can ask:

> **"What exactly happened internally?"**

and trace the answer through the implementation.

That is the central idea of Mini Git.

---

# Current Status

| Item              | Status                                                             |
| ----------------- | ------------------------------------------------------------------ |
| Current Phase     | **17 / 20**                                                        |
| Current Subsystem | **Tags**                                                           |
| Previous Phase    | Advanced Merge State & Conflict Resolution                         |
| Next Phase        | Repository Integrity & Maintenance                                 |
| Language          | C++20                                                              |
| Build System      | CMake                                                              |
| Hashing           | SHA-256 / OpenSSL EVP                                              |
| Storage           | Content-addressable Object Database                                |
| Testing           | CTest                                                              |
| Project Type      | Systems Programming / Version Control / Educational Infrastructure |

---

# License

See `LICENSE` for the project's license and usage terms.

```