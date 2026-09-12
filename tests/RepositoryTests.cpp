#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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

    assert(
        std::filesystem::exists(
            repository.git_directory() / "HEAD"
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

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob("hello");

    const std::string blob_id =
        database.store(blob);

    assert(database.exists(blob_id));

    Tree tree;

    tree.add_entry({
        "hello.txt",
        blob_id,
        false
    });

    const std::string tree_id =
        database.store(tree);

    assert(database.exists(tree_id));

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const std::string commit_id =
        database.store(commit);

    assert(database.exists(commit_id));

    repository.update_branch(
        "main",
        commit_id
    );

    assert(repository.branches().size() == 1);
    assert(repository.current_branch() == "main");
    assert(repository.head_commit() == commit_id);

    repository.create_branch("feature");

    assert(repository.branches().size() == 2);
    assert(repository.current_branch() == "main");
    assert(repository.head_commit() == commit_id);

    repository.checkout("feature");

    assert(repository.current_branch() == "feature");
    assert(repository.head_commit() == commit_id);

    const auto restored_file =
        test_directory / "hello.txt";

    assert(
        std::filesystem::exists(restored_file)
    );

    std::ifstream file(
        restored_file,
        std::ios::binary
    );

    assert(file);

    std::string content;

    std::getline(file, content);

    assert(content == "hello");

    repository.checkout("main");

    assert(repository.current_branch() == "main");
    assert(repository.head_commit() == commit_id);

    assert(
        std::filesystem::exists(restored_file)
    );

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Repository tests passed.\n";

    return 0;
}