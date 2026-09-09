#pragma once

#include "Object.hpp"

#include <filesystem>
#include <string>

class ObjectDatabase {
public:
    explicit ObjectDatabase(
        const std::filesystem::path& git_dir
    );

    std::string store(const Object& object);

    bool exists(const std::string& object_id) const;

    std::string read(const std::string& object_id) const;

private:
    std::filesystem::path objects_dir_;
};