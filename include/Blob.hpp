#pragma once

#include "Object.hpp"

#include <filesystem>
#include <string>

class Blob : public Object {
public:
    explicit Blob(std::string content);

    static Blob from_file(
        const std::filesystem::path& path
    );

    static Blob deserialize(
        const std::string& data
    );

    const std::string& content() const;

    std::string serialize() const override;

private:
    std::string content_;
};