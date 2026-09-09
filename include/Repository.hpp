#pragma once

#include <filesystem>

class Repository {
public:
    explicit Repository(const std::filesystem::path& root);

    void initialize();

private:
    std::filesystem::path root_;
    std::filesystem::path git_dir_;
};