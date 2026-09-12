#include "Tree.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

void Tree::add_entry(
    TreeEntry entry
)
{
    entries_.push_back(
        std::move(entry)
    );
}

const std::vector<TreeEntry>&
Tree::entries() const
{
    return entries_;
}

Tree Tree::deserialize(
    const std::string& data
)
{
    Tree tree;

    std::istringstream input(data);

    std::string line;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream entry_stream(line);

        std::string type;
        std::string object_id;
        std::string name;
        std::string extra;

        if (
            !(entry_stream
                >> type
                >> object_id
                >> name)
        ) {
            throw std::runtime_error(
                "Invalid tree entry"
            );
        }

        if (entry_stream >> extra) {
            throw std::runtime_error(
                "Invalid tree entry: unexpected data"
            );
        }

        if (
            type != "blob" &&
            type != "tree"
        ) {
            throw std::runtime_error(
                "Invalid tree entry type: " +
                type
            );
        }

        if (object_id.empty()) {
            throw std::runtime_error(
                "Tree entry has empty object ID"
            );
        }

        if (name.empty()) {
            throw std::runtime_error(
                "Tree entry has empty name"
            );
        }

        tree.add_entry({
            name,
            object_id,
            type == "tree"
        });
    }

    if (input.bad()) {
        throw std::runtime_error(
            "Failed to read tree object"
        );
    }

    return tree;
}

std::string Tree::serialize() const
{
    std::vector<TreeEntry> sorted_entries =
        entries_;

    std::sort(
        sorted_entries.begin(),
        sorted_entries.end(),
        [](const TreeEntry& left,
           const TreeEntry& right) {
            return left.name < right.name;
        }
    );

    std::ostringstream output;

    for (const auto& entry :
         sorted_entries) {
        output
            << (
                entry.is_tree
                    ? "tree "
                    : "blob "
            )
            << entry.object_id
            << " "
            << entry.name
            << "\n";
    }

    return output.str();
}