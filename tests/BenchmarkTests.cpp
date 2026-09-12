#include "Benchmark.hpp"
#include "Repository.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-benchmark-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    Benchmark benchmark(repository);

    const auto output =
        benchmark.render();

    assert(
        output.find("Mini Git Benchmark") !=
        std::string::npos
    );

    assert(
        output.find("SHA-256 hashing") !=
        std::string::npos
    );

    assert(
        output.find(
            "Object existence lookup"
        ) != std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Benchmark tests passed.\n";

    return 0;
}