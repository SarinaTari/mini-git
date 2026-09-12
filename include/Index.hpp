#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct IndexEntry {
    std::string path;
    std::string object_id;
};

class Index {
public:
    explicit Index(const std::filesystem::path& path);

    void add(IndexEntry entry);

    bool contains(const std::string& path) const;

    const std::vector<IndexEntry>& entries() const;

    void clear();

    void save() const;

    void load();

private:
    std::filesystem::path path_;
    std::vector<IndexEntry> entries_;
};
