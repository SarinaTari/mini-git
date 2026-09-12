#include "Merge.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "Index.hpp"
#include "ObjectDatabase.hpp"
#include "Reference.hpp"
#include "Repository.hpp"
#include "Tree.hpp"
#include "TreeBuilder.hpp"
#include "Hash.hpp"

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

std::string ensure_trailing_newline(
    const std::string& content
) {
    if (
        !content.empty() &&
        content.back() != '\n'
    ) {
        return content + '\n';
    }

    return content;
}

}

Merge::Merge(
    Repository& repository
)
    : repository_(repository) {
}

Merge::Snapshot
Merge::working_tree_snapshot() const {
    Snapshot snapshot;

    const auto root =
        repository_.root();

    const auto git_directory =
        repository_.git_directory();

    for (
        const auto& entry :
        std::filesystem::recursive_directory_iterator(
            root
        )
    ) {
        if (
            entry.path() == git_directory ||
            entry.path().string().rfind(
                git_directory.string() +
                std::filesystem::path::preferred_separator,
                0
            ) == 0
        ) {
            continue;
        }

        if (!entry.is_regular_file()) {
            continue;
        }

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

        const auto relative =
            std::filesystem::relative(
                entry.path(),
                root
            );

        snapshot[
            relative.generic_string()
        ] = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
    }

    return snapshot;
}

Merge::Snapshot
Merge::index_snapshot() const {
    Snapshot snapshot;

    ObjectDatabase database(
        repository_.git_directory()
    );

    Index index(
        repository_.git_directory() / "index"
    );

    index.load();

    for (const auto& entry :
         index.entries()) {

        snapshot[entry.path] =
            blob_content(
                database,
                entry.object_id
            );
    }

    return snapshot;
}

std::string Merge::blob_content(
    ObjectDatabase& database,
    const std::string& object_id
) const {
    const std::string data =
        database.read(object_id);

    const std::size_t separator =
        data.find('\0');

    if (
        separator ==
        std::string::npos
    ) {
        throw std::runtime_error(
            "Invalid blob object: " +
            object_id
        );
    }

    return data.substr(
        separator + 1
    );
}

Merge::Snapshot
Merge::commit_snapshot(
    const std::string& commit_id
) const {
    Snapshot snapshot;

    if (commit_id.empty()) {
        return snapshot;
    }

    ObjectDatabase database(
        repository_.git_directory()
    );

    const std::string commit_data =
        database.read(commit_id);

    const Commit commit =
        Commit::deserialize(
            commit_data
        );

    collect_tree_snapshot(
        database,
        commit.tree_id(),
        "",
        snapshot
    );

    return snapshot;
}

void Merge::collect_tree_snapshot(
    ObjectDatabase& database,
    const std::string& tree_id,
    const std::filesystem::path& prefix,
    Snapshot& snapshot
) const {
    const std::string data =
        database.read(tree_id);

    const Tree tree =
        Tree::deserialize(data);

    for (const auto& entry :
         tree.entries()) {

        const auto path =
            prefix / entry.name;

        if (entry.is_tree) {
            collect_tree_snapshot(
                database,
                entry.object_id,
                path,
                snapshot
            );

            continue;
        }

        snapshot[
            path.generic_string()
        ] = blob_content(
            database,
            entry.object_id
        );
    }
}

std::vector<std::string>
Merge::ancestors(
    const std::string& commit_id
) const {
    std::vector<std::string> result;

    if (commit_id.empty()) {
        return result;
    }

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::queue<std::string> queue;
    std::unordered_set<std::string> visited;

    queue.push(commit_id);

    while (!queue.empty()) {
        const std::string current =
            queue.front();

        queue.pop();

        if (
            current.empty() ||
            visited.contains(current)
        ) {
            continue;
        }

        visited.insert(current);
        result.push_back(current);

        const std::string data =
            database.read(current);

        const Commit commit =
            Commit::deserialize(data);

        for (const auto& parent :
             commit.parent_ids()) {

            if (!parent.empty()) {
                queue.push(parent);
            }
        }
    }

    return result;
}

bool Merge::is_ancestor(
    const std::string& ancestor,
    const std::string& descendant
) const {
    if (
        ancestor.empty() ||
        descendant.empty()
    ) {
        return false;
    }

    if (ancestor == descendant) {
        return true;
    }

    const auto history =
        ancestors(descendant);

    return std::find(
        history.begin(),
        history.end(),
        ancestor
    ) != history.end();
}

std::string Merge::find_merge_base(
    const std::string& current_commit,
    const std::string& target_commit
) const {
    if (
        current_commit.empty() ||
        target_commit.empty()
    ) {
        return "";
    }

    if (
        is_ancestor(
            current_commit,
            target_commit
        )
    ) {
        return current_commit;
    }

    if (
        is_ancestor(
            target_commit,
            current_commit
        )
    ) {
        return target_commit;
    }

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::unordered_map<std::string, std::size_t>
        current_distance;

    std::queue<std::pair<std::string, std::size_t>>
        current_queue;

    current_queue.push({
        current_commit,
        0
    });

    while (!current_queue.empty()) {
        const auto [commit, distance] =
            current_queue.front();

        current_queue.pop();

        if (
            commit.empty() ||
            current_distance.contains(commit)
        ) {
            continue;
        }

        current_distance[commit] =
            distance;

        const Commit current =
            Commit::deserialize(
                database.read(commit)
            );

        for (const auto& parent :
             current.parent_ids()) {

            current_queue.push({
                parent,
                distance + 1
            });
        }
    }

    std::string best_base;

    std::size_t best_score =
        static_cast<std::size_t>(-1);

    std::unordered_map<std::string, std::size_t>
        target_distance;

    std::queue<std::pair<std::string, std::size_t>>
        target_queue;

    target_queue.push({
        target_commit,
        0
    });

    while (!target_queue.empty()) {
        const auto [commit, distance] =
            target_queue.front();

        target_queue.pop();

        if (
            commit.empty() ||
            target_distance.contains(commit)
        ) {
            continue;
        }

        target_distance[commit] =
            distance;

        const auto current_it =
            current_distance.find(commit);

        if (
            current_it !=
            current_distance.end()
        ) {
            const std::size_t score =
                current_it->second +
                distance;

            if (
                best_base.empty() ||
                score < best_score
            ) {
                best_base = commit;
                best_score = score;
            }
        }

        const Commit current =
            Commit::deserialize(
                database.read(commit)
            );

        for (const auto& parent :
             current.parent_ids()) {

            target_queue.push({
                parent,
                distance + 1
            });
        }
    }

    return best_base;
}

Merge::Snapshot
Merge::three_way_merge(
    const Snapshot& base,
    const Snapshot& current,
    const Snapshot& target,
    std::vector<std::string>& conflicts
) const {
    Snapshot result;

    std::set<std::string> paths;

    for (const auto& [path, content] :
         base) {
        paths.insert(path);
    }

    for (const auto& [path, content] :
         current) {
        paths.insert(path);
    }

    for (const auto& [path, content] :
         target) {
        paths.insert(path);
    }

    for (const auto& path : paths) {
        const auto base_it =
            base.find(path);

        const auto current_it =
            current.find(path);

        const auto target_it =
            target.find(path);

        const OptionalContent base_value =
            base_it == base.end()
                ? std::nullopt
                : std::optional<std::string>(
                    base_it->second
                );

        const OptionalContent current_value =
            current_it == current.end()
                ? std::nullopt
                : std::optional<std::string>(
                    current_it->second
                );

        const OptionalContent target_value =
            target_it == target.end()
                ? std::nullopt
                : std::optional<std::string>(
                    target_it->second
                );

        if (current_value == target_value) {
            if (current_value.has_value()) {
                result[path] =
                    *current_value;
            }

            continue;
        }

        if (current_value == base_value) {
            if (target_value.has_value()) {
                result[path] =
                    *target_value;
            }

            continue;
        }

        if (target_value == base_value) {
            if (current_value.has_value()) {
                result[path] =
                    *current_value;
            }

            continue;
        }

        conflicts.push_back(path);
    }

    return result;
}

void Merge::ensure_clean_working_tree(
    const std::string& current_commit
) const {
    const Snapshot expected =
        commit_snapshot(current_commit);

    const Snapshot working =
        working_tree_snapshot();

    if (expected != working) {
        throw std::runtime_error(
            "Cannot merge with uncommitted "
            "working tree changes"
        );
    }

    const Snapshot index =
        index_snapshot();

    if (expected != index) {
        throw std::runtime_error(
            "Cannot merge with staged changes"
        );
    }
}

void Merge::ensure_index_matches_working_tree() const {
    ObjectDatabase database(
        repository_.git_directory()
    );

    Index index(
        repository_.git_directory() / "index"
    );

    index.load();

    for (const auto& entry :
         index.entries()) {

        const auto path =
            repository_.root() /
            std::filesystem::path(entry.path);

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error(
                "Cannot continue merge; file is missing: " +
                entry.path
            );
        }

        Blob blob =
            Blob::from_file(path);

        const std::string current_id =
            Hash::sha256(blob.serialize());

        if (current_id != entry.object_id) {
            throw std::runtime_error(
                "Cannot continue merge; file has "
                "unstaged changes: " +
                entry.path
            );
        }
    }
}

void Merge::write_snapshot(
    const Snapshot& snapshot
) const {
    const auto root =
        repository_.root();

    const auto git_directory =
        repository_.git_directory();

    for (
        auto iterator =
            std::filesystem::recursive_directory_iterator(
                root
            );
        iterator !=
        std::filesystem::recursive_directory_iterator();
        ++iterator
    ) {
        const auto path =
            iterator->path();

        if (
            path == git_directory ||
            path.string().rfind(
                git_directory.string() +
                std::filesystem::path::preferred_separator,
                0
            ) == 0
        ) {
            if (
                iterator->is_directory() &&
                path != git_directory
            ) {
                iterator.disable_recursion_pending();
            }

            continue;
        }

        if (iterator->is_directory()) {
            continue;
        }

        std::filesystem::remove(path);
    }

    for (const auto& [relative_path, content] :
         snapshot) {

        const auto target =
            root /
            std::filesystem::path(
                relative_path
            );

        std::filesystem::create_directories(
            target.parent_path()
        );

        std::ofstream file(
            target,
            std::ios::binary
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to write merged file: " +
                target.string()
            );
        }

        file.write(
            content.data(),
            static_cast<std::streamsize>(
                content.size()
            )
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to write merged file: " +
                target.string()
            );
        }
    }
}

void Merge::synchronize_index(
    const std::string& commit_id
) const {
    const Snapshot snapshot =
        commit_snapshot(commit_id);

    synchronize_index(snapshot);
}

void Merge::synchronize_index(
    const std::map<std::string, std::string>& files
) const {
    ObjectDatabase database(
        repository_.git_directory()
    );

    Index index(
        repository_.git_directory() / "index"
    );

    index.clear();

    for (const auto& [path, content] :
         files) {

        const auto absolute =
            repository_.root() /
            std::filesystem::path(path);

        if (!std::filesystem::exists(absolute)) {
            continue;
        }

        Blob blob =
            Blob::from_file(absolute);

        const std::string object_id =
            database.store(blob);

        index.add(
            IndexEntry{
                path,
                object_id
            }
        );
    }

    index.save();
}

std::string Merge::conflict_content(
    const OptionalContent& current,
    const OptionalContent& target,
    const std::string& branch
) const {
    std::ostringstream output;

    output
        << "<<<<<<< ours\n";

    if (current.has_value()) {
        output
            << ensure_trailing_newline(
                *current
            );
    }

    output
        << "=======\n";

    if (target.has_value()) {
        output
            << ensure_trailing_newline(
                *target
            );
    }

    output
        << ">>>>>>> "
        << branch
        << '\n';

    return output.str();
}

void Merge::write_conflict_files(
    const Snapshot& current,
    const Snapshot& target,
    const std::vector<std::string>& conflicts,
    const std::string& branch
) const {
    for (const auto& path : conflicts) {
        const auto current_it =
            current.find(path);

        const auto target_it =
            target.find(path);

        const OptionalContent current_value =
            current_it == current.end()
                ? std::nullopt
                : std::optional<std::string>(
                    current_it->second
                );

        const OptionalContent target_value =
            target_it == target.end()
                ? std::nullopt
                : std::optional<std::string>(
                    target_it->second
                );

        const auto absolute =
            repository_.root() /
            std::filesystem::path(path);

        std::filesystem::create_directories(
            absolute.parent_path()
        );

        std::ofstream file(
            absolute,
            std::ios::binary
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to write conflict file: " +
                absolute.string()
            );
        }

        const std::string content =
            conflict_content(
                current_value,
                target_value,
                branch
            );

        file.write(
            content.data(),
            static_cast<std::streamsize>(
                content.size()
            )
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to write conflict file: " +
                absolute.string()
            );
        }
    }
}

std::string Merge::merge(
    const std::string& branch,
    const std::string& author,
    const std::string& message
) {
    if (branch.empty()) {
        throw std::invalid_argument(
            "Branch name cannot be empty"
        );
    }

    if (repository_.merge_in_progress()) {
        throw std::runtime_error(
            "A merge is already in progress"
        );
    }

    if (repository_.is_detached_head()) {
        throw std::runtime_error(
            "Cannot merge while HEAD is detached"
        );
    }

    const std::string current_branch =
        repository_.current_branch();

    if (current_branch.empty()) {
        throw std::runtime_error(
            "Cannot determine current branch"
        );
    }

    if (branch == current_branch) {
        throw std::runtime_error(
            "Cannot merge a branch into itself"
        );
    }

    const auto branches =
        repository_.branches();

    if (
        std::find(
            branches.begin(),
            branches.end(),
            branch
        ) == branches.end()
    ) {
        throw std::runtime_error(
            "Branch does not exist: " +
            branch
        );
    }

    Reference target_reference(
        repository_.git_directory(),
        "refs/heads/" + branch
    );

    if (!target_reference.exists()) {
        throw std::runtime_error(
            "Branch does not exist: " +
            branch
        );
    }

    const std::string current_commit =
        repository_.head_commit();

    const std::string target_commit =
        target_reference.read();

    if (target_commit.empty()) {
        throw std::runtime_error(
            "Target branch has no commit: " +
            branch
        );
    }

    if (current_commit.empty()) {
        throw std::runtime_error(
            "Current branch has no commit"
        );
    }

    if (current_commit == target_commit) {
        return current_commit;
    }

    ensure_clean_working_tree(
        current_commit
    );

    /*
     * Fast-forward merge.
     */
    if (
        is_ancestor(
            current_commit,
            target_commit
        )
    ) {
        repository_.update_branch(
            current_branch,
            target_commit
        );

        repository_.restore_commit(
            target_commit
        );

        synchronize_index(
            target_commit
        );

        return target_commit;
    }

    /*
     * Already up to date.
     */
    if (
        is_ancestor(
            target_commit,
            current_commit
        )
    ) {
        return current_commit;
    }

    const std::string base_commit =
        find_merge_base(
            current_commit,
            target_commit
        );

    if (base_commit.empty()) {
        throw std::runtime_error(
            "Cannot merge unrelated histories"
        );
    }

    const Snapshot base =
        commit_snapshot(base_commit);

    const Snapshot current =
        commit_snapshot(current_commit);

    const Snapshot target =
        commit_snapshot(target_commit);

    std::vector<std::string> conflicts;

    const Snapshot merged =
        three_way_merge(
            base,
            current,
            target,
            conflicts
        );

    const std::string merge_message =
        message.empty()
            ? "Merge branch '" +
              branch +
              "'"
            : message;

    /*
     * Conflict case:
     *
     * Do not create a commit.
     * Instead, write conflict markers and
     * persist the merge state.
     */
    if (!conflicts.empty()) {
        write_conflict_files(
            current,
            target,
            conflicts,
            branch
        );

        repository_.begin_merge_state(
            current_commit,
            target_commit,
            merge_message,
            conflicts
        );

        std::ostringstream error;

        error
            << "Merge conflict(s):\n";

        for (const auto& path :
             conflicts) {
            error
                << "  "
                << path
                << '\n';
        }

        error
            << "\nResolve the conflicts, "
               "stage the resolved files, then run:\n"
               "  mini-git merge --continue\n"
               "\n"
               "To cancel the merge, run:\n"
               "  mini-git merge --abort\n";

        throw std::runtime_error(
            error.str()
        );
    }

    /*
     * Clean merge.
     */
    write_snapshot(
        merged
    );

    ObjectDatabase database(
        repository_.git_directory()
    );

    Index index(
        repository_.git_directory() / "index"
    );

    index.clear();

    for (const auto& [path, content] :
         merged) {

        const auto absolute =
            repository_.root() /
            std::filesystem::path(path);

        Blob blob =
            Blob::from_file(absolute);

        const std::string object_id =
            database.store(blob);

        index.add(
            IndexEntry{
                path,
                object_id
            }
        );
    }

    const std::string tree_id =
        TreeBuilder(database)
            .build_from_index(
                index,
                repository_.root()
            );

    index.save();

    Commit merge_commit(
        tree_id,
        std::vector<std::string>{
            current_commit,
            target_commit
        },
        author,
        merge_message
    );

    const std::string merge_commit_id =
        database.store(
            merge_commit
        );

    repository_.update_branch(
        current_branch,
        merge_commit_id
    );

    return merge_commit_id;
}

std::string Merge::continue_merge(
    const std::string& author
) {
    if (!repository_.merge_in_progress()) {
        throw std::runtime_error(
            "No merge is in progress"
        );
    }

    if (repository_.is_detached_head()) {
        throw std::runtime_error(
            "Cannot continue merge while HEAD "
            "is detached"
        );
    }

    const std::string original_head =
        repository_.merge_orig_head();

    const std::string target_commit =
        repository_.merge_head();

    if (original_head.empty()) {
        throw std::runtime_error(
            "Invalid merge state: missing "
            "MERGE_ORIG_HEAD"
        );
    }

    if (target_commit.empty()) {
        throw std::runtime_error(
            "Invalid merge state: missing "
            "MERGE_HEAD"
        );
    }

    if (
        repository_.head_commit() !=
        original_head
    ) {
        throw std::runtime_error(
            "Cannot continue merge because "
            "HEAD has changed"
        );
    }

    const auto conflicts =
        repository_.merge_conflicts();

    if (!conflicts.empty()) {
        std::ostringstream error;

        error
            << "Cannot continue merge; "
               "unresolved conflict(s):\n";

        for (const auto& path : conflicts) {
            error
                << "  "
                << path
                << '\n';
        }

        throw std::runtime_error(
            error.str()
        );
    }

    ensure_index_matches_working_tree();

    Index index(
        repository_.git_directory() / "index"
    );

    index.load();

    ObjectDatabase database(
        repository_.git_directory()
    );

    const std::string tree_id =
        TreeBuilder(database)
            .build_from_index(
                index,
                repository_.root()
            );

    std::string message =
        repository_.merge_message();

    if (message.empty()) {
        message =
            "Merge commit";
    }

    Commit merge_commit(
        tree_id,
        std::vector<std::string>{
            original_head,
            target_commit
        },
        author,
        message
    );

    const std::string commit_id =
        database.store(
            merge_commit
        );

    repository_.update_branch(
        repository_.current_branch(),
        commit_id
    );

    repository_.clear_merge_state();

    return commit_id;
}

void Merge::abort_merge()
{
    if (!repository_.merge_in_progress()) {
        throw std::runtime_error(
            "No merge is in progress"
        );
    }

    const std::string original_head =
        repository_.merge_orig_head();

    if (original_head.empty()) {
        throw std::runtime_error(
            "Invalid merge state: missing "
            "MERGE_ORIG_HEAD"
        );
    }

    if (
        repository_.head_commit() !=
        original_head
    ) {
        throw std::runtime_error(
            "Cannot abort merge because "
            "HEAD has changed"
        );
    }

    repository_.restore_commit(
        original_head
    );

    synchronize_index(
        original_head
    );

    repository_.clear_merge_state();
}