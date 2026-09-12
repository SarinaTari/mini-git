# Mini Git Limitations

## 1. Purpose

Mini Git is an educational Git-inspired version-control system.

It intentionally implements a smaller feature set and a different repository format from Git.

This document records important boundaries so that the project is not mistaken for a Git replacement.

---

## 2. Not Git-Compatible

Mini Git is not intended to be compatible with Git repositories.

It uses:

```text
.mini-git/
```

instead of:

```text
.git/
```

Its object serialization and repository metadata are also Mini Git-specific.

A Mini Git repository should therefore not be expected to be usable by the standard Git executable.

---

## 3. No Git Object-Format Compatibility

Although Mini Git uses concepts such as:

* blobs
* trees
* commits
* object IDs
* references

these are implemented using Mini Git's own serialization and storage design.

The existence of a similarly named concept does not imply binary or format compatibility with Git.

---

## 4. Limited Command Surface

Mini Git implements the commands documented in `COMMANDS.md`.

It does not attempt to reproduce Git's complete command-line interface.

Commands and options that are not implemented should not be assumed to exist.

---

## 5. No Remote Repository Protocol

The project does not implement Git's remote transport ecosystem.

It should therefore not be described as implementing:

* `clone`
* `fetch`
* `pull`
* `push`
* remote tracking
* Git hosting protocols

unless such functionality is explicitly added in a future version.

---

## 6. No Git Hosting Integration

Mini Git itself does not provide GitHub, GitLab, or other hosting integration.

The project repository may be hosted on GitHub as source code, but that is separate from Mini Git's implemented functionality.

---

## 7. Object Format Is Project-Specific

Mini Git's object serialization is designed for this project.

It should not be assumed that another implementation can directly interpret the stored objects.

---

## 8. Merge Semantics Are Limited

Mini Git implements its own merge algorithm.

Its behavior should not be assumed to match every edge case of Git's merge implementation.

In particular, users should treat merge behavior as the behavior defined by the current Mini Git implementation and tests.

---

## 9. Conflict Resolution Is Manual

When a merge produces conflicts, the implementation can write conflict markers and maintain merge state.

The user is responsible for resolving the conflicted files before continuing the merge.

Mini Git does not provide an automatic semantic conflict-resolution system.

---

## 10. No Automatic Repository Repair

Integrity tools can identify repository problems, but they should not be considered a complete automatic repository-repair system.

`fsck`, `doctor`, and related components are diagnostic.

---

## 11. Garbage Collection Is Analysis

The garbage-collection component identifies unreachable objects and estimates reclaimable storage.

The current implementation does not provide a full production-style object deletion and packing system.

Unreachable-object reporting should therefore be understood as analysis rather than complete Git-style garbage collection.

---

## 12. Performance Is Educational

Mini Git includes a benchmark subsystem, but it should not be interpreted as a comprehensive production benchmarking framework.

The implementation prioritizes clarity and correctness over reproducing the performance architecture of Git.

---

## 13. Filesystem Assumptions

Mini Git operates directly on the filesystem.

Behavior can therefore depend on filesystem semantics and platform behavior.

The project has primarily been developed and tested on macOS.

---

## 14. Repository Format Stability

The Mini Git repository format is a project implementation detail.

Future versions could change:

* serialization
* metadata layout
* index representation
* merge state representation
* reference behavior

without preserving compatibility with earlier internal formats unless explicitly stated.

---

## 15. Educational Scope

Mini Git intentionally does not reproduce the full complexity of production Git.

The project focuses on understanding concepts such as:

```text
hashing
objects
storage
indexing
references
history
branching
merging
integrity
reachability
```

This is a feature of the project's scope rather than a defect.

---

## 16. What Mini Git Demonstrates

Despite its limitations, the project demonstrates practical systems-programming concepts including:

* C++20 development
* filesystem programming
* serialization
* SHA-256 hashing
* content-addressed storage
* object graphs
* indexing
* graph traversal
* ancestry algorithms
* three-way merging
* repository integrity analysis
* CMake
* CTest
* modular C++ architecture
* automated testing

---

## 17. Final Scope Statement

Mini Git should be understood as:

> A C++20 educational implementation of core version-control concepts, built around a custom object database, repository model, staging index, references, history graph, merge system, and repository-analysis tools.

It should **not** be understood as:

> A drop-in replacement for Git.

That distinction is fundamental to the project's design.
