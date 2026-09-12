#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectType.hpp"
#include "Tree.hpp"

#include <cassert>
#include <iostream>

int main() {

    Blob blob(
        "Hello ObjectType\n"
    );

    const auto blob_data =
        blob.serialize();

    assert(
        detect_object_type(blob_data)
        == ObjectType::Blob
    );

    Tree tree;

    tree.add_entry({
        "hello.txt",
        "0123456789abcdef",
        false
    });

    const auto tree_data =
        tree.serialize();

    assert(
        detect_object_type(tree_data)
        == ObjectType::Tree
    );

    Commit commit(
        "0123456789abcdef",
        "",
        "Test Author",
        "Initial commit"
    );

    const auto commit_data =
        commit.serialize();

    assert(
        detect_object_type(commit_data)
        == ObjectType::Commit
    );

    assert(
        object_type_name(ObjectType::Blob)
        == "blob"
    );

    assert(
        object_type_name(ObjectType::Tree)
        == "tree"
    );

    assert(
        object_type_name(ObjectType::Commit)
        == "commit"
    );

    std::cout
        << "ObjectType tests passed.\n";

    return 0;
}