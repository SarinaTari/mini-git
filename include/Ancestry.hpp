#pragma once

#include <filesystem>
#include <optional>
#include <set>
#include <string>

class Ancestry {

public:

    explicit Ancestry(
        const std::filesystem::path& git_directory
    );

    std::optional<std::string> merge_base(
        const std::string& first,
        const std::string& second
    ) const;

    bool is_ancestor(
        const std::string& ancestor,
        const std::string& descendant
    ) const;

private:

    void collect_ancestors(
        const std::string& commit_id,
        std::set<std::string>& ancestors
    ) const;

    std::filesystem::path git_directory_;

};