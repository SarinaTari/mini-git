#include "Inspector.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "ObjectType.hpp"
#include "Tree.hpp"

#include <sstream>
#include <stdexcept>

Inspector::Inspector(
    const std::filesystem::path& git_directory
)
    : git_directory_(git_directory) {
}

std::string Inspector::inspect(
    const std::string& object_id
) const {

    if (object_id.empty()) {
        throw std::invalid_argument(
            "Object ID cannot be empty"
        );
    }

    ObjectDatabase database(
        git_directory_
    );

    if (!database.exists(object_id)) {
        throw std::runtime_error(
            "Object not found: " + object_id
        );
    }

    const std::string data =
        database.read(object_id);

    const ObjectType type =
        detect_object_type(data);

    std::ostringstream output;

    output
        << "Object: "
        << object_id
        << "\n";

    output
        << "Type: "
        << object_type_name(type)
        << "\n\n";

    switch (type) {

        case ObjectType::Blob: {

            const Blob blob =
                Blob::deserialize(data);

            output
                << "Size: "
                << blob.content().size()
                << " bytes\n\n";

            output << "Content:\n";
            output << blob.content();

            if (
                blob.content().empty()
                || blob.content().back() != '\n'
            ) {
                output << '\n';
            }

            break;
        }

        case ObjectType::Tree: {

            const Tree tree =
                Tree::deserialize(data);

            output << "Entries:\n";

            for (
                const auto& entry :
                tree.entries()
            ) {

                output
                    << "  "
                    << (
                        entry.is_tree
                            ? "tree"
                            : "blob"
                    )
                    << "  "
                    << entry.object_id
                    << "  "
                    << entry.name
                    << '\n';
            }

            break;
        }

        case ObjectType::Commit: {

            const Commit commit =
                Commit::deserialize(data);

            output
                << "Tree:\n"
                << "  "
                << commit.tree_id()
                << "\n\n";

            output << "Parents:\n";

            if (commit.parent_ids().empty()) {
                output << "  (none)\n";
            }
            else {
                for (
                    const auto& parent :
                    commit.parent_ids()
                ) {

                    output
                        << "  "
                        << parent
                        << '\n';
                }
            }

            output
                << "\nAuthor:\n"
                << "  "
                << commit.author()
                << "\n\n";

            output
                << "Message:\n"
                << "  "
                << commit.message()
                << '\n';

            break;
        }
    }

    return output.str();
}