#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

int main()
{
    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "mini-git-branch-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    Repository repository(test_directory);

    repository.initialize();

    bool failed = false;

    try {
        repository.create_branch("feature");
    }
    catch (...) {
        failed = true;
    }

    assert(failed);

    repository.update_branch(
        "main",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    );

    repository.create_branch("feature");

    const auto branches =
        repository.branches();

    assert(branches.size() == 2);
    assert(branches[0] == "feature");
    assert(branches[1] == "main");

    assert(
        repository.current_branch() == "main"
    );

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Branch tests passed.\n";

    return 0;
}