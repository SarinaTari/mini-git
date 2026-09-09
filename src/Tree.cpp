#include "Tree.hpp"

#include <algorithm>
#include <sstream>
#include <utility>

void Tree::add_entry(TreeEntry entry) {
    entries_.push_back(std::move(entry));
}

std::string Tree::serialize() const {
    std::vector<TreeEntry> sorted_entries = entries_;

    std::sort(
        sorted_entries.begin(),
        sorted_entries.end(),
        [](const TreeEntry& left, const TreeEntry& right) {
            return left.name < right.name;
        }
    );

    std::ostringstream output;

    for (const auto& entry : sorted_entries) {
        output << (entry.is_tree ? "tree " : "blob ")
               << entry.object_id
               << " "
               << entry.name
               << "\n";
    }

    return output.str();
}