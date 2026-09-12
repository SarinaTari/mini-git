#include "Blob.hpp"
#include "Commit.hpp"
#include "IntegrityChecker.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-integrity-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob("integrity content");

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
        "Integrity test"
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

    IntegrityReport report =
        checker.check();

    assert(report.total_objects == 3);
    assert(report.valid_objects == 3);
    assert(report.corrupted_objects.empty());
    assert(report.missing_objects.empty());
    assert(report.invalid_references.empty());

    const auto object_path =
        repository.git_directory()
        / "objects"
        / blob_id;

    {
        std::ofstream file(
            object_path,
            std::ios::binary |
            std::ios::trunc
        );

        assert(file);

        file << "corrupted";

        assert(file.good());
    }

    report =
        checker.check();

    assert(
        std::find(
            report.corrupted_objects.begin(),
            report.corrupted_objects.end(),
            blob_id
        ) != report.corrupted_objects.end()
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Integrity tests passed.\n";

    return 0;
}