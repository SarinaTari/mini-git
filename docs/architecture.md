# Mini Git Architecture

Mini Git is an educational Git-inspired version control system
implemented in C++.

The project is designed to demonstrate the concepts behind
version control systems, including:

- content-addressed storage
- object databases
- snapshots
- staging
- commits
- references
- branches
- history graphs
- checkout
- diff
- merging

## Initial Architecture

```text
mygit CLI
    |
    v
Command Layer
    |
    v
Repository


Notice the sentence:

> **The architecture will evolve as new functionality is implemented.**

That's intentional.

---

# 1.15 README

Let's give the project an initial professional README.

```markdown
# Mini Git

Mini Git is an educational Git-inspired version control system
implemented from scratch in C++.

The project explores the internal concepts behind Git rather than
simply reproducing its command-line interface.

## Goals

- Understand Git's internal object model
- Implement content-addressed storage
- Build a simplified object database
- Implement blobs, trees, and commits
- Implement a staging index
- Implement branches and references
- Explore commit graphs
- Implement simplified diff and merge functionality
- Build a robust and well-tested C++ systems project

## Status

Early development.

## Planned Features

- Repository initialization
- Content hashing
- Blob objects
- Tree objects
- Commit objects
- Object database
- Staging/index
- Status
- Commit history
- Branches
- Checkout
- Diff
- Simplified merge
- Tags
- Repository integrity checking

## Educational Features

Mini Git will also provide tools for inspecting and understanding
its internal state, including:

- repository inspection
- object inspection
- commit graph visualization
- repository statistics
- integrity checking
- internal operation explanations

## Disclaimer

Mini Git is an educational project inspired by Git's architecture.

It is not intended to be compatible with or replace production Git.