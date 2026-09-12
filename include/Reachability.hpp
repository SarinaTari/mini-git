#pragma once

#include <filesystem>
#include <set>
#include <string>

class Reachability {

public:

    explicit Reachability(
        const std::filesystem::path& git_directory
    );

    std::set<std::string> reachable_objects() const;

    std::set<std::string> unreachable_objects() const;

private:

    void visit_commit(
        const std::string& commit_id,
        std::set<std::string>& visited
    ) const;

    void visit_tree(
        const std::string& tree_id,
        std::set<std::string>& visited
    ) const;

    std::filesystem::path git_directory_;

};