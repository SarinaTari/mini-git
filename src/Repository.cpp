#include "Repository.hpp"

#include "Reference.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

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

    if (!head) {
        throw std::runtime_error(
            "Failed to write HEAD"
        );
    }
}

const std::filesystem::path&
Repository::git_directory() const {
    return git_dir_;
}

std::string Repository::head() const {
    return read_head();
}

std::string Repository::current_branch() const {
    const std::string head_value =
        read_head();

    const std::string prefix =
        "ref: refs/heads/";

    if (
        head_value.rfind(prefix, 0) != 0
    ) {
        return "";
    }

    const std::string branch =
        head_value.substr(prefix.size());

    if (
        branch.empty() ||
        !is_valid_branch_name(branch)
    ) {
        throw std::runtime_error(
            "Invalid HEAD branch reference"
        );
    }

    return branch;
}

bool Repository::is_detached() const {
    const std::string head_value =
        read_head();

    const std::string prefix =
        "ref: ";

    return head_value.rfind(prefix, 0) != 0;
}

std::string Repository::head_commit() const {
    const std::string head_value =
        read_head();

    const std::string prefix =
        "ref: refs/heads/";

    if (
        head_value.rfind(prefix, 0) != 0
    ) {
        return head_value;
    }

    const std::string branch =
        head_value.substr(prefix.size());

    if (
        branch.empty() ||
        !is_valid_branch_name(branch)
    ) {
        throw std::runtime_error(
            "Invalid HEAD branch reference"
        );
    }

    Reference reference(
        git_dir_,
        "refs/heads/" + branch
    );

    if (!reference.exists()) {
        return "";
    }

    return reference.read();
}

void Repository::update_branch(
    const std::string& branch,
    const std::string& commit_id
) {
    if (!is_valid_branch_name(branch)) {
        throw std::invalid_argument(
            "Invalid branch name: " +
            branch
        );
    }

    Reference reference(
        git_dir_,
        "refs/heads/" + branch
    );

    reference.write(commit_id);
}

std::vector<std::string>
Repository::branches() const {
    const std::filesystem::path heads_directory =
        git_dir_ / "refs" / "heads";

    std::vector<std::string> result;

    if (
        !std::filesystem::exists(heads_directory)
    ) {
        return result;
    }

    for (
        const auto& entry :
        std::filesystem::directory_iterator(
            heads_directory
        )
    ) {
        if (
            entry.is_regular_file()
        ) {
            result.push_back(
                entry.path().filename().string()
            );
        }
    }

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

std::string Repository::read_head() const {
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

    if (line.empty()) {
        throw std::runtime_error(
            "HEAD is empty"
        );
    }

    return line;
}

bool Repository::is_valid_branch_name(
    const std::string& branch
) {
    if (branch.empty()) {
        return false;
    }

    const std::filesystem::path branch_path(
        branch
    );

    if (branch_path.is_absolute()) {
        return false;
    }

    for (const auto& component : branch_path) {
        if (
            component == ".."
        ) {
            return false;
        }
    }

    return branch != "." &&
           branch != "..";
}