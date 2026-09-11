#include "Reference.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main()
{
    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "mini-git-reference-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    {
        Reference reference(
            test_directory,
            "refs/heads/main"
        );

        assert(
            reference.name() ==
            "refs/heads/main"
        );

        assert(!reference.exists());

        reference.write(
            "abcdef1234567890"
        );

        assert(reference.exists());

        assert(
            reference.read() ==
            "abcdef1234567890"
        );
    }

    {
        bool failed = false;

        try {
            Reference invalid(
                test_directory,
                "../outside"
            );
        }
        catch (const std::invalid_argument&) {
            failed = true;
        }

        assert(failed);
    }

    {
        bool failed = false;

        try {
            Reference invalid(
                test_directory,
                "/absolute/path"
            );
        }
        catch (const std::invalid_argument&) {
            failed = true;
        }

        assert(failed);
    }

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Reference tests passed\n";

    return 0;
}