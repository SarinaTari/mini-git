# Mini Git Command Reference

## 1. Overview

Mini Git provides a command-line interface for interacting with a Mini Git repository.

The executable is:

```bash
./build/mini-git
```

The commands documented here correspond to the implemented command categories.

Mini Git uses its own repository format and is not intended to be Git-compatible.

---

# Repository

## `init`

Initializes a Mini Git repository in the current directory.

```bash
./build/mini-git init
```

The repository metadata is stored under:

```text
.mini-git/
```

---

## `--version`

Displays the Mini Git version.

```bash
./build/mini-git --version
```

---

# Object and Hash Commands

## `hash-file`

Hashes a file using the project's SHA-256 implementation.

```bash
./build/mini-git hash-file <file>
```

Example:

```bash
./build/mini-git hash-file main.cpp
```

---

## `hash-object`

Operates on object data through the implemented object-hashing command.

```bash
./build/mini-git hash-object <file>
```

---

# Staging

## `add`

Stages a file.

```bash
./build/mini-git add <file>
```

The staged entry is represented in the repository index.

---

## `add .`

Stages repository files using the implemented recursive add behavior.

```bash
./build/mini-git add .
```

Repository metadata under `.mini-git` is excluded from normal project-file processing.

---

# Status

## `status`

Displays the current repository status.

```bash
./build/mini-git status
```

The status implementation can report:

* modified files
* deleted files
* untracked files
* merge state
* merge conflicts

---

# Commits

## `commit`

Creates a commit from the current staged state.

```bash
./build/mini-git commit -m "Commit message"
```

Example:

```bash
./build/mini-git commit -m "Add object database"
```

A commit contains:

* a tree ID
* parent commit IDs
* author
* message

---

## `log`

Displays repository commit history.

```bash
./build/mini-git log
```

History is reconstructed from stored commit parent relationships.

---

# Branches

## `branch`

Lists branches.

```bash
./build/mini-git branch
```

---

## `branch <name>`

Creates a branch.

```bash
./build/mini-git branch feature
```

The branch reference is stored under:

```text
.mini-git/refs/heads/
```

---

# Checkout

## `checkout <branch>`

Checks out a branch.

```bash
./build/mini-git checkout feature
```

The checkout operation updates repository state and the working tree according to the implementation's checkout rules.

---

# Diff

## `diff`

Compares the working tree with the index.

```bash
./build/mini-git diff
```

---

## `diff --cached`

Compares the index with `HEAD`.

```bash
./build/mini-git diff --cached
```

---

## `diff <commit>`

Compares a commit with the working tree.

```bash
./build/mini-git diff <commit>
```

---

## `diff <commit> <commit>`

Compares two commits.

```bash
./build/mini-git diff <first-commit> <second-commit>
```

---

# Merge

## `merge <branch>`

Merges the specified branch into the current branch.

```bash
./build/mini-git merge feature
```

The implementation can produce:

* an already-up-to-date result
* a fast-forward result
* a merge commit
* a merge conflict

---

## `merge --continue`

Continues an in-progress merge after conflicts have been resolved.

```bash
./build/mini-git merge --continue
```

---

## `merge --abort`

Aborts an in-progress merge.

```bash
./build/mini-git merge --abort
```

---

# Tags

## `tag`

Lists tags.

```bash
./build/mini-git tag
```

---

## `tag <name>`

Creates a tag using the implemented tag behavior.

```bash
./build/mini-git tag v1.0.0
```

Tags are stored under:

```text
.mini-git/refs/tags/
```

---

# Inspection and Education

## `inspect <object-id>`

Inspects a stored object.

```bash
./build/mini-git inspect <object-id>
```

Supported object categories include:

```text
blob
tree
commit
```

---

## `explain <command>`

Provides an explanation for a supported command.

```bash
./build/mini-git explain add
```

This command is intended as an educational feature.

---

# Repository Analysis

## `graph`

Displays commit graph information.

```bash
./build/mini-git graph
```

---

## `stats`

Displays repository statistics.

```bash
./build/mini-git stats
```

---

## `analyze`

Runs repository analysis.

```bash
./build/mini-git analyze
```

---

## `impact <commit>`

Analyzes the repository impact associated with a commit.

```bash
./build/mini-git impact <commit>
```

---

# Repository Health

## `doctor`

Runs the repository health analysis.

```bash
./build/mini-git doctor
```

The Doctor checks repository structure, references, referenced objects, and reachability information implemented by the Doctor subsystem.

---

## `fsck`

Runs repository integrity checking.

```bash
./build/mini-git fsck
```

The integrity subsystem reports information about object validity, missing/corrupted data, invalid relationships, and unreachable objects where applicable.

---

# Storage

## `storage`

Analyzes repository storage.

```bash
./build/mini-git storage
```

---

# Benchmarking

## `benchmark`

Runs the implemented repository benchmark functionality.

```bash
./build/mini-git benchmark
```

---

# Interactive Exit Commands

The CLI also recognizes:

```text
q
quit
exit
```

where applicable to the command interface.

---

# Typical Workflow

Initialize:

```bash
./build/mini-git init
```

Stage:

```bash
./build/mini-git add main.cpp
```

Inspect:

```bash
./build/mini-git status
```

Commit:

```bash
./build/mini-git commit -m "Initial commit"
```

View history:

```bash
./build/mini-git log
```

Create a branch:

```bash
./build/mini-git branch feature
```

Switch branches:

```bash
./build/mini-git checkout feature
```

---

# Diagnostic Workflow

Useful diagnostic commands include:

```bash
./build/mini-git status
./build/mini-git log
./build/mini-git graph
./build/mini-git stats
./build/mini-git doctor
./build/mini-git storage
./build/mini-git fsck
```

These commands expose different views of the same repository state.
