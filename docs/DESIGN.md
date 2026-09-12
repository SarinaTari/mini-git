# Mini Git Design

## 1. Design Philosophy

Mini Git was designed as a learning-oriented systems project rather than as a replacement for Git.

The implementation focuses on making important version-control concepts explicit in C++:

* object storage
* hashing
* serialization
* staging
* snapshots
* references
* commit history
* branching
* merging
* integrity
* reachability

The primary priorities are:

1. Understandability
2. Correctness
3. Testability
4. Separation of responsibilities
5. Explicit repository state

---

## 2. Why Content-Addressed Objects?

Mini Git identifies stored objects using SHA-256-derived IDs.

This gives the implementation a simple relationship:

```text
serialized data
      │
      ▼
   SHA-256
      │
      ▼
 object ID
```

The object ID can then be used by other objects as a reference.

For example, a commit stores a tree ID and parent commit IDs.

---

## 3. Why Separate Blob, Tree, and Commit?

The three object types represent different concepts.

### Blob

File content.

### Tree

Directory structure.

### Commit

A snapshot plus history metadata.

Separating them makes repository history representable as an object graph.

---

## 4. Why Use Serialization?

Objects need a representation that can be written to disk and reconstructed later.

Each object therefore provides serialization behavior.

The conceptual flow is:

```text
C++ object
    │
    ▼
serialize
    │
    ▼
stored representation
```

and later:

```text
stored representation
    │
    ▼
deserialize
    │
    ▼
C++ object
```

---

## 5. Why Have an Object Database?

The object database isolates persistence from higher-level repository logic.

It provides operations for:

* storing objects
* checking object existence
* reading objects
* enumerating stored object IDs

Commands such as commit, inspection, integrity checking, and analysis can therefore reuse the same storage abstraction.

---

## 6. Why Have an Index?

The index represents staged state.

This provides a distinction between:

```text
working tree
     │
     ▼
 staged state
     │
     ▼
 committed state
```

The index allows the commit operation to use staged entries rather than directly treating every working-tree file as part of the commit.

---

## 7. Why Use References?

Branches and tags need a way to identify commits without modifying commits themselves.

Mini Git represents them through reference files.

For example:

```text
refs/heads/main
        │
        ▼
    commit ID
```

Moving a branch therefore means updating its reference.

---

## 8. Why Use HEAD?

`HEAD` provides a single point from which the current repository state can be determined.

In a branch-based state:

```text
HEAD
 │
 ▼
branch reference
 │
 ▼
commit
```

The repository implementation also supports checking for detached HEAD state.

---

## 9. Why Build Trees From the Index?

The index represents staged paths and object IDs.

`TreeBuilder` converts that information into tree objects.

This separates:

* staging
* tree construction
* commit construction

instead of putting all three responsibilities into the commit implementation.

---

## 10. Why Use Three-Way Merge?

A merge needs to distinguish changes made independently on two sides.

The implementation therefore works with:

```text
base
current
target
```

A simplified decision model is:

```text
current == target
    → no difference between the two sides

current == base
    → target changed

target == base
    → current changed

current != base
and target != base
and current != target
    → conflict
```

This is the conceptual basis of the merge implementation.

---

## 11. Why Persist Merge State?

A conflicting merge cannot always be completed immediately.

Mini Git stores merge information so that the repository can represent an unfinished merge.

This supports the separate operations:

```text
merge
merge --continue
merge --abort
```

The repository records information required by these operations.

---

## 12. Why Represent Snapshots as Maps?

Diff and merge logic frequently needs to compare paths.

A useful internal representation is:

```text
path → content
```

This makes it possible to compare two repository states using the same general representation.

---

## 13. Why Use Filesystem APIs?

The repository operates directly on files and directories.

C++20's `std::filesystem` provides the basic operations required for:

* path manipulation
* directory traversal
* file existence checks
* directory creation
* filesystem state inspection

---

## 14. Why Exclude `.mini-git` From Working-Tree Operations?

`.mini-git` contains Mini Git's internal repository state.

Treating it as normal project content would cause repository metadata to be included in normal working-tree operations.

The implementation therefore treats `.mini-git` separately from project files.

---

## 15. Error Handling

The implementation uses exceptions for many invalid operations and invalid repository states.

Examples include:

* invalid object formats
* missing objects
* invalid references
* invalid branch/tag names
* unsafe repository operations
* invalid merge state

The objective is to fail explicitly instead of continuing with an invalid repository state.

---

## 16. Object Type Detection

Mini Git cannot safely classify arbitrary serialized data simply by assuming that a prefix always identifies an object type.

For example, tree data can contain entries beginning with textual prefixes associated with object types.

The implementation therefore attempts valid deserialization when identifying an object.

If the data cannot be interpreted as a supported object format, an exception is raised.

---

## 17. Repository Integrity

Integrity checking is separated from normal repository operations.

This allows the project to inspect:

* object validity
* references
* relationships
* reachability

without making normal commands responsible for every diagnostic concern.

---

## 18. Reachability

Reachability is based on references and object relationships.

Starting from relevant repository references, the implementation follows:

```text
commit
 ├── tree
 │    ├── tree
 │    └── blob
 │
 └── parent commit
```

Objects that are not encountered through these relationships can be reported as unreachable.

---

## 19. Diagnostics as First-Class Components

Mini Git includes analysis components rather than putting every diagnostic operation into `main.cpp`.

This keeps operations such as:

```text
inspect
graph
stats
analyze
impact
doctor
storage
fsck
```

associated with reusable C++ classes.

---

## 20. Testing Philosophy

Tests focus on behavior rather than implementation details whenever practical.

Repository tests use temporary directories.

This allows scenarios such as:

* repository initialization
* commits
* branches
* checkout
* merge
* object corruption
* reachability

to be tested without depending on the user's actual project directory.

---

## 21. C++ Design

The project uses C++20 and the standard library extensively.

Important facilities include:

* `std::filesystem`
* `std::string`
* `std::vector`
* `std::map`
* `std::set`
* `std::optional`

The code also uses:

* RAII
* const correctness
* references for non-owning relationships
* classes with focused responsibilities

---

## 22. External Dependency

OpenSSL is used for SHA-256 hashing.

The rest of the core repository implementation primarily relies on the C++ standard library.

---

## 23. Why CMake?

CMake provides the project's build configuration.

It defines:

* the C++ standard
* executable sources
* include paths
* OpenSSL linkage
* test executables
* CTest registration

---

## 24. Why CTest?

CTest provides one mechanism for executing the independent test programs.

The Phase 20 suite contains tests covering the major implemented subsystems.

The final verification command is:

```bash
ctest --test-dir build --output-on-failure
```

---

## 25. Git-Inspired, Not Git-Compatible

Mini Git deliberately uses:

```text
.mini-git/
```

rather than:

```text
.git/
```

Its serialized objects and repository metadata are also Mini Git's own format.

The project therefore demonstrates version-control concepts without attempting to reproduce Git's complete on-disk format or behavior.

---

## 26. Design Summary

The central design idea is:

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

Higher-level operations such as diff, merge, integrity checking, and analysis operate on this model.
