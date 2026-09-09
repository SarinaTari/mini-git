#include "Repository.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

Repository::Repository(
    const std::filesystem::path& root
)
    : root_(root),
      git_dir_(root / ".mini-git") {
}

void Repository::initialize() {
    if (std::filesystem::exists(git_dir_)) {
        throw std::runtime_error(
            "Repository already exists: " +
            git_dir_.string()
        );
    }

    std::filesystem::create_directories(
        git_dir_ / "objects"
    );

    std::filesystem::create_directories(
        git_dir_ / "refs" / "heads"
    );

    std::ofstream head(
        git_dir_ / "HEAD"
    );

    if (!head) {
        throw std::runtime_error(
            "Failed to create HEAD"
        );
    }

    head << "ref: refs/heads/main\n";
}

const std::filesystem::path&
Repository::git_directory() const {
    return git_dir_;
}

std::string Repository::current_branch() const {
    const std::filesystem::path head_path =
        git_dir_ / "HEAD";

    std::ifstream head(head_path);

    if (!head) {
        throw std::runtime_error(
            "Failed to read HEAD"
        );
    }

    std::string line;

    if (!std::getline(head, line)) {
        throw std::runtime_error(
            "HEAD is empty"
        );
    }

    const std::string prefix =
        "ref: refs/heads/";

    if (line.rfind(prefix, 0) != 0) {
        throw std::runtime_error(
            "Unsupported HEAD format"
        );
    }

    return line.substr(prefix.size());
}

std::string Repository::head_commit() const {
    const std::string branch =
        current_branch();

    const std::filesystem::path branch_path =
        git_dir_ /
        "refs" /
        "heads" /
        branch;

    if (!std::filesystem::exists(branch_path)) {
        return "";
    }

    std::ifstream branch_file(branch_path);

    if (!branch_file) {
        throw std::runtime_error(
            "Failed to read branch reference: " +
            branch
        );
    }

    std::string commit_id;

    if (!std::getline(branch_file, commit_id)) {
        return "";
    }

    return commit_id;
}

void Repository::update_branch(
    const std::string& branch,
    const std::string& commit_id
) {
    const std::filesystem::path branch_path =
        git_dir_ /
        "refs" /
        "heads" /
        branch;

    std::filesystem::create_directories(
        branch_path.parent_path()
    );

    std::ofstream branch_file(
        branch_path,
        std::ios::trunc
    );

    if (!branch_file) {
        throw std::runtime_error(
            "Failed to update branch: " +
            branch
        );
    }

    branch_file << commit_id << '\n';
}