# Mini Git Testing

## 1. Overview

Mini Git uses CMake and CTest for automated testing.

The test suite is divided into separate executable targets so that major components can be tested independently.

---

## 2. Running the Tests

After configuring and building the project:

```bash
ctest --test-dir build --output-on-failure
```

This runs the registered test suite and prints the output of failed tests.

---

## 3. Building the Project

The standard build command is:

```bash
cmake --build build
```

For a clean build:

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

---

## 4. Test Organization

The suite covers:

### Core functionality

* hashing
* blobs
* trees
* commits
* object database
* file reading
* references
* repository
* index
* tree building
* status

### Repository operations

* log
* branches
* checkout
* diff
* merge
* merge state
* tags

### Educational features

* inspector
* explainer
* graph
* statistics

### Repository intelligence

* analyzer
* impact
* doctor
* storage
* benchmark

### Integrity

* object types
* reachability
* integrity checking
* fsck
* ancestry
* garbage-collection analysis

---

## 5. Test Isolation

Repository-dependent tests create temporary directories.

A typical test follows this pattern:

```text
create temporary repository
        │
        ▼
initialize repository
        │
        ▼
perform operation
        │
        ▼
assert expected state
        │
        ▼
remove temporary repository
```

This prevents tests from depending on the user's actual project repository.

---

## 6. Unit Tests

Unit tests target individual components.

Examples include:

```text
Hash
Blob
Commit
Reference
Index
FileReader
ObjectType
```

These tests verify behavior without requiring the complete CLI.

---

## 7. Repository Tests

Repository tests exercise interactions between multiple components.

Examples include:

* repository initialization
* branch creation
* checkout
* commits
* object storage
* index reconstruction

These tests use temporary repository structures.

---

## 8. Merge Tests

Merge tests cover scenarios involving:

* successful merges
* fast-forward situations
* already-up-to-date situations
* conflicts
* merge state
* continuation
* abortion

The purpose is to test repository state transitions rather than only textual output.

---

## 9. Integrity Tests

Integrity-related tests cover:

* object classification
* reachability
* corrupted objects
* missing objects
* invalid relationships
* unreachable objects
* ancestry
* garbage-collection analysis

---

## 10. Regression Testing

Every significant change should be followed by:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

This ensures that changes to one subsystem do not silently break another subsystem.

---

## 11. Test Failures

When a test fails, first determine whether the failure is:

* compilation
* linking
* runtime exception
* assertion failure
* filesystem/environment related

The failing test can also be executed directly.

For example:

```bash
./build/doctor-tests
```

This is useful when the test aborts before CTest provides enough context.

---

## 12. Test Philosophy

The tests are intended to validate externally observable behavior and important internal invariants.

The suite is not intended to guarantee compatibility with Git.

It validates Mini Git's own implementation.

---

## 13. Final Verification

Before a release tag is created, the project should satisfy:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

All registered tests should pass.

The final repository should also be reviewed with:

```bash
git status
git diff
```

before committing the release.

---

## 14. Testing Summary

The testing architecture is:

```text
CMake
  │
  ▼
CTest
  │
  ├── unit tests
  ├── repository tests
  ├── merge tests
  ├── analysis tests
  └── integrity tests
```

This gives Mini Git a repeatable regression-testing workflow.
