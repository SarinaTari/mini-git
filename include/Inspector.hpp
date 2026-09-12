#pragma once

#include <filesystem>
#include <string>

class Inspector {

public:

    explicit Inspector(
        const std::filesystem::path& git_directory
    );

    std::string inspect(
        const std::string& object_id
    ) const;

private:

    std::filesystem::path git_directory_;

};