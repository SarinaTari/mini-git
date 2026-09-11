#include "Commit.hpp"
#include "Hash.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

void test_commit_deserialization() {
    const std::string serialized =
        "tree tree123\n"
        "parent parent456\n"
        "author Sarina\n"
        "\n"
        "Add feature\n";

    Commit commit =
        Commit::deserialize(serialized);

    assert(commit.tree_id() == "tree123");
    assert(commit.parent_id() == "parent456");
    assert(commit.author() == "Sarina");
    assert(commit.message() == "Add feature");
}

void test_initial_commit_deserialization() {
    const std::string serialized =
        "tree tree123\n"
        "author Sarina\n"
        "\n"
        "Initial commit\n";

    Commit commit =
        Commit::deserialize(serialized);

    assert(commit.tree_id() == "tree123");
    assert(commit.parent_id().empty());
    assert(commit.author() == "Sarina");
    assert(commit.message() == "Initial commit");
}

void test_multiline_message() {
    const std::string serialized =
        "tree tree123\n"
        "parent parent456\n"
        "author Sarina\n"
        "\n"
        "Add feature\n"
        "\n"
        "This commit adds a new feature.\n"
        "It also includes tests.\n";

    Commit commit =
        Commit::deserialize(serialized);

    assert(
        commit.message() ==
        "Add feature\n"
        "\n"
        "This commit adds a new feature.\n"
        "It also includes tests."
    );
}

void test_round_trip() {
    Commit original(
        "tree123",
        "parent456",
        "Sarina",
        "Add feature"
    );

    const std::string serialized =
        original.serialize();

    Commit restored =
        Commit::deserialize(serialized);

    assert(
        restored.serialize() ==
        original.serialize()
    );
}

void test_commit_stored_and_read() {
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() /
        "mini-git-log-object-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    Repository repository(root);

    ObjectDatabase database(
        repository.git_directory()
    );

    Commit commit(
        "tree123",
        "",
        "Sarina",
        "Initial commit"
    );

    const std::string object_id =
        database.store(commit);

    const std::string stored_data =
        database.read(object_id);

    Commit restored =
        Commit::deserialize(stored_data);

    assert(restored.tree_id() == "tree123");
    assert(restored.parent_id().empty());
    assert(restored.author() == "Sarina");
    assert(restored.message() == "Initial commit");

    std::filesystem::remove_all(root);
}

void test_commit_history_chain() {
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() /
        "mini-git-log-history-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git" / "objects"
    );

    Repository repository(root);

    ObjectDatabase database(
        repository.git_directory()
    );

    Commit first(
        "treeA",
        "",
        "Sarina",
        "Initial commit"
    );

    const std::string first_id =
        database.store(first);

    Commit second(
        "treeB",
        first_id,
        "Sarina",
        "Add feature"
    );

    const std::string second_id =
        database.store(second);

    Commit third(
        "treeC",
        second_id,
        "Sarina",
        "Fix bug"
    );

    const std::string third_id =
        database.store(third);

    Commit current =
        Commit::deserialize(
            database.read(third_id)
        );

    assert(current.message() == "Fix bug");
    assert(current.parent_id() == second_id);

    current =
        Commit::deserialize(
            database.read(current.parent_id())
        );

    assert(current.message() == "Add feature");
    assert(current.parent_id() == first_id);

    current =
        Commit::deserialize(
            database.read(current.parent_id())
        );

    assert(current.message() == "Initial commit");
    assert(current.parent_id().empty());

    std::filesystem::remove_all(root);
}

int main() {
    test_commit_deserialization();
    test_initial_commit_deserialization();
    test_multiline_message();
    test_round_trip();
    test_commit_stored_and_read();
    test_commit_history_chain();

    std::cout
        << "All Log tests passed.\n";

    return 0;
}