#include "Blob.hpp"
#include "Commit.hpp"
#include "Tree.hpp"
#include "ObjectDatabase.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <filesystem>

void test_blob() {
    Blob blob("Hello Mini Git");

    const std::string expected =
        std::string("blob 14\0", 8)
        + "Hello Mini Git";

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

void test_object_database() {
    const std::filesystem::path test_root =
        std::filesystem::temp_directory_path() /
        "mini-git-object-db-test";

    std::filesystem::remove_all(test_root);

    const std::filesystem::path git_dir =
        test_root / ".mini-git";

    const std::filesystem::path objects_dir =
        git_dir / "objects";

    std::filesystem::create_directories(objects_dir);

    ObjectDatabase database(git_dir);

    Blob blob("Hello Mini Git");

    const std::string object_id =
        database.store(blob);

    assert(database.exists(object_id));

    const std::string stored_data =
        database.read(object_id);

    assert(stored_data == blob.serialize());

    Blob second_blob("Hello Mini Git");

    const std::string second_id =
        database.store(second_blob);

    assert(second_id == object_id);

    std::filesystem::remove_all(test_root);
}

void test_tree_deterministic_order() {
    Tree first;

    first.add_entry({
        "z.cpp",
        "zzz",
        false
    });

    first.add_entry({
        "README.md",
        "aaa",
        false
    });

    first.add_entry({
        "src",
        "bbb",
        true
    });

    Tree second;

    second.add_entry({
        "src",
        "bbb",
        true
    });

    second.add_entry({
        "z.cpp",
        "zzz",
        false
    });

    second.add_entry({
        "README.md",
        "aaa",
        false
    });

    assert(first.serialize() == second.serialize());

    assert(
        first.serialize() ==
        "blob aaa README.md\n"
        "tree bbb src\n"
        "blob zzz z.cpp\n"
    );
}

int main() {
    test_blob();
    test_tree();
    test_tree_deterministic_order();
    test_commit();
    test_initial_commit();
    test_object_database();

    std::cout << "All Object tests passed.\n";

    return 0;
}