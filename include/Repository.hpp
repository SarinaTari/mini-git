#pragma once

#include "Reference.hpp"
#include "Index.hpp"

#include <filesystem>
#include <string>
#include <vector>

class Repository {
public:
    explicit Repository(
        const std::filesystem::path& root
    );

    void initialize();

    const std::filesystem::path& root() const;

    const std::filesystem::path& git_directory() const;

    std::string head_reference() const;

    bool is_detached_head() const;

    std::string current_branch() const;

    std::string head_commit() const;

    std::vector<std::string> branches() const;

    void update_branch(
        const std::string& branch,
        const std::string& commit_id
    );

    void create_branch(
        const std::string& branch
    );

    void checkout(
        const std::string& branch
    );

    void restore_commit(
        const std::string& commit_id
    ) const;

private:
    std::string read_head() const;

    void write_head(
        const std::string& content
    ) const;

    void checkout_tree(
        const std::string& tree_id
    ) const;

    void checkout_tree_recursive(
        const std::string& tree_id,
        const std::filesystem::path& directory
    ) const;

    void verify_checkout_is_safe(
        const std::string& target_commit
    ) const;

    void remove_working_tree_files() const;

    void rebuild_index_from_tree(
        const std::string& tree_id
    ) const;

    void add_tree_to_index(
        const std::string& tree_id,
        const std::filesystem::path& relative_directory,
        Index& index
    ) const;

    std::filesystem::path root_;
    std::filesystem::path git_dir_;
};