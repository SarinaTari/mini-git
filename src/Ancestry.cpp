#include "Ancestry.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"

#include <queue>
#include <stdexcept>

Ancestry::Ancestry(
    const std::filesystem::path& git_directory
)
    : git_directory_(git_directory) {
}

void Ancestry::collect_ancestors(
    const std::string& commit_id,
    std::set<std::string>& ancestors
) const
{
    if (commit_id.empty()) {
        return;
    }

    if (!ancestors.insert(commit_id).second) {
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

    for (const auto& parent :
         commit.parent_ids()) {

        collect_ancestors(
            parent,
            ancestors
        );
    }
}

bool Ancestry::is_ancestor(
    const std::string& ancestor,
    const std::string& descendant
) const
{
    if (
        ancestor.empty() ||
        descendant.empty()
    ) {
        return false;
    }

    if (ancestor == descendant) {
        return true;
    }

    ObjectDatabase database(
        git_directory_
    );

    if (
        !database.exists(ancestor) ||
        !database.exists(descendant)
    ) {
        return false;
    }

    std::queue<std::string> pending;

    pending.push(descendant);

    std::set<std::string> visited;

    while (!pending.empty()) {

        const std::string current =
            pending.front();

        pending.pop();

        if (!visited.insert(current).second) {
            continue;
        }

        if (current == ancestor) {
            return true;
        }

        const Commit commit =
            Commit::deserialize(
                database.read(current)
            );

        for (const auto& parent :
             commit.parent_ids()) {

            if (!parent.empty()) {
                pending.push(parent);
            }
        }
    }

    return false;
}

std::optional<std::string>
Ancestry::merge_base(
    const std::string& first,
    const std::string& second
) const
{
    if (
        first.empty() ||
        second.empty()
    ) {
        return std::nullopt;
    }

    ObjectDatabase database(
        git_directory_
    );

    if (
        !database.exists(first) ||
        !database.exists(second)
    ) {
        return std::nullopt;
    }

    std::set<std::string> first_ancestors;

    collect_ancestors(
        first,
        first_ancestors
    );

    std::queue<std::string> pending;

    pending.push(second);

    std::set<std::string> visited;

    while (!pending.empty()) {

        const std::string current =
            pending.front();

        pending.pop();

        if (!visited.insert(current).second) {
            continue;
        }

        if (
            first_ancestors.find(current) !=
            first_ancestors.end()
        ) {
            return current;
        }

        const Commit commit =
            Commit::deserialize(
                database.read(current)
            );

        for (const auto& parent :
             commit.parent_ids()) {

            if (!parent.empty()) {
                pending.push(parent);
            }
        }
    }

    return std::nullopt;
}