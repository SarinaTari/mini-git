#include "Tree.hpp"

#include <sstream>
#include <utility>

void Tree::add_entry(TreeEntry entry) {
    entries_.push_back(std::move(entry));
}

std::string Tree::serialize() const {
    std::ostringstream output;

    for (const auto& entry : entries_) {
        output << (entry.is_tree ? "tree " : "blob ")
               << entry.object_id
               << " "
               << entry.name
               << "\n";
    }

    return output.str();
}