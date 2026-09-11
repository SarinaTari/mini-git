#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

class Repository;
class ObjectDatabase;

class Merge {
public:
    explicit Merge(
        Repository& repository
    );

    std::string merge(
        const std::string& branch,
        const std::string& author,
        const std::string& message = ""
    );

private:
    using Snapshot =
        std::map<std::string, std::string>;

    using OptionalContent =
        std::optional<std::string>;

    Snapshot working_tree_snapshot() const;

    Snapshot index_snapshot() const;

    Snapshot commit_snapshot(
        const std::string& commit_id
    ) const;

    void collect_tree_snapshot(
        ObjectDatabase& database,
        const std::string& tree_id,
        const std::filesystem::path& prefix,
        Snapshot& snapshot
    ) const;

    std::string blob_content(
        ObjectDatabase& database,
        const std::string& object_id
    ) const;

    std::string find_merge_base(
        const std::string& current_commit,
        const std::string& target_commit
    ) const;

    bool is_ancestor(
        const std::string& ancestor,
        const std::string& descendant
    ) const;

    std::vector<std::string> ancestors(
        const std::string& commit_id
    ) const;

    Snapshot three_way_merge(
        const Snapshot& base,
        const Snapshot& current,
        const Snapshot& target,
        std::vector<std::string>& conflicts
    ) const;

    void ensure_clean_working_tree(
        const std::string& current_commit
    ) const;

    void write_snapshot(
        const Snapshot& snapshot
    ) const;

    void synchronize_index(
        const std::string& commit_id
    ) const;

    void synchronize_index(
        const std::map<std::string, std::string>& files
    ) const;

    Repository& repository_;
};