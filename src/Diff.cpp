#include "Diff.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

Diff::Diff(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Diff::working_tree_vs_index() {
    return compare_snapshots(
        index_snapshot(),
        working_tree_snapshot()
    );
}

std::string Diff::index_vs_head() {
    const std::string head =
        repository_.head_commit();

    if (head.empty()) {
        return compare_snapshots(
            {},
            index_snapshot()
        );
    }

    return compare_snapshots(
        commit_snapshot(head),
        index_snapshot()
    );
}

std::string Diff::commit_vs_working_tree(
    const std::string& commit_id
) const {
    if (commit_id.empty()) {
        throw std::invalid_argument(
            "Commit ID cannot be empty"
        );
    }

    return compare_snapshots(
        commit_snapshot(commit_id),
        working_tree_snapshot()
    );
}

std::string Diff::commit_vs_commit(
    const std::string& first_commit,
    const std::string& second_commit
) const {
    if (first_commit.empty() ||
        second_commit.empty()) {
        throw std::invalid_argument(
            "Commit IDs cannot be empty"
        );
    }

    return compare_snapshots(
        commit_snapshot(first_commit),
        commit_snapshot(second_commit)
    );
}

Diff::Snapshot
Diff::working_tree_snapshot() const {
    Snapshot snapshot;

    std::filesystem::recursive_directory_iterator iterator(
        repository_.root()
    );

    const std::filesystem::recursive_directory_iterator end;

    while (iterator != end) {
        const auto& entry = *iterator;

        if (
            entry.is_directory() &&
            entry.path().filename() ==
                ".mini-git"
        ) {
            iterator.disable_recursion_pending();
            ++iterator;
            continue;
        }

        if (!entry.is_regular_file()) {
            ++iterator;
            continue;
        }

        const auto relative =
            std::filesystem::relative(
                entry.path(),
                repository_.root()
            );

        const std::string path =
            relative.generic_string();

        std::ifstream file(
            entry.path(),
            std::ios::binary
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to read file: " +
                entry.path().string()
            );
        }

        std::ostringstream content;

        content << file.rdbuf();

        snapshot[path] =
            content.str();

        ++iterator;
    }

    return snapshot;
}

Diff::Snapshot
Diff::index_snapshot() const {
    Snapshot snapshot;

    Index index(
        repository_.git_directory() / "index"
    );

    index.load();

    ObjectDatabase database(
        repository_.git_directory()
    );

    for (const auto& entry :
         index.entries()) {

        const std::string data =
            database.read(
                entry.object_id
            );

        const Blob blob =
            Blob::deserialize(data);

        snapshot[entry.path] =
            blob.content();
    }

    return snapshot;
}

Diff::Snapshot
Diff::commit_snapshot(
    const std::string& commit_id
) const {
    ObjectDatabase database(
        repository_.git_directory()
    );

    const std::string commit_data =
        database.read(commit_id);

    const Commit commit =
        Commit::deserialize(
            commit_data
        );

    Snapshot snapshot;

    collect_tree_snapshot(
        commit.tree_id(),
        repository_.root(),
        snapshot
    );

    return snapshot;
}

void Diff::collect_tree_snapshot(
    const std::string& tree_id,
    const std::filesystem::path& directory,
    Snapshot& snapshot
) const {
    ObjectDatabase database(
        repository_.git_directory()
    );

    const std::string tree_data =
        database.read(tree_id);

    const Tree tree =
        Tree::deserialize(tree_data);

    for (const auto& entry :
         tree.entries()) {

        const auto path =
            directory / entry.name;

        if (entry.is_tree) {
            collect_tree_snapshot(
                entry.object_id,
                path,
                snapshot
            );

            continue;
        }

        const std::string blob_data =
            database.read(
                entry.object_id
            );

        const Blob blob =
            Blob::deserialize(blob_data);

        const auto relative =
            std::filesystem::relative(
                path,
                repository_.root()
            );

        snapshot[
            relative.generic_string()
        ] = blob.content();
    }
}

std::string Diff::compare_snapshots(
    const Snapshot& old_snapshot,
    const Snapshot& new_snapshot
) const {
    std::ostringstream output;

    std::map<std::string, bool> paths;

    for (const auto& [path, content] :
         old_snapshot) {
        (void)content;
        paths[path] = true;
    }

    for (const auto& [path, content] :
         new_snapshot) {
        (void)content;
        paths[path] = true;
    }

    for (const auto& [path, unused] :
         paths) {
        (void)unused;

        const auto old_it =
            old_snapshot.find(path);

        const auto new_it =
            new_snapshot.find(path);

        const bool exists_old =
            old_it != old_snapshot.end();

        const bool exists_new =
            new_it != new_snapshot.end();

        if (
            exists_old &&
            exists_new &&
            old_it->second == new_it->second
        ) {
            continue;
        }

        const std::string old_content =
            exists_old
                ? old_it->second
                : "";

        const std::string new_content =
            exists_new
                ? new_it->second
                : "";

        output
            << diff_file(
                path,
                old_content,
                new_content
            );
    }

    return output.str();
}

std::string Diff::diff_file(
    const std::string& path,
    const std::string& old_content,
    const std::string& new_content
) const {
    return format_diff(
        path,
        old_content,
        new_content
    );
}

std::vector<std::string>
Diff::split_lines(
    const std::string& content
) {
    std::vector<std::string> lines;

    std::istringstream input(content);

    std::string line;

    while (std::getline(input, line)) {
        lines.push_back(line);
    }

    if (
        !content.empty() &&
        content.back() == '\n'
    ) {
        /*
         * getline naturally omits the empty
         * line after a trailing newline.
         *
         * We intentionally do not add another
         * logical line here.
         */
    }

    return lines;
}

std::string Diff::format_diff(
    const std::string& path,
    const std::string& old_content,
    const std::string& new_content
) {
    const auto old_lines =
        split_lines(old_content);

    const auto new_lines =
        split_lines(new_content);

    const std::size_t old_size =
        old_lines.size();

    const std::size_t new_size =
        new_lines.size();

    std::vector<std::vector<std::size_t>> lcs(
        old_size + 1,
        std::vector<std::size_t>(
            new_size + 1,
            0
        )
    );

    for (std::size_t i = old_size;
         i > 0;
         --i) {

        for (std::size_t j = new_size;
             j > 0;
             --j) {

            if (
                old_lines[i - 1] ==
                new_lines[j - 1]
            ) {
                lcs[i - 1][j - 1] =
                    lcs[i][j] + 1;
            }
            else {
                lcs[i - 1][j - 1] =
                    std::max(
                        lcs[i][j - 1],
                        lcs[i - 1][j]
                    );
            }
        }
    }

    std::vector<std::string> changes;

    std::size_t i = 0;
    std::size_t j = 0;

    while (
        i < old_size &&
        j < new_size
    ) {
        if (
            old_lines[i] ==
            new_lines[j]
        ) {
            changes.push_back(
                " " + old_lines[i]
            );

            ++i;
            ++j;
        }
        else if (
            lcs[i + 1][j] >=
            lcs[i][j + 1]
        ) {
            changes.push_back(
                "-" + old_lines[i]
            );

            ++i;
        }
        else {
            changes.push_back(
                "+" + new_lines[j]
            );

            ++j;
        }
    }

    while (i < old_size) {
        changes.push_back(
            "-" + old_lines[i]
        );

        ++i;
    }

    while (j < new_size) {
        changes.push_back(
            "+" + new_lines[j]
        );

        ++j;
    }

    std::ostringstream output;

    output
        << "diff -- mini-git "
        << path
        << '\n';

    output
        << "--- a/"
        << path
        << '\n';

    output
        << "+++ b/"
        << path
        << '\n';

    output
        << "@@ -1,"
        << old_size
        << " +1,"
        << new_size
        << " @@\n";

    for (const auto& change :
         changes) {

        output
            << change
            << '\n';
    }

    return output.str();
}