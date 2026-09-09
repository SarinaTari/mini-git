#include "Repository.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

Repository::Repository(const std::filesystem::path& root)
    : root_(root),
      git_dir_(root / ".mini-git") {
}

void Repository::initialize() {
    if (std::filesystem::exists(git_dir_)) {
        throw std::runtime_error(
            "Repository already exists: " + git_dir_.string()
        );
    }

    std::filesystem::create_directories(git_dir_ / "objects");
    std::filesystem::create_directories(git_dir_ / "refs" / "heads");

    std::ofstream head(git_dir_ / "HEAD");

    if (!head) {
        throw std::runtime_error("Failed to create HEAD");
    }

    head << "ref: refs/heads/main\n";
}

const std::filesystem::path& Repository::git_directory() const {
    return git_dir_;
}