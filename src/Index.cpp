#include "Index.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

Index::Index(const std::filesystem::path& path)
    : path_(path) {
}

void Index::add(IndexEntry entry) {
    for (auto& existing : entries_) {
        if (existing.path == entry.path) {
            existing.object_id = entry.object_id;
            return;
        }
    }

    entries_.push_back(std::move(entry));
}

bool Index::contains(
    const std::string& path
) const {
    for (const auto& entry : entries_) {
        if (entry.path == path) {
            return true;
        }
    }

    return false;
}

const std::vector<IndexEntry>&
Index::entries() const {
    return entries_;
}

void Index::clear() {
    entries_.clear();
}

void Index::save() const {
    std::ofstream file(path_);

    if (!file) {
        throw std::runtime_error(
            "Failed to write index: " + path_.string()
        );
    }

    for (const auto& entry : entries_) {
        file << entry.path
             << '\t'
             << entry.object_id
             << '\n';
    }
}

void Index::load() {
    entries_.clear();

    if (!std::filesystem::exists(path_)) {
        return;
    }

    std::ifstream file(path_);

    if (!file) {
        throw std::runtime_error(
            "Failed to read index: " + path_.string()
        );
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream input(line);

        std::string path;
        std::string object_id;

        if (!(input >> path >> object_id)) {
            throw std::runtime_error(
                "Invalid index entry: " + line
            );
        }

        entries_.push_back({
            path,
            object_id
        });
    }
}