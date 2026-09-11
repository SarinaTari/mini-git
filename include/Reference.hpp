#pragma once

#include <filesystem>
#include <string>

class Reference {
public:
    Reference(
        const std::filesystem::path& git_directory,
        std::string name
    );

    const std::string& name() const;

    std::filesystem::path path() const;

    bool exists() const;

    std::string read() const;

    void write(const std::string& object_id) const;

private:
    static void validate_name(const std::string& name);

    std::filesystem::path git_directory_;
    std::string name_;
};