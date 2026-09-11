#pragma once

#include "Object.hpp"

#include <string>
#include <vector>

struct TreeEntry {
    std::string name;
    std::string object_id;
    bool is_tree;
};

class Tree : public Object {
public:
    void add_entry(
        TreeEntry entry
    );

    const std::vector<TreeEntry>& entries() const;

    static Tree deserialize(
        const std::string& data
    );

    std::string serialize() const override;

private:
    std::vector<TreeEntry> entries_;
};