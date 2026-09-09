#include "Blob.hpp"
#include "Commit.hpp"
#include "Tree.hpp"

#include <cassert>
#include <iostream>
#include <string>

void test_blob() {
    Blob blob("Hello Mini Git!");

    const std::string expected =
        std::string("blob 15\0", 8)
        + "Hello Mini Git!";

    assert(blob.serialize() == expected);
}

void test_tree() {
    Tree tree;

    tree.add_entry({
        "main.cpp",
        "abc123",
        false
    });

    tree.add_entry({
        "src",
        "def456",
        true
    });

    const std::string result = tree.serialize();

    assert(
        result ==
        "blob abc123 main.cpp\n"
        "tree def456 src\n"
    );
}

void test_commit() {
    Commit commit(
        "tree123",
        "parent456",
        "Sarina",
        "Initial commit"
    );

    const std::string result = commit.serialize();

    assert(
        result ==
        "tree tree123\n"
        "parent parent456\n"
        "author Sarina\n"
        "\n"
        "Initial commit\n"
    );
}

void test_initial_commit() {
    Commit commit(
        "tree123",
        "",
        "Sarina",
        "Initial commit"
    );

    const std::string result = commit.serialize();

    assert(
        result ==
        "tree tree123\n"
        "author Sarina\n"
        "\n"
        "Initial commit\n"
    );
}

int main() {
    test_blob();
    test_tree();
    test_commit();
    test_initial_commit();

    std::cout << "All Object tests passed.\n";

    return 0;
}