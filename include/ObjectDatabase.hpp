#pragma once

#include "Object.hpp"

#include <filesystem>
#include <string>
#include <vector>

class ObjectDatabase {

public:

    explicit ObjectDatabase(
        const std::filesystem::path& git_dir
    );

    std::string store(
        const Object& object
    );

    bool exists(
        const std::string& object_id
    ) const;

    std::string read(
        const std::string& object_id
    ) const;

    std::vector<std::string> object_ids() const;

private:

    std::filesystem::path objects_dir_;

};