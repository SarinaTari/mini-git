#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Repository {
public:
    explicit Repository(
        const std::filesystem::path& root
    );

    void initialize();

    const std::filesystem::path& git_directory() const;

    std::string head() const;

    std::string current_branch() const;

    bool is_detached() const;

    std::string head_commit() const;

    void update_branch(
        const std::string& branch,
        const std::string& commit_id
    );

    std::vector<std::string> branches() const;

private:
    std::filesystem::path root_;
    std::filesystem::path git_dir_;

    std::string read_head() const;

    static bool is_valid_branch_name(
        const std::string& branch
    );
};