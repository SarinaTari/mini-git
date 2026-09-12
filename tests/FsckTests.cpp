#include "Blob.hpp"
#include "Commit.hpp"
#include "IntegrityChecker.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

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

    Blob blob("fsck content");

    const std::string blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry({
        "file.txt",
        blob_id,
        false
    });

    const std::string tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Fsck test"
    );

    const std::string commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    IntegrityChecker checker(
        repository.git_directory()
    );

    const IntegrityReport report =
        checker.check();

    assert(
        report.repository_consistent()
    );

    assert(
        report.total_objects == 3
    );

    assert(
        report.valid_objects == 3
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