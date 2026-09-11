#include "Commit.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

Commit::Commit(
    std::string tree_id,
    std::string parent_id,
    std::string author,
    std::string message
)
    : tree_id_(std::move(tree_id)),
      author_(std::move(author)),
      message_(std::move(message)) {

    if (!parent_id.empty()) {
        parent_ids_.push_back(
            std::move(parent_id)
        );
    }
}

Commit::Commit(
    std::string tree_id,
    std::vector<std::string> parent_ids,
    std::string author,
    std::string message
)
    : tree_id_(std::move(tree_id)),
      parent_ids_(std::move(parent_ids)),
      author_(std::move(author)),
      message_(std::move(message)) {
}

std::string Commit::serialize() const {
    std::ostringstream output;

    output
        << "tree "
        << tree_id_
        << "\n";

    for (const auto& parent :
         parent_ids_) {

        if (!parent.empty()) {
            output
                << "parent "
                << parent
                << "\n";
        }
    }

    output
        << "author "
        << author_
        << "\n";

    output << "\n";

    output
        << message_
        << "\n";

    return output.str();
}

Commit Commit::deserialize(
    const std::string& data
) {
    std::istringstream input(data);

    std::string line;

    std::string tree_id;
    std::vector<std::string> parent_ids;
    std::string author;
    std::string message;

    bool reading_message = false;

    while (std::getline(input, line)) {
        if (!reading_message) {
            if (line.empty()) {
                reading_message = true;
                continue;
            }

            if (line.rfind("tree ", 0) == 0) {
                tree_id =
                    line.substr(5);

                continue;
            }

            if (line.rfind("parent ", 0) == 0) {
                const std::string parent =
                    line.substr(7);

                if (!parent.empty()) {
                    parent_ids.push_back(parent);
                }

                continue;
            }

            if (line.rfind("author ", 0) == 0) {
                author =
                    line.substr(7);

                continue;
            }

            throw std::runtime_error(
                "Invalid commit header: " +
                line
            );
        }

        if (!message.empty()) {
            message += '\n';
        }

        message += line;
    }

    if (tree_id.empty()) {
        throw std::runtime_error(
            "Commit is missing tree"
        );
    }

    if (author.empty()) {
        throw std::runtime_error(
            "Commit is missing author"
        );
    }

    return Commit(
        tree_id,
        parent_ids,
        author,
        message
    );
}

const std::string& Commit::tree_id() const {
    return tree_id_;
}

const std::string& Commit::parent_id() const {
    static const std::string empty;

    if (parent_ids_.empty()) {
        return empty;
    }

    return parent_ids_.front();
}

const std::vector<std::string>&
Commit::parent_ids() const {
    return parent_ids_;
}

const std::string& Commit::author() const {
    return author_;
}

const std::string& Commit::message() const {
    return message_;
}