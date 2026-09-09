#pragma once

#include <filesystem>
#include <string>

class Repository {
public:
    explicit Repository(
        const std::filesystem::path& root
    );

    void initialize();

    const std::filesystem::path& git_directory() const;

    std::string current_branch() const;

    std::string head_commit() const;

    void update_branch(
        const std::string& branch,
        const std::string& commit_id
    );

private:
    std::filesystem::path root_;
    std::filesystem::path git_dir_;
};