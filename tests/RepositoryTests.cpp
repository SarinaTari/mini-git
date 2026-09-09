#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

void test_initial_repository() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-repository-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    assert(
        repository.current_branch() ==
        "main"
    );

    assert(
        repository.head_commit().empty()
    );

    std::filesystem::remove_all(root);
}

void test_update_branch() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-branch-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    repository.update_branch(
        "main",
        "abc123"
    );

    assert(
        repository.head_commit() ==
        "abc123"
    );

    repository.update_branch(
        "main",
        "def456"
    );

    assert(
        repository.head_commit() ==
        "def456"
    );

    std::filesystem::remove_all(root);
}

int main() {
    test_initial_repository();
    test_update_branch();

    std::cout
        << "All Repository tests passed.\n";

    return 0;
}