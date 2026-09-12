# Mini Git Integrity and Recovery

## 1. Overview

Mini Git contains several components for inspecting repository consistency:

* `ObjectType`
* `Reachability`
* `IntegrityChecker`
* `Ancestry`
* `GarbageCollector`
* `Doctor`

These components operate on the repository's existing object and reference structures.

---

## 2. Object Integrity

Objects are identified using SHA-256-derived IDs.

The integrity checker can use the stored object data and its object ID to detect inconsistencies.

The report distinguishes between valid and corrupted objects according to the checks performed by `IntegrityChecker`.

---

## 3. Object Type Validation

Stored object data must correspond to one of the supported object types:

```text
Blob
Tree
Commit
```

Invalid serialized data is rejected by object-type detection.

---

## 4. Missing Objects

References and object relationships may refer to object IDs.

If a referenced object does not exist in the object database, the integrity system can report it as missing.

Examples include:

* a branch pointing to a missing object
* a commit referring to a missing object
* a tree containing a missing entry

---

## 5. Invalid Relationships

Integrity checking also examines relationships between stored objects.

Examples include relationships involving:

```text
commit → tree
commit → parent
tree → tree
tree → blob
```

Invalid relationships can be reported separately from corrupted object data.

---

## 6. Reachability

Reachability starts from repository references and follows stored object relationships.

Conceptually:

```text
reference
    │
    ▼
 commit
  /   \
tree  parent
 │
 ▼
blob/tree
```

The set of objects reached through traversal represents reachable repository data.

---

## 7. Unreachable Objects

An object may exist in the object database without being reachable from the references considered by the reachability implementation.

Such objects are reported as unreachable.

Unreachable does not automatically mean corrupted.

It means that the object is not reached through the repository graph used by the analysis.

---

## 8. Integrity Report

`IntegrityReport` contains:

```text
total_objects
valid_objects
corrupted_objects
missing_objects
invalid_references
unreachable_objects
```

The report also provides a consistency result.

---

## 9. `fsck`

The CLI exposes integrity checking through:

```bash
./build/mini-git fsck
```

This is intended as a diagnostic command.

It does not replace the normal repository operations.

---

## 10. Doctor

The Doctor provides a higher-level health report:

```bash
./build/mini-git doctor
```

The current implementation checks repository structure, references, referenced objects, and reachability information.

It summarizes findings as:

```text
Health: GOOD
```

or a warning/error state depending on the findings.

---

## 11. Garbage Collection Analysis

`GarbageCollector` identifies unreachable objects and estimates their reclaimable storage.

Its report contains:

```text
unreachable_objects
reclaimable_bytes
```

The current implementation is a preview/reporting mechanism.

It does not automatically delete unreachable objects.

---

## 12. Recovery Philosophy

Mini Git's diagnostic tools are primarily inspection tools.

They report repository problems rather than automatically modifying repository history to repair them.

This is intentional.

Automatic recovery can itself destroy information if the diagnosis is incorrect.

---

## 13. Merge Recovery

Merge operations have explicit recovery commands.

If a merge is in progress:

```bash
./build/mini-git merge --continue
```

can continue the merge after resolution.

Alternatively:

```bash
./build/mini-git merge --abort
```

can abort the merge.

---

## 14. Repository State During Merge

The repository stores merge state including information needed by the merge implementation.

This allows `status` and merge commands to recognize an unfinished merge.

---

## 15. Integrity vs Recovery

The project deliberately separates:

```text
diagnosis
```

from:

```text
modification
```

Integrity commands identify problems.

Repository operations perform changes.

The diagnostic commands do not claim to repair every possible repository corruption automatically.

---

## 16. Recommended Diagnostic Sequence

When investigating a repository problem:

```bash
./build/mini-git status
./build/mini-git doctor
./build/mini-git fsck
./build/mini-git stats
./build/mini-git storage
```

The exact useful command depends on the problem being investigated.

---

## 17. Summary

Mini Git provides several levels of repository inspection:

```text
Object
  │
  ▼
ObjectType
  │
  ▼
IntegrityChecker
  │
  ├── missing
  ├── corrupted
  ├── invalid relationships
  └── unreachable
          │
          ▼
   GarbageCollector
```

These facilities are intended to make repository internals observable and testable.
