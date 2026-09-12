# Mini Git Branching and Merging

## 1. Overview

Branches and merges are implemented using references and commit relationships.

A branch identifies a commit through a reference.

A merge combines repository states using commit ancestry and a merge base.

---

## 2. Branches

A branch is represented by a reference under:

```text
.mini-git/refs/heads/
```

The reference contains the commit ID associated with that branch.

Conceptually:

```text
main
 │
 ▼
commit C
```

---

## 3. Creating a Branch

Creating a branch creates a new branch reference based on the current repository state.

Conceptually:

```text
        A
       / \
    main  feature
```

Both references initially identify the same commit.

---

## 4. Branch Movement

When a new commit is created, the current branch reference is updated.

For example:

```text
Before:

main → A

After commit:

main → B
        │
        └── parent → A
```

The existing commit remains part of history.

---

## 5. Checkout

Checkout changes the current branch/reference state and restores the target commit's tree into the working tree.

The repository implementation performs safety checks before changing the working tree.

The index is also synchronized with the checked-out tree.

---

## 6. Commit History

Commit relationships form the history graph.

A linear history:

```text
A → B → C
```

is represented through parent IDs.

A branch is therefore a reference to a point in that graph rather than a separate copy of the history.

---

## 7. Merge

A merge combines the current branch with another branch.

The implementation determines:

* current commit
* target commit
* merge base

and then compares their snapshots.

---

## 8. Merge Base

The merge base is a common ancestor used as the baseline for a three-way merge.

Conceptually:

```text
        Base
       /    \
      A      B
       \    /
        Merge
```

The `Ancestry` subsystem provides merge-base and ancestry functionality.

---

## 9. Three-Way Comparison

For each path, the merge compares:

```text
Base
Current
Target
```

A simplified decision model is:

```text
Current == Target
    → same result

Current == Base
    → target version

Target == Base
    → current version

Current != Base
and Target != Base
and Current != Target
    → conflict
```

The implementation also handles paths that exist on only some sides.

---

## 10. Fast-Forward

If the target commit is ahead of the current commit without requiring a divergent merge, the implementation can perform a fast-forward operation.

This does not require creating a separate merge commit.

---

## 11. Already Up to Date

If the target commit is already represented in the current history according to the implemented ancestry rules, the merge does not need to create another history state.

---

## 12. Merge Conflicts

When both sides independently modify a path in incompatible ways, the merge implementation can mark that path as a conflict.

Conflict files use merge markers.

The format includes markers corresponding to:

```text
<<<<<<< ours
current content
=======
target content
>>>>>>> branch
```

The exact branch name used in the closing marker comes from the merge operation.

---

## 13. Merge State

When conflicts prevent immediate completion, Mini Git stores merge state.

The repository records information including:

* original HEAD
* merge target
* merge message
* conflict paths

This allows the repository to recognize that a merge is currently in progress.

---

## 14. `merge --continue`

After conflicts have been resolved, the merge can be continued.

```bash
./build/mini-git merge --continue
```

The implementation checks the merge state and repository/index conditions before creating the resulting merge commit.

---

## 15. `merge --abort`

An unfinished merge can be aborted:

```bash
./build/mini-git merge --abort
```

The implementation restores the state associated with the original merge HEAD and clears merge state.

---

## 16. Merge Safety

The merge implementation checks repository state before beginning the operation.

In particular, it requires an appropriate clean working-tree/index state before proceeding.

This prevents an ordinary merge from silently overwriting unrelated local modifications.

---

## 17. Merge Commit

A successful non-fast-forward merge can produce a commit with multiple parents.

Conceptually:

```text
      B
     / \
    A   D
     \ /
      C
```

where `C` records both relevant parent commits.

---

## 18. Relationship Between Branches and Commits

Branches do not contain copies of commits.

Instead:

```text
branch
  │
  ▼
commit
  │
  ├── parent
  ├── tree
  └── ...
```

Multiple branches can therefore point into the same commit graph.

---

## 19. Merge and the Index

The merge implementation updates the working tree and synchronizes the index with the resulting state where appropriate.

During a conflict, the index and working tree can represent an incomplete merge state until the conflict is resolved.

---

## 20. Merge Design Summary

The implementation combines:

```text
References
    │
    ▼
Commit graph
    │
    ▼
Ancestry
    │
    ▼
Merge base
    │
    ▼
Three-way comparison
    │
    ├── clean result
    └── conflict
          │
          ▼
     merge state
          │
      ┌───┴────┐
      ▼        ▼
 continue    abort
```

This is the central branching and merging model used by Mini Git.
