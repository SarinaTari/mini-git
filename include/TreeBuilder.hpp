#pragma once

#include "Index.hpp"
#include "ObjectDatabase.hpp"

#include <filesystem>
#include <string>

class TreeBuilder {
public:
    explicit TreeBuilder(
        ObjectDatabase& database
    );

    std::string build(
        const std::filesystem::path& directory
    );

    std::string build_from_index(
        const Index& index,
        const std::filesystem::path& root
    );

private:
    ObjectDatabase& database_;
};