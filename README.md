# Mini Git

A Git-inspired version control system built from scratch in C++.

The goal of this project is to explore how version control systems work internally, including:

- content-addressable storage
- object databases
- blobs
- trees
- commits
- staging areas
- references
- branches
- commit graphs

This is an educational project inspired by Git. It is not intended to be a replacement for real Git.

## Build

```bash
cmake -S . -B build
cmake --build build


We'll expand the README substantially later.

---

# 1.13 `docs/architecture.md`

For now:

```markdown
# Architecture

Mini Git will be developed as a layered version control system.

Initial architecture:

```text
CLI
 │
 ▼
Command Layer
 │
 ▼
Repository


Again, we're intentionally **not implementing all of this yet**.

---

# 1.14 LICENSE

Since you want this to be a public portfolio project, MIT is a reasonable choice.

You can use the standard MIT License text with your name and year.

If you'd like, we can add the proper license text after the project structure is working.

---

# 1.15 First Git commit

Once everything works:

```bash
git status