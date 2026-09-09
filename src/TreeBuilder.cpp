#include "TreeBuilder.hpp"

#include "Blob.hpp"
#include "Tree.hpp"

#include <filesystem>
#include <stdexcept>

TreeBuilder::TreeBuilder(ObjectDatabase& database)
    : database_(database) {
}

std::string TreeBuilder::build(
    const std::filesystem::path& directory
) {
    if (!std::filesystem::is_directory(directory)) {
        throw std::runtime_error(
            "Not a directory: " + directory.string()
        );
    }

    Tree tree;

    for (const auto& entry :
         std::filesystem::directory_iterator(directory)) {

        // Never include Mini Git's own repository metadata.
        if (entry.path().filename() == ".mini-git") {
            continue;
        }

        if (entry.is_regular_file()) {
            Blob blob = Blob::from_file(entry.path());

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