#include "Commit.hpp"

#include <sstream>
#include <utility>

Commit::Commit(
    std::string tree_id,
    std::string parent_id,
    std::string author,
    std::string message
)
    : tree_id_(std::move(tree_id)),
      parent_id_(std::move(parent_id)),
      author_(std::move(author)),
      message_(std::move(message)) {
}

std::string Commit::serialize() const {
    std::ostringstream output;

    output << "tree "
           << tree_id_
           << '\n';

    if (!parent_id_.empty()) {
        output << "parent "
               << parent_id_
               << '\n';
    }

    output << "author "
           << author_
           << '\n';

    output << '\n';

    output << message_
           << '\n';

    return output.str();
}