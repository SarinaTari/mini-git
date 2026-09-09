#pragma once

#include <filesystem>

class Repository {
public:
    explicit Repository(const std::filesystem::path& root);

    void initialize();

    const std::filesystem::path& git_directory() const;

private:
    std::filesystem::path root_;
    std::filesystem::path git_dir_;
};