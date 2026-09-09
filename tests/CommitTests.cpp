#include "Commit.hpp"

#include <cassert>
#include <iostream>
#include <string>

void test_initial_commit() {
    Commit commit(
        "tree123",
        "",
        "Alice",
        "Initial commit"
    );

    const std::string serialized =
        commit.serialize();

    assert(
        serialized.find(
            "tree tree123\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "author Alice\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "Initial commit\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "parent "
        ) == std::string::npos
    );
}

void test_commit_with_parent() {
    Commit commit(
        "tree456",
        "commit123",
        "Bob",
        "Second commit"
    );

    const std::string serialized =
        commit.serialize();

    assert(
        serialized.find(
            "tree tree456\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "parent commit123\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "author Bob\n"
        ) != std::string::npos
    );

    assert(
        serialized.find(
            "Second commit\n"
        ) != std::string::npos
    );
}

void test_different_commits_have_different_serialization() {
    Commit first(
        "tree123",
        "",
        "Alice",
        "First"
    );

    Commit second(
        "tree123",
        "",
        "Alice",
        "Second"
    );

    assert(
        first.serialize() !=
        second.serialize()
    );
}

int main() {
    test_initial_commit();
    test_commit_with_parent();
    test_different_commits_have_different_serialization();

    std::cout
        << "All Commit tests passed.\n";

    return 0;
}