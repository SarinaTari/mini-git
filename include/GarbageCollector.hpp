#pragma once

#include <filesystem>
#include <set>
#include <string>

struct GarbageCollectionReport {

    std::set<std::string> unreachable_objects;

    std::uintmax_t reclaimable_bytes = 0;
};

class GarbageCollector {

public:

    explicit GarbageCollector(
        const std::filesystem::path& git_directory
    );

    GarbageCollectionReport preview() const;

    std::string render() const;

private:

    std::filesystem::path git_directory_;

};