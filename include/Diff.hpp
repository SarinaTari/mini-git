#pragma once

#include "Index.hpp"

#include <filesystem>
#include <map>
#include <string>

class Repository;
class ObjectDatabase;

class Diff {
public:
    explicit Diff(
        const Repository& repository
    );

    std::string working_tree_vs_index();

    std::string index_vs_head();

    std::string commit_vs_working_tree(
        const std::string& commit_id
    ) const;

    std::string commit_vs_commit(
        const std::string& first_commit,
        const std::string& second_commit
    ) const;

private:
    using Snapshot =
        std::map<std::string, std::string>;

    Snapshot working_tree_snapshot() const;

    Snapshot index_snapshot() const;

    Snapshot commit_snapshot(
        const std::string& commit_id
    ) const;

    void collect_tree_snapshot(
        const std::string& tree_id,
        const std::filesystem::path& directory,
        Snapshot& snapshot
    ) const;

    std::string compare_snapshots(
        const Snapshot& old_snapshot,
        const Snapshot& new_snapshot
    ) const;

    std::string diff_file(
        const std::string& path,
        const std::string& old_content,
        const std::string& new_content
    ) const;

    static std::string format_diff(
        const std::string& path,
        const std::string& old_content,
        const std::string& new_content
    );

    static std::vector<std::string> split_lines(
        const std::string& content
    );

    const Repository& repository_;
};
