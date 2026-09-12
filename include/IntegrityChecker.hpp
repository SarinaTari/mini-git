#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct IntegrityReport {

    std::size_t total_objects = 0;

    std::size_t valid_objects = 0;

    std::vector<std::string> corrupted_objects;

    std::vector<std::string> missing_objects;

    std::vector<std::string> invalid_references;

    std::vector<std::string> unreachable_objects;

    bool repository_consistent() const;
};

class IntegrityChecker {

public:

    explicit IntegrityChecker(
        const std::filesystem::path& git_directory
    );

    IntegrityReport check() const;

    std::string render() const;

private:

    bool verify_object(
        const std::string& object_id
    ) const;

    void verify_commit_relationships(
        const std::string& object_id,
        std::vector<std::string>& missing,
        std::vector<std::string>& invalid
    ) const;

    void verify_tree_relationships(
        const std::string& object_id,
        std::vector<std::string>& missing,
        std::vector<std::string>& invalid
    ) const;

    std::filesystem::path git_directory_;

};