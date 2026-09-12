#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Reachability.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-reachability-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob(
        "reachable content"
    );

    const auto blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry({
        "file.txt",
        blob_id,
        false
    });

    const auto tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Reachability test"
    );

    const auto commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    Blob unreachable_blob(
        "unreachable"
    );

    const auto unreachable_id =
        database.store(
            unreachable_blob
        );

    Reachability reachability(
        repository.git_directory()
    );

    const auto reachable =
        reachability.reachable_objects();

    const auto unreachable =
        reachability.unreachable_objects();

    assert(
        reachable.find(commit_id) !=
        reachable.end()
    );

    assert(
        reachable.find(tree_id) !=
        reachable.end()
    );

    assert(
        reachable.find(blob_id) !=
        reachable.end()
    );

    assert(
        unreachable.find(unreachable_id) !=
        unreachable.end()
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Reachability tests passed.\n";

    return 0;
}