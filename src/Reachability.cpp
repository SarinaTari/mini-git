#include "Reachability.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <stdexcept>

Reachability::Reachability(
    const std::filesystem::path& git_directory
)
    : git_directory_(git_directory) {
}

void Reachability::visit_commit(
    const std::string& commit_id,
    std::set<std::string>& visited
) const
{
    if (commit_id.empty()) {
        return;
    }

    if (!visited.insert(commit_id).second) {
        return;
    }

    ObjectDatabase database(
        git_directory_
    );

    if (!database.exists(commit_id)) {
        return;
    }

    const Commit commit =
        Commit::deserialize(
            database.read(commit_id)
        );

    visit_tree(
        commit.tree_id(),
        visited
    );

    for (const auto& parent :
         commit.parent_ids()) {

        visit_commit(
            parent,
            visited
        );
    }
}

void Reachability::visit_tree(
    const std::string& tree_id,
    std::set<std::string>& visited
) const
{
    if (tree_id.empty()) {
        return;
    }

    if (!visited.insert(tree_id).second) {
        return;
    }

    ObjectDatabase database(
        git_directory_
    );

    if (!database.exists(tree_id)) {
        return;
    }

    const Tree tree =
        Tree::deserialize(
            database.read(tree_id)
        );

    for (const auto& entry :
         tree.entries()) {

        if (entry.object_id.empty()) {
            continue;
        }

        if (entry.is_tree) {
            visit_tree(
                entry.object_id,
                visited
            );
        }
        else {
            visited.insert(
                entry.object_id
            );
        }
    }
}

std::set<std::string>
Reachability::reachable_objects() const
{
    std::set<std::string> reachable;

    Repository repository(
        git_directory_.parent_path()
    );

    const std::string head =
        repository.head_commit();

    if (!head.empty()) {
        visit_commit(
            head,
            reachable
        );
    }

    for (const auto& branch :
         repository.branches()) {

        Reference reference(
            git_directory_,
            "refs/heads/" + branch
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string commit =
            reference.read();

        if (!commit.empty()) {
            visit_commit(
                commit,
                reachable
            );
        }
    }

    for (const auto& tag :
         repository.tags()) {

        Reference reference(
            git_directory_,
            "refs/tags/" + tag
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string commit =
            reference.read();

        if (!commit.empty()) {
            visit_commit(
                commit,
                reachable
            );
        }
    }

    return reachable;
}

std::set<std::string>
Reachability::unreachable_objects() const
{
    ObjectDatabase database(
        git_directory_
    );

    const auto reachable =
        reachable_objects();

    std::set<std::string> unreachable;

    for (const auto& object_id :
         database.object_ids()) {

        if (
            reachable.find(object_id) ==
            reachable.end()
        ) {
            unreachable.insert(object_id);
        }
    }

    return unreachable;
}