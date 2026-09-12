#include "Impact.hpp"

#include "Commit.hpp"
#include "Diff.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Snapshot =
    std::map<std::string, std::string>;

void collect_snapshot(
    ObjectDatabase& database,
    const std::string& tree_id,
    const std::string& prefix,
    Snapshot& snapshot
) {
    const Tree tree =
        Tree::deserialize(
            database.read(tree_id)
        );

    for (const auto& entry :
         tree.entries()) {

        const std::string path =
            prefix.empty()
                ? entry.name
                : prefix + "/" + entry.name;

        if (entry.is_tree) {

            collect_snapshot(
                database,
                entry.object_id,
                path,
                snapshot
            );

        }
        else {
            snapshot[path] =
                entry.object_id;
        }
    }
}

}

Impact::Impact(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Impact::render(
    const std::string& commit_id
) const {

    if (commit_id.empty()) {
        throw std::invalid_argument(
            "Commit ID cannot be empty"
        );
    }

    ObjectDatabase database(
        repository_.git_directory()
    );

    if (!database.exists(commit_id)) {
        throw std::runtime_error(
            "Commit not found: " + commit_id
        );
    }

    const Commit commit =
        Commit::deserialize(
            database.read(commit_id)
        );

    Snapshot current;

    collect_snapshot(
        database,
        commit.tree_id(),
        "",
        current
    );

    Snapshot parent;

    if (!commit.parent_ids().empty()) {

        const std::string parent_id =
            commit.parent_ids().front();

        const Commit parent_commit =
            Commit::deserialize(
                database.read(parent_id)
            );

        collect_snapshot(
            database,
            parent_commit.tree_id(),
            "",
            parent
        );
    }

    std::vector<std::string> added;
    std::vector<std::string> removed;
    std::vector<std::string> modified;

    for (const auto& [path, object_id] :
         current) {

        const auto iterator =
            parent.find(path);

        if (iterator == parent.end()) {
            added.push_back(path);
        }
        else if (
            iterator->second != object_id
        ) {
            modified.push_back(path);
        }
    }

    for (const auto& [path, object_id] :
         parent) {

        (void)object_id;

        if (current.find(path) ==
            current.end()) {

            removed.push_back(path);
        }
    }

    std::sort(
        added.begin(),
        added.end()
    );

    std::sort(
        removed.begin(),
        removed.end()
    );

    std::sort(
        modified.begin(),
        modified.end()
    );

    std::set<std::string> directories;

    auto collect_directories =
        [&directories](
            const std::vector<std::string>& files
        ) {

            for (const auto& file :
                 files) {

                std::string::size_type position =
                    file.find('/');

                while (
                    position !=
                    std::string::npos
                ) {

                    directories.insert(
                        file.substr(
                            0,
                            position
                        )
                    );

                    position =
                        file.find(
                            '/',
                            position + 1
                        );
                }
            }
        };

    collect_directories(added);
    collect_directories(removed);
    collect_directories(modified);

    std::ostringstream output;

    output
        << "Commit Impact\n"
        << "=============\n\n";

    output
        << "Commit:\n"
        << "  "
        << commit_id
        << "\n\n";

    output
        << "Message:\n"
        << "  "
        << commit.message()
        << "\n\n";

    output
        << "Author:\n"
        << "  "
        << commit.author()
        << "\n\n";

    output
        << "Parents:\n";

    if (commit.parent_ids().empty()) {
        output << "  (initial commit)\n";
    }
    else {

        for (const auto& parent_id :
             commit.parent_ids()) {

            output
                << "  "
                << parent_id
                << '\n';
        }
    }

    output
        << "\nSummary:\n"
        << "  Added:     "
        << added.size()
        << "\n"
        << "  Modified:  "
        << modified.size()
        << "\n"
        << "  Removed:   "
        << removed.size()
        << "\n"
        << "  Total:     "
        << (
            added.size() +
            modified.size() +
            removed.size()
        )
        << "\n";

    output << "\nAffected files:\n";

    if (
        added.empty() &&
        modified.empty() &&
        removed.empty()
    ) {
        output << "  (none)\n";
    }

    for (const auto& path : added) {
        output
            << "  + "
            << path
            << '\n';
    }

    for (const auto& path : modified) {
        output
            << "  M "
            << path
            << '\n';
    }

    for (const auto& path : removed) {
        output
            << "  - "
            << path
            << '\n';
    }

    output << "\nAffected directories:\n";

    if (directories.empty()) {
        output << "  (root only)\n";
    }
    else {

        for (const auto& directory :
             directories) {

            output
                << "  "
                << directory
                << '\n';
        }
    }

    return output.str();
}