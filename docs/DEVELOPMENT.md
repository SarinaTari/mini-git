# Mini Git Development Guide

## 1. Requirements

Development requires:

* a C++20 compiler
* CMake 3.20 or newer
* OpenSSL
* a filesystem environment supporting the repository operations used by Mini Git

The project has been developed and tested on macOS with Apple Silicon.

---

## 2. Repository Structure

The main project structure is:

```text
mini-git/
├── CMakeLists.txt
├── include/
├── src/
├── tests/
├── docs/
└── README.md
```

---

## 3. Source Code

Production implementation files are stored in:

```text
src/
```

Public project headers are stored in:

```text
include/
```

Tests are stored in:

```text
tests/
```

Documentation is stored in:

```text
docs/
```

---

## 4. Configure

A new build directory can be configured with:

```bash
cmake -S . -B build
```

---

## 5. Build

Build the project with:

```bash
cmake --build build
```

Parallel builds can also be used:

```bash
cmake --build build --parallel
```

---

## 6. Run Mini Git

The executable is:

```bash
./build/mini-git
```

For example:

```bash
./build/mini-git --version
```

---

## 7. Run Tests

Run the complete suite with:

```bash
ctest --test-dir build --output-on-failure
```

Individual test executables can also be run directly.

Example:

```bash
./build/hash-tests
```

---

## 8. Adding a New Component

A typical new component requires:

```text
include/NewComponent.hpp
src/NewComponent.cpp
tests/NewComponentTests.cpp
```

The corresponding sources and test target should then be added to `CMakeLists.txt`.

---

## 9. Header Responsibilities

Headers should define the public interface of their corresponding components.

Implementation details should normally remain in `.cpp` files unless they need to be visible for templates or other language requirements.

---

## 10. Source Responsibilities

Source files should contain implementation logic corresponding to their headers.

When a source file directly uses a type or function from another component, it should include the appropriate header rather than relying on transitive includes.

---

## 11. Error Handling

New functionality should explicitly handle invalid states.

Avoid silently ignoring:

* missing files
* invalid object IDs
* invalid repository state
* malformed serialized objects
* invalid references

Errors should be reported consistently with the surrounding implementation.

---

## 12. Testing New Features

New functionality should have corresponding tests where practical.

A feature should ideally be tested for:

* normal behavior
* invalid input
* boundary cases
* repository state transitions
* filesystem behavior
* interactions with related components

---

## 13. Temporary Repositories

Repository tests should use temporary directories.

A test should:

1. create a unique temporary repository path
2. remove any previous test data
3. create the directory
4. initialize Mini Git
5. perform the test
6. assert the expected behavior
7. clean up

---

## 14. Build Before Testing

After source changes:

```bash
cmake --build build
```

should be run before:

```bash
ctest --test-dir build --output-on-failure
```

A test failure caused by an old binary can otherwise be misleading.

---

## 15. Clean Builds

If the build directory may contain stale configuration or generated files:

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

This is particularly useful after:

* renaming the project
* changing CMake targets
* changing dependencies
* moving source files

---

## 16. Code Quality

The project should maintain:

* clear class responsibilities
* const correctness
* RAII
* meaningful names
* explicit error handling
* minimal duplication
* focused tests

Code should remain understandable to someone studying the project rather than only to someone who already knows the implementation.

---

## 17. Documentation

When externally visible behavior changes, the relevant documentation should be updated.

At minimum, consider:

```text
README.md
docs/COMMANDS.md
docs/ARCHITECTURE.md
docs/LIMITATIONS.md
```

The documentation should describe implemented behavior rather than planned functionality.

---

## 18. Git Workflow

A normal development cycle is:

```text
modify
   │
   ▼
build
   │
   ▼
test
   │
   ▼
inspect diff
   │
   ▼
commit
```

Before a release:

```bash
git status
git diff
ctest --test-dir build --output-on-failure
```

should be reviewed.

---

## 19. Release Workflow

The final release process should include:

1. Build successfully.
2. Run the complete test suite.
3. Review source changes.
4. Review documentation.
5. Verify repository cleanliness.
6. Commit the release changes.
7. Create the final version tag.
8. Push the branch and tag.

---

## 20. Development Principle

Mini Git is an educational systems project.

When choosing between two implementations, prefer the solution that is:

* correct
* understandable
* testable
* appropriately modular

rather than adding complexity solely to imitate production Git internals.
