#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

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

    {
        std::ofstream file(
            test_directory / "file.txt"
        );

        file << "hello\n";
    }

    /*
     * No commit yet.
     * Branch creation must fail.
     */
    bool failed = false;

    try {
        repository.create_branch("feature");
    }
    catch (...) {
        failed = true;
    }

    assert(failed);

    /*
     * Create a minimal first commit by directly
     * creating a reference target for this unit test.
     */
    repository.update_branch(
        "main",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    );

    repository.create_branch("feature");

    assert(
        repository.branches().size() == 2
    );

    assert(
        repository.branches()[0] == "feature"
    );

    assert(
        repository.branches()[1] == "main"
    );

    assert(
        repository.current_branch() == "main"
    );

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Branch tests passed\n";

    return 0;
}
