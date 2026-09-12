#include "Blob.hpp"
#include "Commit.hpp"
#include "IntegrityChecker.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-fsck-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob(
        "fsck content"
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
        "Fsck test"
    );

    const auto commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    IntegrityChecker checker(
        repository.git_directory()
    );

    const auto report =
        checker.check();

    assert(
        report.repository_consistent()
    );

    assert(
        report.total_objects == 3
    );

    assert(
        report.unreachable_objects.empty()
    );

    const std::string output =
        checker.render();

    assert(
        output.find(
            "Repository is structurally consistent."
        ) != std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Fsck tests passed.\n";

    return 0;
}