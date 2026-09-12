#include "Blob.hpp"
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
        / "mini-git-repository-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    Repository repository(test_directory);

    repository.initialize();

    // --------------------------------------------------------
    // Repository initialization
    // --------------------------------------------------------

    assert(
        std::filesystem::exists(
            repository.git_directory()
            / "HEAD"
        )
    );

    assert(
        repository.current_branch() == "main"
    );

    assert(
        !repository.is_detached_head()
    );

    assert(
        repository.head_commit().empty()
    );

    assert(
        repository.branches().empty()
    );

    // --------------------------------------------------------
    // Create ObjectDatabase
    // --------------------------------------------------------

    ObjectDatabase database(
        repository.git_directory()
    );

    // --------------------------------------------------------
    // Create a real Blob object
    // --------------------------------------------------------

    Blob blob("hello");

    const std::string blob_id =
        database.store(blob);

    assert(
        database.exists(blob_id)
    );

    // --------------------------------------------------------
    // Create a real Tree object
    // --------------------------------------------------------

    Tree tree;

    tree.add_entry(
        TreeEntry{
            "hello.txt",
            blob_id,
            false
        }
    );

    const std::string tree_id =
        database.store(tree);

    assert(
        database.exists(tree_id)
    );

    // --------------------------------------------------------
    // Create a real Commit object
    // --------------------------------------------------------

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const std::string commit_id =
        database.store(commit);

    assert(
        database.exists(commit_id)
    );

    // --------------------------------------------------------
    // Point main at the real commit
    // --------------------------------------------------------

    repository.update_branch(
        "main",
        commit_id
    );

    assert(
        repository.branches().size() == 1
    );

    assert(
        repository.current_branch() == "main"
    );

    assert(
        repository.head_commit() ==
        commit_id
    );

    // --------------------------------------------------------
    // Create feature branch
    // --------------------------------------------------------

    repository.create_branch(
        "feature"
    );

    assert(
        repository.branches().size() == 2
    );

    assert(
        repository.current_branch() == "main"
    );

    assert(
        repository.head_commit() ==
        commit_id
    );

    // --------------------------------------------------------
    // Checkout feature
    // --------------------------------------------------------

    repository.checkout(
        "feature"
    );

    assert(
        repository.current_branch() ==
        "feature"
    );

    assert(
        repository.head_commit() ==
        commit_id
    );

    // --------------------------------------------------------
    // Verify working tree restoration
    // --------------------------------------------------------

    const auto restored_file =
        test_directory / "hello.txt";

    assert(
        std::filesystem::exists(
            restored_file
        )
    );

    std::ifstream file(
        restored_file,
        std::ios::binary
    );

    assert(file);

    std::string content;

    std::getline(
        file,
        content
    );

    assert(
        content == "hello"
    );

    // --------------------------------------------------------
    // Checkout main again
    // --------------------------------------------------------

    repository.checkout(
        "main"
    );

    assert(
        repository.current_branch() == "main"
    );

    assert(
        repository.head_commit() ==
        commit_id
    );

    // --------------------------------------------------------
    // Verify working tree after switching back
    // --------------------------------------------------------

    assert(
        std::filesystem::exists(
            restored_file
        )
    );

    // --------------------------------------------------------
    // Cleanup
    // --------------------------------------------------------

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Repository tests passed\n";

    return 0;
}
