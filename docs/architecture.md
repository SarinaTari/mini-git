# Mini Git Architecture

## Overview

Mini Git is an educational version control system written in modern C++.

The project is designed to demonstrate how a Git-like version control system can be built from fundamental concepts such as:

* filesystem operations
* content-addressable storage
* hashing
* objects
* repositories
* object databases
* trees
* staging
* commits
* references
* branches
* history

Mini Git is **not intended to be a drop-in replacement for Git** and does not aim for compatibility with Git's complete internal format.

Instead, the project focuses on understanding the architecture and engineering principles behind a version control system.

The implementation is intentionally incremental. Each phase introduces a new subsystem and builds on functionality implemented previously.

---

# High-Level Architecture

The architecture is being developed incrementally toward the following structure:

```text
                              mini-git CLI
                                   │
                                   ▼
                            Command Layer
                                   │
             ┌─────────────────────┼─────────────────────┐
             ▼                     ▼                     ▼
        Working Tree             Index              Repository
             │                     │                     │
             │                     │                     ▼
             │                     │              Object Database
             │                     │                     │
             │                     │              ┌──────┼──────┐
             │                     │              ▼      ▼      ▼
             │                     │            Blob    Tree   Commit
             │                     │
             │                     ▼
             │                  Status
             │
             ▼
          Trees
             │
             ▼
       Future Commits
             │
             ▼
        References
             │
             ▼
            HEAD
```

The currently implemented foundation includes:

* repository initialization
* SHA-256 hashing
* object abstractions
* Blob objects
* Tree objects
* Commit objects
* binary-safe file reading
* file-to-Blob conversion
* object serialization
* object identifiers
* persistent object storage
* object retrieval
* object existence checking
* duplicate-object detection
* deterministic Tree serialization
* recursive Tree construction
* directory-to-Tree conversion
* nested Trees
* empty-directory Trees
* `.mini-git` exclusion
* Index / staging
* Index persistence
* `mini-git add <file>`

Higher-level functionality such as `status`, commits, references, branches, checkout, diff, merge, and history will be implemented in later phases.

---

# Repository Structure

Mini Git currently follows this structure:

```text
mini-git/

├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── include/
│   ├── Repository.hpp
│   ├── Hash.hpp
│   ├── Object.hpp
│   ├── Blob.hpp
│   ├── Tree.hpp
│   ├── Commit.hpp
│   ├── FileReader.hpp
│   ├── ObjectDatabase.hpp
│   ├── TreeBuilder.hpp
│   └── Index.hpp
│
├── src/
│   ├── main.cpp
│   ├── Repository.cpp
│   ├── Hash.cpp
│   ├── Blob.cpp
│   ├── Tree.cpp
│   ├── Commit.cpp
│   ├── FileReader.cpp
│   ├── ObjectDatabase.cpp
│   ├── TreeBuilder.cpp
│   └── Index.cpp
│
├── tests/
│   ├── HashTests.cpp
│   ├── ObjectTests.cpp
│   ├── FileReaderTests.cpp
│   ├── BlobTests.cpp
│   ├── TreeBuilderTests.cpp
│   └── IndexTests.cpp
│
└── docs/
    └── architecture.md
```

The `include/` directory contains class declarations.

The `src/` directory contains implementations.

The `tests/` directory contains automated tests for individual components and their interactions.

The `docs/` directory contains project documentation and architectural information.

---

# Repository

The `Repository` class represents a Mini Git repository.

Currently, repository initialization creates:

```text
.mini-git/

├── HEAD
├── objects/
├── refs/
│   └── heads/
└──
```

The initial `HEAD` contains:

```text
ref: refs/heads/main
```

This establishes `main` as the default branch reference.

At the current stage, `main` does not yet point to a commit because repository-level commit creation has not been implemented.

The `Repository` component is responsible for repository-level filesystem structures and for identifying the `.mini-git` directory associated with the repository root.

Repository initialization is currently performed from the current working directory.

Repository discovery from nested directories is planned for a later phase.

---

# Working Tree

The working tree is the collection of actual files and directories being managed by Mini Git.

For example:

```text
project/

├── main.cpp
├── README.md
└── src/
    ├── App.cpp
    └── Utils.cpp
```

These files exist outside `.mini-git/`.

The working tree represents the user's current filesystem state.

Mini Git uses the working tree as an input to its object and staging pipelines.

The working tree can be transformed into:

```text
Files
  ↓
Blobs
  ↓
Trees
  ↓
Object Database
```

The working tree can also be compared against the Index in later status operations.

---

# Hashing

Mini Git uses SHA-256 to generate deterministic object identifiers.

The hashing layer is implemented by the `Hash` class.

Conceptually:

```text
Input Data
    │
    ▼
 SHA-256
    │
    ▼
64-character hexadecimal Object ID
```

For example:

```text
"hello"
   │
   ▼
SHA-256
   │
   ▼
2cf24dba5fb0a30e26e83b2ac5b9e29e...
```

The hash depends entirely on the input data.

Therefore:

```text
same input
    ↓
same hash
```

while:

```text
different input
    ↓
different hash
```

The hash function is provided through OpenSSL's cryptographic library.

Mini Git hashes the **serialized object representation**, rather than hashing raw file contents directly.

This distinction is important because different object types have different serialized representations.

---

# Object Model

Mini Git uses an object-oriented representation for repository objects.

The base abstraction is:

```text
Object
  │
  ├── Blob
  │
  ├── Tree
  │
  └── Commit
```

The base `Object` class defines the common serialization interface:

```cpp
virtual std::string serialize() const = 0;
```

Every object must therefore provide a serialized representation.

This creates a common pipeline:

```text
Object
   │
   ▼
serialize()
   │
   ▼
Serialized Bytes
   │
   ▼
Hash
   │
   ▼
Object ID
```

The Object Database operates on this common `Object` abstraction rather than needing to know the internal implementation details of every object type.

---

# Blob

A Blob represents file contents.

Conceptually:

```text
File
 │
 ▼
Blob
 │
 └── file contents
```

A Blob does not need to know the original filename.

For example:

```text
main.cpp
```

might contain:

```cpp
int main() {
    return 0;
}
```

The Blob represents the content itself.

The filename is stored separately by Tree objects and Index entries.

This separation is important because identical file contents can produce the same Blob object regardless of the filename used to reference them.

---

# File Reading and Blob Objects

Mini Git separates filesystem file reading from object representation.

The `FileReader` component is responsible for reading the exact bytes of a filesystem file.

The current workflow is:

```text
File
 │
 ▼
FileReader
 │
 ▼
File Contents
 │
 ▼
Blob::from_file()
 │
 ▼
Blob
 │
 ▼
serialize()
 │
 ▼
SHA-256
 │
 ▼
Object ID
```

The pipeline continues into persistent storage:

```text
File
 │
 ▼
FileReader
 │
 ▼
Blob
 │
 ▼
serialize()
 │
 ▼
SHA-256
 │
 ▼
Object ID
 │
 ▼
ObjectDatabase
 │
 ▼
.mini-git/objects/<object-id>
```

This separation keeps responsibilities clear:

```text
FileReader
    → reads bytes from the filesystem

Blob
    → represents file content as an object

Hash
    → generates object identifiers

ObjectDatabase
    → persists and retrieves serialized objects
```

---

# FileReader

`FileReader` provides a simple abstraction for reading files.

Its interface is:

```cpp
class FileReader {
public:
    static std::string read(
        const std::filesystem::path& path
    );
};
```

Files are opened in binary mode:

```cpp
std::ifstream file(path, std::ios::binary);
```

This is important because Mini Git must be able to represent arbitrary files, not only text files.

For example, a file may contain:

```text
00 01 7F FF
```

A binary-safe reader must preserve those bytes exactly.

The implementation therefore reads the complete byte sequence into a `std::string`.

Although `std::string` is commonly associated with text, it can safely contain arbitrary byte sequences, including null bytes.

Therefore:

```text
binary file
    │
    ▼
FileReader
    │
    ▼
exact byte sequence
```

No text conversion is intentionally performed.

---

# Blob::from_file()

A Blob can be created directly from a filesystem file using:

```cpp
Blob::from_file()
```

The internal flow is:

```text
Blob::from_file(path)
        │
        ▼
FileReader::read(path)
        │
        ▼
File contents
        │
        ▼
Blob
```

This keeps responsibilities separated.

`FileReader` handles filesystem input.

`Blob` handles object representation.

The Blob class therefore does not need to implement file-reading logic itself.

---

# Blob Serialization

Mini Git currently serializes Blob objects using the following simplified format:

```text
blob <size>\0<content>
```

For example, the content:

```text
Hello Mini Git!
```

has 15 bytes.

Its serialized representation is conceptually:

```text
blob 15\0Hello Mini Git!
```

The `\0` represents an actual null byte.

The size is calculated from the byte count of the content:

```cpp
content_.size()
```

This is important because the object format must describe the exact number of bytes contained in the object.

The serialization is intentionally inspired by Git's object model but is **not Git-compatible**.

Mini Git uses this simplified representation to make the object model easier to understand.

The serialized representation is what gets hashed to produce the object identifier.

---

# Tree

A Tree represents a directory-like structure.

Unlike a Blob, which represents only file contents, a Tree represents relationships between names and repository objects.

Each Tree entry currently contains:

```text
name
object_id
is_tree
```

Conceptually:

```text
Tree

├── main.cpp  → Blob
├── README.md → Blob
└── src       → Tree
```

The relationship can be visualized as:

```text
                    Tree
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       Blob       Blob         Tree
      main.cpp   README.md      │
                                ├── Blob
                                └── Blob
```

The filename belongs to the Tree entry, not to the Blob.

This allows the same Blob object to potentially be referenced by different paths.

---

# Tree Serialization

Tree serialization converts the entries of a Tree into a deterministic byte representation.

The current representation is:

```text
blob <object-id> <name>
tree <object-id> <name>
```

with one entry per line.

For example:

```text
blob aaa README.md
tree bbb src
blob zzz main.cpp
```

Tree entries are sorted by name before serialization.

This is important because filesystem directory iteration order should not determine the resulting Tree object ID.

Conceptually:

```text
Directory Entries
        │
        ▼
   Sort by Name
        │
        ▼
Deterministic Tree Serialization
        │
        ▼
      SHA-256
        │
        ▼
   Tree Object ID
```

Therefore, inserting the same entries in different orders produces the same serialized Tree.

This gives Mini Git deterministic Tree identity.

---

# TreeBuilder

`TreeBuilder` is responsible for converting actual filesystem directories into Tree objects.

Its purpose is to connect the working tree to the object model.

The architecture is:

```text
Filesystem Directory
        │
        ▼
   TreeBuilder
        │
        ├──────────────┐
        ▼              ▼
   Regular File     Directory
        │              │
        ▼              ▼
      Blob         Recursive
        │          TreeBuilder
        │              │
        ▼              ▼
   Object ID        Tree ID
        │              │
        └──────┬───────┘
               ▼
          Parent Tree
               │
               ▼
        ObjectDatabase
```

`TreeBuilder` receives an `ObjectDatabase` reference.

This allows it to store every Blob and Tree it creates.

The builder itself does not implement object persistence.

Instead:

```text
TreeBuilder
    → constructs object relationships

ObjectDatabase
    → persists those objects
```

This separation keeps the architecture modular.

---

# Building Blobs from Files

When `TreeBuilder` encounters a regular file, it performs:

```text
Regular File
     │
     ▼
Blob::from_file()
     │
     ▼
Blob
     │
     ▼
ObjectDatabase::store()
     │
     ▼
Blob Object ID
```

The resulting object ID is added to the Tree together with the filename.

For example:

```text
main.cpp
```

may become:

```text
main.cpp → Blob abc123...
```

The Tree does not contain the file contents directly.

It contains a reference to the Blob object.

---

# Building Trees from Directories

When `TreeBuilder` encounters a directory, it recursively builds another Tree.

For example:

```text
project/

├── main.cpp
└── src/
    ├── App.cpp
    └── Utils.cpp
```

becomes:

```text
Root Tree

├── main.cpp → Blob

└── src → Tree
           │
           ├── App.cpp → Blob
           └── Utils.cpp → Blob
```

The recursive process is:

```text
build(project/)
    │
    ├── main.cpp
    │       │
    │       ▼
    │      Blob
    │
    └── src/
            │
            ▼
       build(src/)
            │
            ├── App.cpp → Blob
            │
            └── Utils.cpp → Blob
            │
            ▼
           Tree
```

The nested Tree is stored in the Object Database.

Its object ID is then added to the parent Tree.

---

# Recursive Tree Construction

The complete recursive workflow is:

```text
Directory
    │
    ▼
TreeBuilder::build()
    │
    ├── File
    │    │
    │    ▼
    │   Blob
    │    │
    │    ▼
    │  Object ID
    │
    └── Directory
         │
         ▼
    TreeBuilder::build()
         │
         ▼
        Tree
         │
         ▼
      Object ID
         │
         ▼
    Parent Tree
         │
         ▼
 ObjectDatabase::store()
```

This process continues until the entire directory hierarchy has been represented.

Therefore a complete project can be transformed into an object graph.

---

# Empty Directories

Empty directories are represented by empty Tree objects.

For example:

```text
project/

└── empty/
```

produces:

```text
Root Tree
└── empty → Tree
```

The `empty` Tree contains no entries but is still stored in the Object Database.

This demonstrates that Trees represent directories independently of whether they contain files.

---

# `.mini-git` Exclusion

The `.mini-git` directory is repository metadata and must not become part of the project's own working-tree snapshot.

Therefore `TreeBuilder` explicitly ignores:

```text
.mini-git/
```

The traversal performs:

```text
Directory Entry
      │
      ▼
Is it .mini-git?
   ┌──┴──┐
   │     │
  yes    no
   │     │
   ▼     ▼
 skip   process
```

Without this rule, building the root Tree would recursively include the repository's own object database.

That would create an undesirable self-reference:

```text
.mini-git/
    │
    ▼
   Tree
    │
    ▼
Object Database
    │
    ▼
.mini-git/
```

Ignoring `.mini-git` prevents repository metadata from becoming part of the working-tree snapshot.

---

# Tree Object Storage

Trees are stored in the Object Database in the same general way as other objects.

The pipeline is:

```text
Tree
 │
 ▼
serialize()
 │
 ▼
Serialized Tree
 │
 ▼
SHA-256
 │
 ▼
Tree Object ID
 │
 ▼
ObjectDatabase
 │
 ▼
.mini-git/objects/<tree-id>
```

This means Trees participate in the same content-addressable storage system as Blobs.

---

# Complete Directory-to-Object Pipeline

The complete filesystem-to-object workflow is:

```text
                    Filesystem
                        │
                        ▼
                 TreeBuilder
                        │
          ┌─────────────┴─────────────┐
          ▼                           ▼
       File                       Directory
          │                           │
          ▼                           ▼
   FileReader                    TreeBuilder
          │                           │
          ▼                           ▼
        Blob                         Tree
          │                           │
          ▼                           ▼
   ObjectDatabase             ObjectDatabase
          │                           │
          └─────────────┬─────────────┘
                        ▼
                   Parent Tree
                        │
                        ▼
                    serialize()
                        │
                        ▼
                     SHA-256
                        │
                        ▼
                    Object ID
                        │
                        ▼
                 Object Database
                        │
                        ▼
          .mini-git/objects/<object-id>
```

For example:

```text
project/

├── README.md
├── main.cpp
└── src/
    ├── App.cpp
    └── Utils.cpp
```

can become:

```text
Root Tree

├── README.md → Blob A
├── main.cpp  → Blob B
└── src        → Tree C
                  │
                  ├── App.cpp   → Blob D
                  └── Utils.cpp → Blob E
```

The objects are stored separately:

```text
.mini-git/objects/

├── Blob A
├── Blob B
├── Blob C
├── Blob D
└── Blob E
```

The root Tree acts as the entry point into the complete directory snapshot.

---

# Content-Addressable Storage

Mini Git is designed around content-addressable storage.

Instead of identifying an object using a human-generated numeric ID, the object identifier is derived from its serialized contents.

Conceptually:

```text
Object
  │
  ▼
Serialize
  │
  ▼
SHA-256
  │
  ▼
Object ID
```

The same principle applies to both Blobs and Trees.

For example:

```text
Blob
 │
 ▼
Serialized Blob
 │
 ▼
SHA-256
 │
 ▼
Blob ID
```

and:

```text
Tree
 │
 ▼
Serialized Tree
 │
 ▼
SHA-256
 │
 ▼
Tree ID
```

This creates an important property:

```text
same object content
        ↓
same serialized representation
        ↓
same object ID
```

and therefore enables object deduplication.

---

# Object Database

The Object Database is responsible for persistent storage and retrieval of serialized repository objects.

The implementation is provided by the `ObjectDatabase` class.

Its primary responsibilities are:

* storing serialized objects
* generating object identifiers
* checking whether objects already exist
* reading objects by object identifier
* avoiding duplicate object storage

The Object Database works with the generic `Object` interface.

This means it does not need to know whether an object is a Blob, Tree, or Commit.

Conceptually:

```text
Object
   │
   ▼
serialize()
   │
   ▼
Serialized Data
   │
   ▼
SHA-256
   │
   ▼
Object ID
   │
   ▼
ObjectDatabase
   │
   ▼
Persistent File
```

This generic design allows the same storage mechanism to persist different object types.

---

# Object Storage Layout

Mini Git currently uses a simplified object storage layout:

```text
.mini-git/

└── objects/

    ├── <object-id-1>
    ├── <object-id-2>
    └── <object-id-3>
```

Each stored object is represented by a file whose filename is its object identifier.

The file contains the serialized object data.

The current storage format is intentionally simpler than Git's actual object database layout.

Git commonly uses a two-level directory structure based on the beginning of the object hash.

Mini Git currently uses a flat directory:

```text
.mini-git/objects/<object-id>
```

This is a deliberate simplification for educational purposes.

---

# ObjectDatabase::store()

The `store()` operation performs the following steps:

```text
Object
   │
   ▼
serialize()
   │
   ▼
Serialized Data
   │
   ▼
SHA-256
   │
   ▼
Object ID
   │
   ▼
Check Existing Object
   │
   ├── exists → reuse ID
   │
   └── does not exist
            │
            ▼
      Write Object File
```

The object ID is calculated from the complete serialized representation.

If an object with the same ID already exists, the existing object is reused.

This means identical objects are not stored multiple times.

For example:

```text
Blob A

"Hello Mini Git!"

       │
       ▼

Object ID X


Blob B

"Hello Mini Git!"

       │
       ▼

Object ID X
```

The Object Database therefore stores only one copy.

The same principle applies to Trees.

---

# Object Retrieval

Objects can be retrieved using their object identifiers.

Conceptually:

```text
Object ID
    │
    ▼
ObjectDatabase::read()
    │
    ▼
.mini-git/objects/<object-id>
    │
    ▼
Serialized Object Data
```

If the requested object does not exist, the Object Database reports an error.

The current `read()` operation returns the stored serialized bytes.

Deserializing those bytes back into a specific `Blob`, `Tree`, or `Commit` object will be introduced as the object system becomes more complete.

---

# Object Existence

The Object Database can determine whether an object exists:

```cpp
database.exists(object_id);
```

Conceptually:

```text
Object ID
    │
    ▼
Check Object Path
    │
    ├── exists → true
    │
    └── missing → false
```

This is used by the storage system to avoid duplicate objects.

---

# Hash Object Command

Mini Git currently provides:

```bash
mini-git hash-object <file>
```

The command connects the command-line interface to the Object Database.

Its current workflow is:

```text
File
 │
 ▼
Blob::from_file()
 │
 ▼
Blob
 │
 ▼
ObjectDatabase::store()
 │
 ▼
SHA-256
 │
 ▼
Object ID
 │
 ▼
.mini-git/objects/<object-id>
```

The command prints the resulting object identifier.

Running the command again without changing the file produces the same object identifier and reuses the existing object.

Changing the file contents produces a different object identifier and therefore creates a different stored object.

---

# Hash File Command

Mini Git also contains the earlier educational `hash-file` command.

The command demonstrates the file-to-Blob-to-hash pipeline without making persistent object storage the primary focus.

Conceptually:

```text
File
 │
 ▼
Blob
 │
 ▼
Serialization
 │
 ▼
SHA-256
 │
 ▼
Object ID
```

The `hash-file` command is retained as an educational/debugging interface.

The `hash-object` command extends this workflow by persisting the resulting serialized object in the Object Database.

---

# Repository and Object Database Relationship

The `Repository` and `ObjectDatabase` classes have separate responsibilities.

```text
Repository
    │
    └── identifies repository structure
             │
             ▼
          .mini-git/
             │
             ▼
       ObjectDatabase
             │
             ▼
          objects/
```

`Repository` is responsible for repository-level structure.

`ObjectDatabase` is responsible for object persistence.

This separation prevents the repository abstraction from becoming responsible for the internal details of object storage.

---

# TreeBuilder and ObjectDatabase Relationship

`TreeBuilder` depends on the Object Database but does not own it.

```text
Repository
     │
     ▼
 .mini-git/
     │
     ▼
ObjectDatabase
     ▲
     │
TreeBuilder
     │
     ▼
Working Tree
```

The Object Database provides persistence.

TreeBuilder provides filesystem-to-object conversion.

Therefore:

```text
TreeBuilder

    → discovers files and directories

    → creates Blobs and Trees

    → connects object IDs into Tree entries


ObjectDatabase

    → stores serialized objects

    → retrieves objects

    → checks object existence

    → deduplicates objects
```

This keeps filesystem traversal separate from persistence.

---

# Index / Staging Area

The Index is the staging area between the working tree and future repository snapshots.

The conceptual workflow is:

```text
Working Tree
      │
      │ mini-git add
      ▼
    Index
      │
      │ future mini-git commit
      ▼
  Repository Snapshot
```

The Index is now implemented.

Its purpose is to record which object version is currently staged for each path.

For example:

```text
Index

├── main.cpp    → Blob A
├── README.md   → Blob B
└── src/App.cpp → Blob C
```

The Index therefore acts as a mapping:

```text
path → object_id
```

The current Index entry is represented by:

```cpp
struct IndexEntry {
    std::string path;
    std::string object_id;
};
```

---

# Index Responsibilities

The `Index` class is responsible for:

* storing staged paths
* associating paths with object IDs
* updating existing staged paths
* checking whether a path is staged
* exposing staged entries
* saving the index to disk
* loading the index from disk

The Index does **not**:

* read file contents
* calculate hashes
* create Blob objects
* store Blob objects
* construct Trees
* create commits

Those responsibilities belong to other components.

The separation is:

```text
FileReader
    → reads files

Blob
    → represents file contents

Hash
    → generates object IDs

ObjectDatabase
    → stores objects

Index
    → records staged path → object ID mappings
```

---

# Index Persistence

Mini Git persists the Index at:

```text
.mini-git/index
```

The current Index format is intentionally simple text:

```text
path<TAB>object_id
```

For example:

```text
main.cpp    abc123...
README.md   def456...
src/App.cpp ghi789...
```

The format is **not compatible with Git's binary index format**.

The simplified text representation is intentional because it makes the staging mechanism easy to inspect and understand.

Future versions may introduce a more robust index representation.

---

# Index::add()

The `Index::add()` operation inserts or updates a staged entry.

Conceptually:

```text
Index::add(path, object_id)
        │
        ▼
Search existing entries
        │
   ┌────┴────┐
   │         │
 found     missing
   │         │
   ▼         ▼
update     insert
```

For example:

```text
Before:

main.cpp → Blob A
README.md → Blob B
```

Running:

```text
add main.cpp → Blob C
```

produces:

```text
After:

main.cpp → Blob C
README.md → Blob B
```

The Index does not create a duplicate entry for `main.cpp`.

This ensures that each path has one current staged object ID.

---

# Index Loading and Saving

The Index supports persistence through:

```cpp
index.save();
index.load();
```

The workflow is:

```text
Index in Memory
      │
      ▼
   save()
      │
      ▼
.mini-git/index
```

and:

```text
.mini-git/index
      │
      ▼
   load()
      │
      ▼
Index in Memory
```

This means staged information survives after Mini Git exits.

---

# Add Command

Mini Git currently provides:

```bash
mini-git add <file>
```

The command connects the working tree, Blob system, Object Database, and Index.

The complete workflow is:

```text
Working Tree
     │
     │ mini-git add <file>
     ▼
 FileReader
     │
     ▼
   Blob
     │
     ▼
ObjectDatabase
     │
     ▼
 Blob Object ID
     │
     ▼
   Index
     │
     ▼
.mini-git/index
```

More explicitly:

```text
File
 │
 ▼
Blob::from_file()
 │
 ▼
Blob
 │
 ▼
ObjectDatabase::store()
 │
 ▼
Object ID
 │
 ▼
Index::load()
 │
 ▼
Index::add()
 │
 ▼
Index::save()
 │
 ▼
.mini-git/index
```

This is the first complete implementation of the classic:

```text
Working Tree → Staging Area
```

relationship.

---

# Add Command and Object Immutability

When a file is staged, Mini Git stores its current contents as a Blob.

Suppose:

```text
main.cpp
```

contains:

```text
Version A
```

Running:

```bash
mini-git add main.cpp
```

creates:

```text
main.cpp → Blob A
```

If the file is changed:

```text
Version B
```

and staged again:

```bash
mini-git add main.cpp
```

a new Blob is created:

```text
main.cpp → Blob B
```

The previous Blob remains in the Object Database.

Conceptually:

```text
Version A
   │
   ▼
Blob A
   │
   ▼
Index ─────────→ Blob A


File changes


Version B
   │
   ▼
Blob B
   │
   ▼
Index ─────────→ Blob B
```

The old object is not modified.

This is a fundamental property of content-addressable object storage.

---

# Index and Tree Relationship

The Index and Tree represent different concepts.

The Index represents staged file versions:

```text
Index

main.cpp    → Blob A
README.md   → Blob B
src/App.cpp → Blob C
```

A Tree represents directory structure:

```text
Root Tree

├── main.cpp    → Blob A
├── README.md   → Blob B
└── src         → Tree
                   └── App.cpp → Blob C
```

The Index therefore contains path-to-object mappings, while a Tree contains hierarchical directory relationships.

The eventual relationship will be:

```text
Index
  │
  ▼
Tree Construction
  │
  ▼
Root Tree
  │
  ▼
Commit
```

The conversion from the Index into a Tree snapshot will be implemented as part of the commit workflow.

---

# Current Staging Architecture

The currently implemented staging pipeline is:

```text
                    Working Tree
                         │
                         │
                         ▼
                      File
                         │
                         ▼
                    FileReader
                         │
                         ▼
                       Blob
                         │
                         ▼
                 ObjectDatabase
                         │
                         ▼
                    Object ID
                         │
                         ▼
                       Index
                         │
                         ▼
                 .mini-git/index
```

This gives Mini Git its first persistent representation of staged project state.

---

# Current Index Limitations

The current Index implementation is intentionally simplified.

Currently supported:

```text
mini-git add <file>
```

The following functionality is not yet implemented:

```text
mini-git add .
```

Automatic detection of deleted files is also not yet implemented.

The current implementation assumes commands are executed from the repository root.

Repository discovery from nested directories will be introduced separately.

The current text index format also has limitations around paths containing special characters, particularly because the initial parser uses whitespace-based extraction.

These limitations are intentional at this stage and will be addressed through later robustness work.

---

# Commit

A Commit represents a snapshot of repository state.

The current Commit object contains:

```text
tree
parent
author
message
```

Conceptually:

```text
Commit

├── tree
├── parent
├── author
└── message
```

The commit points to a Tree.

The Tree points to Blobs and other Trees.

Therefore the object graph becomes:

```text
Commit
   │
   ▼
 Tree
   │
   ├── Blob
   ├── Blob
   └── Tree
        ├── Blob
        └── Blob
```

A commit may also point to a previous commit:

```text
Commit C
   │
   └── parent → Commit B
                    │
                    └── parent → Commit A
```

The Commit class and serialization are implemented.

However, the complete repository-level commit command has not yet been implemented.

The future commit workflow will connect:

```text
Index
  │
  ▼
Tree
  │
  ▼
Commit
  │
  ▼
Reference
```

---

# HEAD

`HEAD` identifies the currently checked-out reference.

During repository initialization, Mini Git creates:

```text
HEAD
```

containing:

```text
ref: refs/heads/main
```

Conceptually:

```text
HEAD
 │
 ▼
refs/heads/main
 │
 ▼
Commit
```

At the current stage, the final Commit relationship does not yet exist because repository-level commits and reference management have not been implemented.

This functionality will be introduced in later phases.

---

# References

References provide human-readable names for commits.

The planned structure is:

```text
.mini-git/

└── refs/
    └── heads/
        ├── main
        ├── feature-a
        └── feature-b
```

A branch reference will contain an object ID pointing to a commit.

Conceptually:

```text
main
 │
 ▼
Commit C
 │
 ▼
Commit B
 │
 ▼
Commit A
```

This makes branches lightweight pointers into the commit graph.

Reference management is not yet implemented.

---

# Commit History

Once commits and references are fully implemented, history will form a directed graph.

A simple linear history:

```text
A ← B ← C
          ↑
         main
```

A branching history:

```text
        B ← C
       /
A ←───
       \
        D ← E
             ↑
           feature
```

The branch reference points to the latest commit on that branch.

Later phases will implement history traversal and visualization.

---

# Testing Architecture

Mini Git uses automated tests for individual subsystems and their interactions.

Current test programs include:

```text
HashTests.cpp

    ├── empty input
    ├── known SHA-256 values
    ├── deterministic hashing
    ├── different inputs
    └── binary data
```

```text
ObjectTests.cpp

    ├── Blob serialization
    ├── Tree serialization
    ├── deterministic Tree serialization
    ├── Commit serialization
    ├── initial Commit serialization
    ├── Object Database storage
    ├── object existence
    ├── object retrieval
    └── duplicate-object detection
```

```text
FileReaderTests.cpp

    ├── text-file reading
    ├── binary-file reading
    ├── null-byte preservation
    └── missing-file errors
```

```text
BlobTests.cpp

    ├── Blob creation from files
    ├── Blob serialization
    └── binary-file Blob handling
```

```text
TreeBuilderTests.cpp

    ├── directory Tree construction
    ├── file-to-Blob conversion
    ├── nested directory traversal
    ├── nested Tree creation
    ├── empty directory handling
    └── .mini-git exclusion
```

```text
IndexTests.cpp

    ├── adding Index entries
    ├── updating existing entries
    ├── multiple staged entries
    ├── Index persistence
    └── update persistence
```

The tests are registered with CTest.

The complete test suite can be executed using:

```bash
ctest --test-dir build --output-on-failure
```

The filesystem-based tests use temporary files and directories where appropriate so that tests remain isolated from the actual project.

The test suite will become more extensive as repository-level functionality is introduced.

---

# Component Relationships

The current component relationships can be visualized as:

```text
                 Repository
                      │
                      ▼
                 .mini-git/
                      │
          ┌───────────┼───────────┐
          ▼           ▼           ▼
       Objects      Index        Refs
          │           │
          ▼           │
   ObjectDatabase     │
          │           │
          ▼           ▼
      objects/     .mini-git/index
```

The current filesystem-to-object relationship is:

```text
Working Tree
     │
     ▼
TreeBuilder
     │
     ├───────────────┐
     ▼               ▼
   Files         Directories
     │               │
     ▼               ▼
   Blob        Recursive Tree
     │               │
     └───────┬───────┘
             ▼
           Tree
             │
             ▼
      ObjectDatabase
```

The staging relationship is:

```text
Working Tree
     │
     ▼
FileReader
     │
     ▼
Blob
     │
     ▼
ObjectDatabase
     │
     ▼
Object ID
     │
     ▼
Index
     │
     ▼
.mini-git/index
```

The object model is:

```text
Object
 │
 ├── Blob
 │     └── file contents
 │
 ├── Tree
 │     └── object references
 │
 └── Commit
       └── snapshot metadata
```

---

# Current Data Flow

The complete currently implemented filesystem-to-object workflow is:

```text
                    Working Tree
                         │
                         ▼
                    TreeBuilder
                         │
             ┌───────────┴───────────┐
             ▼                       ▼
           File                  Directory
             │                       │
             ▼                       ▼
        FileReader             TreeBuilder
             │                       │
             ▼                       ▼
           Blob                     Tree
             │                       │
             └───────────┬───────────┘
                         ▼
                       Tree
                         │
                         ▼
                    serialize()
                         │
                         ▼
                      SHA-256
                         │
                         ▼
                     Object ID
                         │
                         ▼
                  ObjectDatabase
                         │
                         ▼
             .mini-git/objects/<id>
```

The staging data flow is:

```text
Working Tree
     │
     ▼
FileReader
     │
     ▼
Blob
     │
     ▼
ObjectDatabase
     │
     ▼
Blob Object ID
     │
     ▼
Index
     │
     ▼
.mini-git/index
```

The future complete repository flow will become:

```text
Working Tree
     │
     ▼
    Index
     │
     ▼
    Tree
     │
     ▼
   Commit
     │
     ▼
 Reference
     │
     ▼
    HEAD
```

---

# Current Implementation Status

The current implementation status at the end of Phase 8 is:

| Component                        | Status          |
| -------------------------------- | --------------- |
| Project structure                | Implemented     |
| CMake build system               | Implemented     |
| Git/GitHub integration           | Implemented     |
| Repository initialization        | Implemented     |
| `.mini-git/` directory           | Implemented     |
| `HEAD` initialization            | Implemented     |
| SHA-256 hashing                  | Implemented     |
| Object abstraction               | Implemented     |
| Blob                             | Implemented     |
| Tree                             | Implemented     |
| Commit                           | Implemented     |
| FileReader                       | Implemented     |
| Binary-safe file reading         | Implemented     |
| `Blob::from_file()`              | Implemented     |
| Blob serialization               | Implemented     |
| File → Blob pipeline             | Implemented     |
| File → Object ID pipeline        | Implemented     |
| ObjectDatabase                   | Implemented     |
| Persistent object storage        | Implemented     |
| Object retrieval                 | Implemented     |
| Object existence checking        | Implemented     |
| Duplicate-object detection       | Implemented     |
| `hash-file`                      | Implemented     |
| `hash-object`                    | Implemented     |
| Deterministic Tree serialization | Implemented     |
| TreeBuilder                      | Implemented     |
| Recursive directory traversal    | Implemented     |
| Directory → Tree conversion      | Implemented     |
| Nested Trees                     | Implemented     |
| Empty directory Trees            | Implemented     |
| `.mini-git` exclusion            | Implemented     |
| Index                            | Implemented     |
| Index entries                    | Implemented     |
| Index add/update                 | Implemented     |
| Index persistence                | Implemented     |
| Index loading                    | Implemented     |
| Index saving                     | Implemented     |
| `add <file>`                     | Implemented     |
| Hash tests                       | Implemented     |
| Object tests                     | Implemented     |
| FileReader tests                 | Implemented     |
| Blob tests                       | Implemented     |
| TreeBuilder tests                | Implemented     |
| Index tests                      | Implemented     |
| Repository discovery             | Not implemented |
| `add .`                          | Not implemented |
| Deletion staging                 | Not implemented |
| `status`                         | Not implemented |
| Index → Tree snapshot            | Not implemented |
| Real commit command              | Not implemented |
| `log`                            | Not implemented |
| Reference management             | Not implemented |
| Branch management                | Not implemented |
| Checkout                         | Not implemented |
| Diff                             | Not implemented |
| Merge                            | Not implemented |
| Conflict handling                | Not implemented |
| Tags                             | Not implemented |
| Garbage collection               | Not implemented |
| Repository integrity checking    | Not implemented |
| Performance benchmarking         | Not implemented |

---

# Planned Architecture

The final architecture is expected to evolve toward:

```text
                              CLI
                               │
                               ▼
                        Command Parser
                               │
                               ▼
                         Command Layer
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
          ▼                    ▼                    ▼
    Working Tree             Index             Repository
          │                    │                    │
          │                    │                    ▼
          │                    │              Object Database
          │                    │                    │
          │                    │             ┌──────┼──────┐
          │                    │             ▼      ▼      ▼
          │                    │           Blob    Tree   Commit
          │                    │
          │                    ▼
          │                  Status
          │
          ▼
         Diff
                               │
                               ▼
                          References
                               │
                         ┌─────┴─────┐
                         ▼           ▼
                       HEAD       Branches
                                     │
                                     ▼
                                  Commits
```

The central object pipeline is already established:

```text
Files
  │
  ▼
Blobs
  │
  ▼
Trees
  │
  ▼
Serialization
  │
  ▼
Hashing
  │
  ▼
Object Identifier
  │
  ▼
Object Database
  │
  ▼
Persistent Objects
```

The staging pipeline is now also established:

```text
Working Tree
      │
      ▼
     Blob
      │
      ▼
Object Database
      │
      ▼
 Object ID
      │
      ▼
    Index
```

The next major layer will compare the Working Tree and Index and introduce repository state reporting.

---

# Educational Tools

Additional educational commands are planned:

```text
mini-git inspect
mini-git graph
mini-git explain
mini-git stats
mini-git fsck
```

These commands are intended to make Mini Git's internal behavior visible and easier to understand.

For example:

```bash
mini-git explain add main.cpp
```

could eventually show:

```text
Working Tree
     │
     ▼
Read file
     │
     ▼
Create Blob
     │
     ▼
Serialize Blob
     │
     ▼
Calculate SHA-256
     │
     ▼
Store Object
     │
     ▼
Update Index
```

The educational tools will be introduced as the corresponding internal systems become available.

---

# Design Principles

Mini Git follows several architectural principles.

## 1. Separation of Responsibilities

Each component should have a focused responsibility.

For example:

```text
FileReader
    → reads files


Blob
    → represents file content


Tree
    → represents directory structure


TreeBuilder
    → converts filesystem directories into Trees


Hash
    → calculates object IDs


ObjectDatabase
    → stores and retrieves serialized objects


Index
    → records staged path → object ID mappings


Repository
    → manages repository structure
```

Components should not unnecessarily duplicate each other's responsibilities.

---

## 2. Deterministic Behavior

Given identical input, Mini Git should produce identical results.

For example:

```text
same serialized object
        ↓
same SHA-256
        ↓
same object ID
```

Trees explicitly sort their entries before serialization so that filesystem traversal order does not affect Tree identity.

Therefore:

```text
same directory contents
        ↓
same Tree entries
        ↓
same sorted serialization
        ↓
same Tree object ID
```

This property is fundamental to content-addressable storage.

---

## 3. Binary Safety

Filesystem data must be treated as bytes rather than assuming everything is text.

This is why file reading and object storage use binary mode.

Binary safety applies to:

* FileReader
* Blob serialization
* ObjectDatabase storage
* ObjectDatabase retrieval

---

## 4. Testability

Core functionality should be testable independently.

For example:

```text
Hash

FileReader

Blob

Tree

Commit

ObjectDatabase

TreeBuilder

Index
```

can each be tested without requiring the entire application to run.

Filesystem-based tests additionally use isolated temporary files and directories where appropriate.

---

## 5. Incremental Development

Mini Git is implemented subsystem by subsystem.

Each phase introduces a small amount of functionality while building on previously implemented components.

This reduces complexity and makes failures easier to isolate.

The current progression is:

```text
Files
  ↓
Blobs
  ↓
Object Database
  ↓
Trees
  ↓
Index
  ↓
Status
  ↓
Commits
  ↓
References
  ↓
Branches
```

---

## 6. Explicit Simplification

Where Mini Git differs from real Git, the difference should be intentional and documented.

Examples include:

* SHA-256 as the project's chosen hashing algorithm
* simplified object serialization
* flat object storage
* simplified Tree representation
* simplified Commit representation
* simplified text-based Index format
* recursive Tree construction
* absence of Git-compatible repository formats

The goal is to understand the underlying concepts rather than reproduce Git's complete implementation.

---

# Phase Roadmap

The major development stages are:

```text
Phase 0
Understand Git

Phase 1
Project Setup

Phase 2
Repository Initialization

Phase 3
Hashing

Phase 4
Object Model

Phase 5
Blob Objects and File Integration

Phase 6
Object Database

Phase 7
Trees

Phase 8
Index / Staging

Phase 9
Status

Phase 10
Commits

Phase 11
Log

Phase 12
HEAD and References

Phase 13
Branches

Phase 14
Checkout

Phase 15
Diff

Phase 16
Merge

Phase 17
Conflict Handling

Phase 18
Tags

Phase 19
Garbage Collection

Phase 20
Comprehensive Testing

Phase 21
Robustness and Error Handling

Phase 22
Performance

Phase 23
Architecture and C++ Refactoring

Phase 24
Documentation

Phase 25
Mini Git vs Real Git

Phase 26
Portfolio and Interview Preparation
```

---

# End of Phase 8

At the end of Phase 8, Mini Git has progressed from storing repository objects to maintaining a persistent staging area.

The complete currently implemented pipeline is:

```text
                     Working Tree
                          │
                          ▼
                         File
                          │
                          ▼
                     FileReader
                          │
                          ▼
                        Blob
                          │
                          ▼
                  ObjectDatabase
                          │
                          ▼
                      Object ID
                          │
                          ▼
                        Index
                          │
                          ▼
                  .mini-git/index
```

Mini Git can now:

* initialize a repository
* create repository metadata
* initialize `HEAD`
* read real files
* preserve binary file contents
* represent file contents as Blob objects
* serialize objects
* generate deterministic SHA-256 object identifiers
* persist serialized objects
* retrieve stored objects
* check whether objects exist
* detect and reuse duplicate objects
* construct Trees from real directories
* recursively process nested directories
* create nested Tree objects
* preserve empty directories as Tree objects
* ignore `.mini-git` during Tree construction
* deterministically serialize Tree entries
* persist Trees in the Object Database
* create an Index
* add paths to the Index
* update existing staged paths
* check staged paths
* save the Index to disk
* load the Index from disk
* stage a file with `mini-git add <file>`
* store the staged file's Blob in the Object Database
* associate the staged path with its Blob object ID

The object model now supports:

```text
Directory
    │
    ▼
  Tree
    │
    ├── Blob
    ├── Blob
    └── Tree
          │
          ├── Blob
          └── Blob
```

The staging system now adds:

```text
Working Tree
      │
      ▼
     Blob
      │
      ▼
Object Database
      │
      ▼
  Object ID
      │
      ▼
    Index
```

The Index therefore becomes the bridge between the user's working files and future repository snapshots.

The next major subsystem is **Phase 9 — Status**.

Phase 9 will introduce repository state comparison by examining the relationship between:

```text
HEAD
 │
 ▼
Index
 │
 ▼
Working Tree
```

and will eventually allow Mini Git to report states such as:

```text
untracked files
modified files
staged changes
clean working tree
```

This will transform Mini Git from a system that can **stage files** into a system that can **understand and report repository state**.
