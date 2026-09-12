# Mini Git Object Model

## 1. Overview

Mini Git represents repository history using three primary object types:

```text
Object
├── Blob
├── Tree
└── Commit
```

Each object provides a serialized representation that can be stored in the object database.

---

## 2. Object

`Object` is the base abstraction.

It defines:

```cpp
virtual std::string serialize() const = 0;
```

The derived classes provide their own serialization.

---

## 3. Blob

A `Blob` represents file content.

Conceptually:

```text
file content
     │
     ▼
    Blob
     │
     ▼
 serialized representation
```

The Blob class supports:

* construction from content
* construction from a file
* deserialization
* content access
* serialization

---

## 4. Tree

A `Tree` represents directory entries.

Each `TreeEntry` contains:

```text
name
object_id
is_tree
```

A tree can therefore reference:

* another Tree
* a Blob

This produces a directory hierarchy.

---

## 5. Commit

A `Commit` represents a point in repository history.

It contains:

```text
tree ID
parent IDs
author
message
```

A commit may have multiple parents.

This allows merge commits to be represented.

---

## 6. Commit Relationships

A normal history can be represented as:

```text
A
│
▼
B
│
▼
C
```

where each commit stores the ID of its parent.

A merge can produce:

```text
      B
     / \
    A   D
     \ /
      C
```

where `C` has multiple parents.

---

## 7. Tree Relationships

A commit points to a tree:

```text
Commit
   │
   ▼
 Tree
```

A tree can point to nested trees and blobs:

```text
Tree
├── Blob
├── Blob
└── Tree
    ├── Blob
    └── Blob
```

---

## 8. Object IDs

Objects are stored using SHA-256-derived IDs.

The general process is:

```text
object serialization
        │
        ▼
      SHA-256
        │
        ▼
     object ID
```

Other objects use these IDs to reference stored objects.

---

## 9. Object Storage

The `ObjectDatabase` stores serialized objects.

The object database provides:

```cpp
store()
exists()
read()
object_ids()
```

It does not construct commits or trees itself.

Those responsibilities belong to the object classes and higher-level components.

---

## 10. Object Type Detection

`ObjectType` identifies whether serialized data can be interpreted as:

```text
Blob
Tree
Commit
```

Detection relies on the project's serialization formats and deserialization logic.

Invalid or unrecognized serialized data results in an exception.

---

## 11. Object Graph

The repository can therefore be viewed as an object graph:

```text
Reference
    │
    ▼
 Commit
 ┌──┴───────┐
 ▼          ▼
Tree      Parent
 │          │
 ├──Blob    ▼
 ├──Tree   Commit
 └──Blob
```

The same object may be referenced by multiple parts of the graph.

---

## 12. Why the Model Matters

This model allows Mini Git to implement:

* history traversal
* branch references
* merges
* reachability
* integrity checking
* object inspection
* repository analysis

using the same underlying objects.

---

## 13. Serialization Boundary

Object classes own serialization and deserialization.

The object database stores the serialized representation.

This gives the project a separation between:

```text
object representation
```

and:

```text
persistent storage
```

---

## 14. Object Integrity

Because objects are identified through SHA-256, the integrity subsystem can compare stored object data with the expected object ID.

This provides one mechanism for detecting modified object data.

---

## 15. Object Immutability

The repository model treats stored historical objects as content-addressed records.

Higher-level operations create new objects when repository history changes rather than modifying existing commit history in place.

This is a design property of the object model; it should not be interpreted as a filesystem-level immutability guarantee.

---

## 16. Summary

The object model is the foundation of Mini Git:

```text
              Object
             /  |  \
            /   |   \
         Blob  Tree  Commit
                │      │
                │      ├── tree
                │      └── parents
                │
                └── entries
```

References identify commits, commits identify trees and parents, and trees identify other trees and blobs.
