#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

int main()
{
    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "mini-git-checkout-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    Repository repository(test_directory);

    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Tree tree;

    const std::string tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        "",
        "test",
        "Initial commit"
    );

    const std::string commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    repository.create_branch("feature");

    assert(
        repository.branches().size() == 2
    );

    repository.checkout("feature");

    assert(
        repository.current_branch() == "feature"
    );

    assert(
        repository.head_commit() == commit_id
    );

    repository.checkout("main");

    assert(
        repository.current_branch() == "main"
    );

    assert(
        repository.head_commit() == commit_id
    );

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Checkout tests passed\n";

    return 0;
}
