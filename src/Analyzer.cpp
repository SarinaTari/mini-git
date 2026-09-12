#include "Analyzer.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

struct CommitInfo {

    std::string id;

    std::string message;

    std::string author;

    std::vector<std::string> parents;

};

void collect_commits(
    ObjectDatabase& database,
    const std::string& start,
    std::map<std::string, CommitInfo>& commits
) {
    if (start.empty()) {
        return;
    }

    std::vector<std::string> pending;
    pending.push_back(start);

    while (!pending.empty()) {

        const std::string id =
            pending.back();

        pending.pop_back();

        if (id.empty()) {
            continue;
        }

        if (commits.find(id) != commits.end()) {
            continue;
        }

        if (!database.exists(id)) {
            continue;
        }

        const Commit commit =
            Commit::deserialize(
                database.read(id)
            );

        commits.emplace(
            id,
            CommitInfo{
                id,
                commit.message(),
                commit.author(),
                commit.parent_ids()
            }
        );

        for (const auto& parent :
             commit.parent_ids()) {

            pending.push_back(parent);
        }
    }
}

void collect_tree_files(
    ObjectDatabase& database,
    const std::string& tree_id,
    const std::string& prefix,
    std::set<std::string>& files
) {
    if (tree_id.empty()) {
        return;
    }

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

            collect_tree_files(
                database,
                entry.object_id,
                path,
                files
            );

        }
        else {
            files.insert(path);
        }
    }
}

}

Analyzer::Analyzer(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Analyzer::render() const {

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::map<std::string, CommitInfo> commits;

    const auto branches =
        repository_.branches();

    for (const auto& branch :
         branches) {

        Reference reference(
            repository_.git_directory(),
            "refs/heads/" + branch
        );

        if (!reference.exists()) {
            continue;
        }

        collect_commits(
            database,
            reference.read(),
            commits
        );
    }

    for (const auto& tag :
         repository_.tags()) {

        Reference reference(
            repository_.git_directory(),
            "refs/tags/" + tag
        );

        if (!reference.exists()) {
            continue;
        }

        collect_commits(
            database,
            reference.read(),
            commits
        );
    }

    const std::string head =
        repository_.head_commit();

    collect_commits(
        database,
        head,
        commits
    );

    std::size_t merge_commits = 0;

    std::map<std::string, std::size_t>
        author_counts;

    for (const auto& [id, commit] :
         commits) {

        if (commit.parents.size() > 1) {
            ++merge_commits;
        }

        ++author_counts[
            commit.author
        ];
    }

    std::ostringstream output;

    output
        << "Mini Git Repository Analysis\n"
        << "============================\n\n";

    output
        << "Repository:\n"
        << "  Root:       "
        << repository_.root()
        << "\n"
        << "  Git dir:    "
        << repository_.git_directory()
        << "\n\n";

    output
        << "History:\n"
        << "  Commits:    "
        << commits.size()
        << "\n"
        << "  Merge commits: "
        << merge_commits
        << "\n";

    output
        << "  Branches:   "
        << branches.size()
        << "\n"
        << "  Tags:       "
        << repository_.tags().size()
        << "\n\n";

    output
        << "Authors:\n";

    if (author_counts.empty()) {
        output
            << "  (none)\n";
    }
    else {

        std::vector<
            std::pair<std::string, std::size_t>
        > authors(
            author_counts.begin(),
            author_counts.end()
        );

        std::sort(
            authors.begin(),
            authors.end(),
            [](const auto& left,
               const auto& right) {

                if (left.second !=
                    right.second) {

                    return left.second >
                           right.second;
                }

                return left.first <
                       right.first;
            }
        );

        for (const auto& [author, count] :
             authors) {

            output
                << "  "
                << author
                << ": "
                << count
                << " commit";

            if (count != 1) {
                output << 's';
            }

            output << '\n';
        }
    }

    output << "\nBranches:\n";

    if (branches.empty()) {
        output << "  (none)\n";
    }
    else {

        for (const auto& branch :
             branches) {

            Reference reference(
                repository_.git_directory(),
                "refs/heads/" + branch
            );

            output
                << "  "
                << branch;

            if (
                !head.empty() &&
                reference.exists() &&
                reference.read() == head
            ) {
                output << " (HEAD)";
            }

            output << '\n';
        }
    }

    return output.str();
}