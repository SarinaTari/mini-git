#pragma once

#include "ObjectDatabase.hpp"

#include <filesystem>
#include <string>

class TreeBuilder {
public:
    explicit TreeBuilder(ObjectDatabase& database);

    std::string build(
        const std::filesystem::path& directory
    );

private:
    ObjectDatabase& database_;
};