#include "Blob.hpp"
#include "Commit.hpp"
#include "Impact.hpp"
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
        / "mini-git-impact-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob first_blob("first");

    const std::string first_blob_id =
        database.store(first_blob);

    Tree first_tree;

    first_tree.add_entry({
        "main.cpp",
        first_blob_id,
        false
    });

    const std::string first_tree_id =
        database.store(first_tree);

    Commit first_commit(
        first_tree_id,
        "",
        "Alice",
        "Initial commit"
    );

    const std::string first_commit_id =
        database.store(first_commit);

    Blob second_blob("second");

    const std::string second_blob_id =
        database.store(second_blob);

    Tree second_tree;

    second_tree.add_entry({
        "main.cpp",
        second_blob_id,
        false
    });

    second_tree.add_entry({
        "README.md",
        first_blob_id,
        false
    });

    const std::string second_tree_id =
        database.store(second_tree);

    Commit second_commit(
        second_tree_id,
        first_commit_id,
        "Alice",
        "Update project"
    );

    const std::string second_commit_id =
        database.store(second_commit);

    repository.update_branch(
        "main",
        second_commit_id
    );

    Impact impact(repository);

    const std::string output =
        impact.render(second_commit_id);

    assert(
        output.find("Update project") !=
        std::string::npos
    );

    assert(
        output.find("main.cpp") !=
        std::string::npos
    );

    assert(
        output.find("README.md") !=
        std::string::npos
    );

    assert(
        output.find("Modified:") !=
        std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Impact tests passed.\n";

    return 0;
}