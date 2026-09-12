#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Stats.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-stats-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob("hello");

    const auto blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry({
        "hello.txt",
        blob_id,
        false
    });

    const auto tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const auto commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    Stats stats(repository);

    const auto output =
        stats.render();

    assert(
        output.find("Blobs:") !=
        std::string::npos
    );

    assert(
        output.find("Trees:") !=
        std::string::npos
    );

    assert(
        output.find("Commits:") !=
        std::string::npos
    );

    assert(
        output.find("Branches:") !=
        std::string::npos
    );

    assert(
        output.find("Initial commit") ==
        std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Stats tests passed.\n";

    return 0;
}