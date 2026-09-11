#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void test_repository_initialization() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-repository-init-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    assert(
        std::filesystem::exists(
            root / ".mini-git"
        )
    );

    assert(
        std::filesystem::exists(
            root / ".mini-git" / "objects"
        )
    );

    assert(
        std::filesystem::exists(
            root / ".mini-git" / "refs" / "heads"
        )
    );

    assert(
        std::filesystem::exists(
            root / ".mini-git" / "HEAD"
        )
    );

    std::filesystem::remove_all(root);
}

void test_symbolic_head() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-symbolic-head-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    assert(
        repository.head() ==
        "ref: refs/heads/main"
    );

    assert(
        repository.current_branch() ==
        "main"
    );

    assert(
        !repository.is_detached()
    );

    std::filesystem::remove_all(root);
}

void test_head_commit() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-head-commit-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    assert(
        repository.head_commit().empty()
    );

    repository.update_branch(
        "main",
        "abcdef123456"
    );

    assert(
        repository.head_commit() ==
        "abcdef123456"
    );

    std::filesystem::remove_all(root);
}

void test_branch_listing() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-branch-list-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    repository.update_branch(
        "main",
        "commit-main"
    );

    repository.update_branch(
        "feature",
        "commit-feature"
    );

    repository.update_branch(
        "experiment",
        "commit-experiment"
    );

    const std::vector<std::string> branches =
        repository.branches();

    assert(branches.size() == 3);

    assert(branches[0] == "experiment");
    assert(branches[1] == "feature");
    assert(branches[2] == "main");

    std::filesystem::remove_all(root);
}

void test_detached_head() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-detached-head-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    const auto head_path =
        root / ".mini-git" / "HEAD";

    std::ofstream head(head_path);

    assert(head);

    head
        << "abcdef1234567890\n";

    head.close();

    assert(
        repository.is_detached()
    );

    assert(
        repository.current_branch().empty()
    );

    assert(
        repository.head_commit() ==
        "abcdef1234567890"
    );

    std::filesystem::remove_all(root);
}

void test_multiple_branch_updates() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-branch-update-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    repository.update_branch(
        "main",
        "commit-one"
    );

    assert(
        repository.head_commit() ==
        "commit-one"
    );

    repository.update_branch(
        "main",
        "commit-two"
    );

    assert(
        repository.head_commit() ==
        "commit-two"
    );

    std::filesystem::remove_all(root);
}

int main() {
    test_repository_initialization();
    test_symbolic_head();
    test_head_commit();
    test_branch_listing();
    test_detached_head();
    test_multiple_branch_updates();

    std::cout
        << "All Repository tests passed.\n";

    return 0;
}