# Mini Git Diff

## Overview

The Diff subsystem compares two repository states and reports the content differences between them.

Phase 14 introduces four comparison modes:

```text
Working Tree ↔ Index

Index ↔ HEAD

Commit ↔ Working Tree

Commit ↔ Commit
```

The Diff subsystem is read-only.

Running a diff does not:

* create objects
* modify the Index
* modify HEAD
* modify branches
* create commits

---

# Why Diff?

A version control system needs to answer more than:

> What files exist?

It also needs to answer:

> What changed?

For example:

```text
Old:

hello
world
```

New:

```text
hello
Mini Git
```

The meaningful change is:

```text
-world
+Mini Git
```

Diff provides this comparison.

---

# State Model

Mini Git has several important states:

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
```

Diff allows these states to be compared.

---

# Supported Commands

## Working Tree vs Index

```bash
mini-git diff
```

Meaning:

```text
Index ↔ Working Tree
```

This shows unstaged changes.

---

## Index vs HEAD

```bash
mini-git diff --cached
```

Meaning:

```text
HEAD ↔ Index
```

This shows staged changes.

---

## Commit vs Working Tree

```bash
mini-git diff <commit>
```

Meaning:

```text
Commit ↔ Working Tree
```

This shows how the current filesystem differs from a particular commit.

---

## Commit vs Commit

```bash
mini-git diff <commit1> <commit2>
```

Meaning:

```text
Commit 1 ↔ Commit 2
```

This compares two immutable repository snapshots.

---

# Snapshot Representation

Different repository states are converted to the same representation:

```text
path → content
```

For example:

```text
main.cpp → "#include <iostream>\nint main() {}\n"

README.md → "# Mini Git\n"
```

This makes the comparison independent of how the state is stored.

---

# Working Tree Snapshot

The Working Tree is read directly from the filesystem.

The `.mini-git` directory is ignored because it contains repository metadata rather than user files.

Conceptually:

```text
project/
├── main.cpp
├── README.md
├── src/
│   └── parser.cpp
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

The Index stores:

```text
path → Blob ID
```

The Diff subsystem resolves those Blob IDs through the Object Database.

The process is:

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
Serialized Blob
 │
 ▼
Blob
 │
 ▼
Content
```

The final snapshot is:

```text
path → content
```

---

# Commit Snapshot

A Commit points to a Tree.

The Tree is recursively traversed:

```text
Commit
 │
 ▼
Tree
 ├── Blob
 ├── Blob
 └── Tree
      ├── Blob
      └── Blob
```

Each Blob is loaded from the Object Database.

The resulting snapshot is:

```text
path → content
```

---

# Comparing Snapshots

Once two snapshots exist:

```text
Old Snapshot

path → content
```

and:

```text
New Snapshot

path → content
```

the Diff subsystem creates the union of their paths.

For each path it determines whether the file is:

```text
unchanged
modified
added
deleted
```

---

# Unchanged

If:

```text
old exists
new exists
old content == new content
```

there is no output.

Example:

```text
old:
hello

new:
hello
```

Result:

```text
no diff
```

---

# Modified

If a path exists in both snapshots but its contents differ:

```text
old:
hello
world

new:
hello
Mini Git
```

the result is:

```text
-world
+Mini Git
```

---

# Added

If a path exists only in the new snapshot:

```text
old:
missing

new:
hello.txt
```

all new lines are additions.

Example:

```text
+hello
+world
```

---

# Deleted

If a path exists only in the old snapshot:

```text
old:
hello.txt

new:
missing
```

all old lines are removals.

Example:

```text
-hello
-world
```

---

# Line-Based Comparison

Phase 14 compares files line by line.

Given:

```text
Old:

A
B
C
```

and:

```text
New:

A
X
C
```

the common lines are:

```text
A
C
```

The resulting changes are:

```text
-B
+X
```

---

# Longest Common Subsequence

The implementation uses the Longest Common Subsequence concept.

The LCS identifies the largest sequence of lines that appears in both versions while preserving order.

For:

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

the LCS is:

```text
A
C
D
```

The line:

```text
B
```

was removed and:

```text
X
```

was added.

---

# LCS Table

The algorithm builds a dynamic-programming table.

Conceptually:

```text
LCS[i][j]
```

represents the longest common subsequence for portions of the two line sequences.

If two lines match:

```text
old[i] == new[j]
```

the table extends the subsequence.

Otherwise the best neighboring value is selected.

The table is then used to reconstruct the changes.

---

# Unified Diff

Mini Git formats the result in a familiar structure:

```text
diff -- mini-git hello.txt
--- a/hello.txt
+++ b/hello.txt
@@ -1,2 +1,2 @@
 hello
-world
+Mini Git
```

The symbols mean:

```text
space  unchanged
-      removed
+      added
```

---

# Example

Suppose the committed file contains:

```text
hello
world
```

and the Working Tree contains:

```text
hello
Mini Git
```

Running:

```bash
mini-git diff <commit>
```

produces a diff conceptually like:

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

# Staged Diff Example

Suppose:

```text
HEAD:

hello
world
```

The user changes the file:

```text
hello
Mini Git
```

and stages it.

Then:

```bash
mini-git diff
```

compares:

```text
Index ↔ Working Tree
```

Since they are identical:

```text
no output
```

But:

```bash
mini-git diff --cached
```

compares:

```text
HEAD ↔ Index
```

and shows:

```text
-world
+Mini Git
```

This demonstrates the difference between **unstaged** and **staged** changes.

---

# Commit-to-Commit Example

Suppose:

```text
Commit A:

hello
world
```

and:

```text
Commit B:

hello
Mini Git
```

Then:

```bash
mini-git diff <commit-A> <commit-B>
```

produces:

```text
-world
+Mini Git
```

The commits themselves remain unchanged.

---

# Read-Only Design

Diff is an analysis operation.

It should not change:

```text
HEAD
Branches
Index
Objects
Commits
Working Tree
```

The operation is conceptually:

```text
State A
   │
   ▼
Snapshot A

State B
   │
   ▼
Snapshot B

Snapshot A
     │
     ▼
   Compare
     │
     ▼
   Output
```

---

# Architecture

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
                 Compare Paths
                        │
                        ▼
                 Compare Contents
                        │
                        ▼
                       LCS
                        │
                        ▼
                 Unified Output
```

---

# Relationship to Future Merge

Diff is an important foundation for Merge.

Merge will need to understand:

```text
what changed on branch A?
what changed on branch B?
what did they both change?
```

Phase 14 already provides the fundamental concept of comparing snapshots.

Future merge logic can therefore build upon:

```text
Commit → Snapshot
```

and:

```text
Snapshot → Differences
```

---

# Current Limitations

Phase 14 intentionally provides a relatively simple Diff implementation.

It does not yet provide:

* advanced Git-compatible hunk generation
* sophisticated context calculation
* binary diffs
* rename detection
* copy detection
* similarity indexes
* word-level comparison
* colorized output
* patch application
* interactive diff mode

These are not required for the core educational objective of this phase.

---

# Future Improvements

Possible future improvements include:

## Better Hunks

Instead of displaying the entire changed file, generate compact hunks around changes.

## Context Lines

Display several unchanged lines surrounding changes.

## Binary Detection

Detect binary files and provide an appropriate summary instead of treating arbitrary bytes as text.

## Rename Detection

Compare file similarity to detect:

```text
old-name.txt → new-name.txt
```

## Word-Level Diff

Compare changes inside lines.

## Patch Support

Eventually support applying generated patches.

---

# Design Principle

The most important architectural idea introduced by Phase 14 is:

```text
Repository State
       │
       ▼
   Normalized
   Snapshot
       │
       ▼
 State Comparison
```

Instead of writing separate comparison logic for every possible pair of states, Mini Git normalizes them first.

This makes the architecture easier to extend.

---

# Summary

Phase 14 adds the ability to answer:

```text
What changed?
```

The process is:

```text
Working Tree / Index / Commit
             │
             ▼
          Snapshot
             │
             ▼
      Compare snapshots
             │
             ▼
           LCS
             │
             ▼
       Unified Diff
```

This provides the foundation for future:

* Merge
* Conflict Detection
* Three-Way Merge
* Repository Inspection
* Educational Explanations

Phase 14 therefore connects the repository's **persistent history model** with its **state-analysis model**.
