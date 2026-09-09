#include "TreeBuilder.hpp"

#include "Blob.hpp"
#include "Tree.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

TreeBuilder::TreeBuilder(
    ObjectDatabase& database
)
    : database_(database) {
}

std::string TreeBuilder::build(
    const std::filesystem::path& directory
) {
    if (!std::filesystem::is_directory(directory)) {
        throw std::runtime_error(
            "Not a directory: " +
            directory.string()
        );
    }

    Tree tree;

    for (
        const auto& entry :
        std::filesystem::directory_iterator(directory)
    ) {
        if (
            entry.path().filename() ==
            ".mini-git"
        ) {
            continue;
        }

        if (entry.is_regular_file()) {
            Blob blob =
                Blob::from_file(entry.path());

            const std::string blob_id =
                database_.store(blob);

            tree.add_entry({
                entry.path().filename().string(),
                blob_id,
                false
            });
        }
        else if (entry.is_directory()) {
            const std::string tree_id =
                build(entry.path());

            tree.add_entry({
                entry.path().filename().string(),
                tree_id,
                true
            });
        }
    }

    return database_.store(tree);
}

std::string TreeBuilder::build_from_index(
    const Index& index,
    const std::filesystem::path& root
) {
    struct Node {
        std::map<std::string, std::string> blobs;
        std::map<std::string, Node> directories;
    };

    Node root_node;

    for (const auto& entry : index.entries()) {
        std::filesystem::path relative_path =
            entry.path;

        if (relative_path.is_absolute()) {
            relative_path =
                std::filesystem::relative(
                    relative_path,
                    root
                );
        }

        Node* current =
            &root_node;

        std::vector<std::string> parts;

        for (
            const auto& part :
            relative_path
        ) {
            parts.push_back(
                part.string()
            );
        }

        if (parts.empty()) {
            continue;
        }

        for (
            std::size_t i = 0;
            i + 1 < parts.size();
            ++i
        ) {
            current =
                &current->directories[parts[i]];
        }

        current->blobs[parts.back()] =
            entry.object_id;
    }

    std::function<std::string(const Node&)> build_node;

    build_node =
        [&](const Node& node) -> std::string {
            Tree tree;

            for (const auto& [name, object_id] :
                 node.blobs) {

                tree.add_entry({
                    name,
                    object_id,
                    false
                });
            }

            for (const auto& [name, child] :
                 node.directories) {

                const std::string child_id =
                    build_node(child);

                tree.add_entry({
                    name,
                    child_id,
                    true
                });
            }

            return database_.store(tree);
        };

    return build_node(root_node);
}