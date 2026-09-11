#pragma once

#include <filesystem>
#include <string>

class Reference {
public:
    Reference(
        const std::filesystem::path& git_dir,
        std::string name
    );

    bool exists() const;

    std::string read() const;

    void write(
        const std::string& object_id
    ) const;

    const std::string& name() const;

private:
    std::filesystem::path git_dir_;
    std::string name_;

    std::filesystem::path path() const;

    void validate_name() const;
};