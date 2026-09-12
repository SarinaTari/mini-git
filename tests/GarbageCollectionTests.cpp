#include "Blob.hpp"
#include "Commit.hpp"
#include "GarbageCollector.hpp"
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
        / "mini-git-gc-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob reachable_blob(
        "reachable"
    );

    const auto blob_id =
        database.store(reachable_blob);

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
        "GC test"
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

    GarbageCollector collector(
        repository.git_directory()
    );

    const auto report =
        collector.preview();

    assert(
        report.unreachable_objects.find(
            unreachable_id
        ) != report.unreachable_objects.end()
    );

    assert(
        report.unreachable_objects.find(
            blob_id
        ) ==
        report.unreachable_objects.end()
    );

    assert(
        report.reclaimable_bytes > 0
    );

    const auto output =
        collector.render();

    assert(
        output.find(
            "Dry run only"
        ) != std::string::npos
    );

    assert(
        std::filesystem::exists(
            repository.git_directory()
            / "objects"
            / unreachable_id
        )
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Garbage collection tests passed.\n";

    return 0;
}