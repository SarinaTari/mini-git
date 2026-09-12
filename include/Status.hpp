#pragma once

#include "Index.hpp"

#include <filesystem>
#include <string>
#include <vector>

struct StatusResult {
    std::vector<std::string> modified;
    std::vector<std::string> deleted;
    std::vector<std::string> untracked;

    bool merge_in_progress = false;

    std::vector<std::string> conflicts;
};

class Status {
public:
    Status(
        const std::filesystem::path& root,
        Index& index
    );

    StatusResult collect();

private:
    void collect_modified(
        StatusResult& result
    );

    void collect_untracked(
        StatusResult& result
    );

    void collect_merge_state(
        StatusResult& result
    );

    std::filesystem::path root_;
    Index& index_;
};
