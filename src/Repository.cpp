#include "Repository.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Tree.hpp"
#include "Index.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

Repository::Repository(
    const std::filesystem::path& root
)
    : root_(std::filesystem::absolute(root)),
      git_dir_(root_ / ".mini-git")
{
}

void Repository::initialize()
{
    if (std::filesystem::exists(git_dir_)) {
        throw std::runtime_error(
            "Repository already initialized"
        );
    }

    std::filesystem::create_directories(
        git_dir_ / "objects"
    );

    std::filesystem::create_directories(
        git_dir_ / "refs" / "heads"
    );

    std::filesystem::create_directories(
        git_dir_ / "refs" / "tags"
    );

    std::ofstream head_file(
        git_dir_ / "HEAD"
    );

    if (!head_file) {
        throw std::runtime_error(
            "Failed to create HEAD"
        );
    }

    head_file << "ref: refs/heads/main\n";
}

const std::filesystem::path&
Repository::root() const
{
    return root_;
}

const std::filesystem::path&
Repository::git_directory() const
{
    return git_dir_;
}

std::string Repository::read_head() const
{
    std::ifstream file(
        git_dir_ / "HEAD"
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to read HEAD"
        );
    }

    std::string line;

    if (!std::getline(file, line)) {
        throw std::runtime_error(
            "HEAD is empty"
        );
    }

    return line;
}

void Repository::write_head(
    const std::string& content
) const
{
    std::ofstream file(
        git_dir_ / "HEAD"
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to write HEAD"
        );
    }

    file << content << '\n';
}

std::string Repository::head_reference() const
{
    const std::string head =
        read_head();

    constexpr const char* prefix =
        "ref: ";

    if (head.rfind(prefix, 0) != 0) {
        return "";
    }

    return head.substr(5);
}

bool Repository::is_detached_head() const
{
    return head_reference().empty();
}

std::string Repository::current_branch() const
{
    const std::string reference =
        head_reference();

    constexpr const char* prefix =
        "refs/heads/";

    if (reference.rfind(prefix, 0) != 0) {
        return "";
    }

    return reference.substr(
        std::string(prefix).size()
    );
}

std::string Repository::head_commit() const
{
    const std::string head =
        read_head();

    constexpr const char* prefix =
        "ref: ";

    if (head.rfind(prefix, 0) == 0) {
        const std::string reference_name =
            head.substr(5);

        Reference reference(
            git_dir_,
            reference_name
        );

        if (!reference.exists()) {
            return "";
        }

        return reference.read();
    }

    return head;
}

std::vector<std::string>
Repository::branches() const
{
    std::vector<std::string> result;

    const auto heads_directory =
        git_dir_ / "refs" / "heads";

    if (!std::filesystem::exists(
            heads_directory
        )) {
        return result;
    }

    for (
        const auto& entry :
        std::filesystem::recursive_directory_iterator(
            heads_directory
        )
    ) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto relative =
            std::filesystem::relative(
                entry.path(),
                heads_directory
            );

        result.push_back(
            relative.generic_string()
        );
    }

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

std::vector<std::string>
Repository::tags() const
{
    std::vector<std::string> result;

    const auto tags_directory =
        git_dir_ / "refs" / "tags";

    if (!std::filesystem::exists(tags_directory)) {
        return result;
    }

    for (
        const auto& entry :
        std::filesystem::recursive_directory_iterator(
            tags_directory
        )
    ) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto relative =
            std::filesystem::relative(
                entry.path(),
                tags_directory
            );

        result.push_back(
            relative.generic_string()
        );
    }

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

bool Repository::tag_exists(
    const std::string& tag
) const
{
    Reference reference(
        git_dir_,
        "refs/tags/" + tag
    );

    return reference.exists();
}

std::string Repository::tag_commit(
    const std::string& tag
) const
{
    Reference reference(
        git_dir_,
        "refs/tags/" + tag
    );

    if (!reference.exists()) {
        throw std::runtime_error(
            "Tag does not exist: " + tag
        );
    }

    return reference.read();
}

void Repository::create_tag(
    const std::string& tag,
    const std::string& commit_id
)
{
    if (tag.empty()) {
        throw std::invalid_argument(
            "Tag name cannot be empty"
        );
    }

    Reference reference(
        git_dir_,
        "refs/tags/" + tag
    );

    if (reference.exists()) {
        throw std::runtime_error(
            "Tag already exists: " + tag
        );
    }

    const std::string target =
        commit_id.empty()
            ? head_commit()
            : commit_id;

    if (target.empty()) {
        throw std::runtime_error(
            "Cannot create a tag without a commit"
        );
    }

    ObjectDatabase database(
        git_dir_
    );

    if (!database.exists(target)) {
        throw std::runtime_error(
            "Commit does not exist: " + target
        );
    }

    const std::string data =
        database.read(target);

    try {
        (void)Commit::deserialize(data);
    }
    catch (const std::exception&) {
        throw std::runtime_error(
            "Tag target is not a valid commit: " + target
        );
    }

    reference.write(target);
}

void Repository::delete_tag(
    const std::string& tag
)
{
    if (tag.empty()) {
        throw std::invalid_argument(
            "Tag name cannot be empty"
        );
    }

    Reference reference(
        git_dir_,
        "refs/tags/" + tag
    );

    if (!reference.exists()) {
        throw std::runtime_error(
            "Tag does not exist: " + tag
        );
    }

    if (!std::filesystem::remove(reference.path())) {
        throw std::runtime_error(
            "Failed to delete tag: " + tag
        );
    }
}

void Repository::update_branch(
    const std::string& branch,
    const std::string& commit_id
)
{
    if (branch.empty()) {
        throw std::invalid_argument(
            "Branch name cannot be empty"
        );
    }

    Reference reference(
        git_dir_,
        "refs/heads/" + branch
    );

    reference.write(commit_id);
}

void Repository::create_branch(
    const std::string& branch
)
{
    if (branch.empty()) {
        throw std::invalid_argument(
            "Branch name cannot be empty"
        );
    }

    const std::string commit =
        head_commit();

    if (commit.empty()) {
        throw std::runtime_error(
            "Cannot create a branch before "
            "the first commit"
        );
    }

    Reference reference(
        git_dir_,
        "refs/heads/" + branch
    );

    if (reference.exists()) {
        throw std::runtime_error(
            "Branch already exists: " + branch
        );
    }

    reference.write(commit);
}

void Repository::verify_checkout_is_safe(
    const std::string& target_commit
) const
{
    /*
     * Phase 16 prevents branch switching while
     * an unresolved merge is active.
     *
     * This avoids silently discarding merge state.
     */
    (void)target_commit;

    if (merge_in_progress()) {
        throw std::runtime_error(
            "Cannot checkout while a merge is in progress; "
            "use 'mini-git merge --continue' or "
            "'mini-git merge --abort'"
        );
    }
}

void Repository::checkout_tree(
    const std::string& tree_id
) const
{
    checkout_tree_recursive(
        tree_id,
        root_
    );
}

void Repository::checkout_tree_recursive(
    const std::string& tree_id,
    const std::filesystem::path& directory
) const
{
    ObjectDatabase database(
        git_dir_
    );

    const std::string data =
        database.read(tree_id);

    std::istringstream stream(data);

    std::string type;
    std::string object_id;
    std::string name;

    while (
        stream >> type
        >> object_id
        >> name
    ) {
        const auto target =
            directory / name;

        if (type == "blob") {
            const std::string blob_data =
                database.read(object_id);

            const std::size_t separator =
                blob_data.find('\0');

            if (
                separator ==
                std::string::npos
            ) {
                throw std::runtime_error(
                    "Invalid blob object: " +
                    object_id
                );
            }

            const std::string content =
                blob_data.substr(
                    separator + 1
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
                    "Failed to restore file: " +
                    target.string()
                );
            }

            file.write(
                content.data(),
                static_cast<std::streamsize>(
                    content.size()
                )
            );
        }
        else if (type == "tree") {
            std::filesystem::create_directories(
                target
            );

            checkout_tree_recursive(
                object_id,
                target
            );
        }
        else {
            throw std::runtime_error(
                "Invalid tree entry type: " +
                type
            );
        }
    }
}

void Repository::remove_working_tree_files() const
{
    for (
        const auto& entry :
        std::filesystem::directory_iterator(
            root_
        )
    ) {
        if (
            entry.path().filename() ==
            ".mini-git"
        ) {
            continue;
        }

        std::filesystem::remove_all(
            entry.path()
        );
    }
}

void Repository::restore_commit(
    const std::string& commit_id
) const
{
    if (commit_id.empty()) {
        throw std::invalid_argument(
            "Commit ID cannot be empty"
        );
    }

    ObjectDatabase database(
        git_dir_
    );

    const std::string commit_data =
        database.read(commit_id);

    const Commit commit =
        Commit::deserialize(
            commit_data
        );

    remove_working_tree_files();

    checkout_tree(
        commit.tree_id()
    );
}

void Repository::rebuild_index_from_tree(
    const std::string& tree_id
) const
{
    Index index(
        git_dir_ / "index"
    );

    index.load();
    index.clear();

    add_tree_to_index(
        tree_id,
        std::filesystem::path{},
        index
    );

    index.save();
}

void Repository::add_tree_to_index(
    const std::string& tree_id,
    const std::filesystem::path& relative_directory,
    Index& index
) const
{
    ObjectDatabase database(
        git_dir_
    );

    const std::string data =
        database.read(tree_id);

    Tree tree =
        Tree::deserialize(data);

    for (const auto& entry : tree.entries()) {
        const auto relative_path =
            relative_directory / entry.name;

        if (entry.is_tree) {
            add_tree_to_index(
                entry.object_id,
                relative_path,
                index
            );
        }
        else {
            index.add({
                relative_path.generic_string(),
                entry.object_id
            });
        }
    }
}

// ============================================================
// Merge state
// ============================================================

bool Repository::merge_in_progress() const
{
    return std::filesystem::exists(
        git_dir_ / "MERGE_HEAD"
    );
}

std::string Repository::merge_head() const
{
    std::ifstream file(
        git_dir_ / "MERGE_HEAD"
    );

    if (!file) {
        return "";
    }

    std::string value;

    std::getline(
        file,
        value
    );

    return value;
}

std::string Repository::merge_orig_head() const
{
    std::ifstream file(
        git_dir_ / "MERGE_ORIG_HEAD"
    );

    if (!file) {
        return "";
    }

    std::string value;

    std::getline(
        file,
        value
    );

    return value;
}

std::string Repository::merge_message() const
{
    std::ifstream file(
        git_dir_ / "MERGE_MSG"
    );

    if (!file) {
        return "";
    }

    std::ostringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

std::vector<std::string>
Repository::merge_conflicts() const
{
    std::vector<std::string> conflicts;

    std::ifstream file(
        git_dir_ / "MERGE_CONFLICTS"
    );

    if (!file) {
        return conflicts;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            conflicts.push_back(line);
        }
    }

    return conflicts;
}

bool Repository::is_merge_conflict(
    const std::string& path
) const
{
    const auto conflicts =
        merge_conflicts();

    return std::find(
        conflicts.begin(),
        conflicts.end(),
        path
    ) != conflicts.end();
}

void Repository::begin_merge_state(
    const std::string& original_head,
    const std::string& merge_head,
    const std::string& message,
    const std::vector<std::string>& conflicts
) const
{
    if (
        original_head.empty() ||
        merge_head.empty()
    ) {
        throw std::invalid_argument(
            "Merge state requires valid commit IDs"
        );
    }

    if (merge_in_progress()) {
        throw std::runtime_error(
            "A merge is already in progress"
        );
    }

    {
        std::ofstream file(
            git_dir_ / "MERGE_HEAD"
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to create MERGE_HEAD"
            );
        }

        file << merge_head << '\n';
    }

    {
        std::ofstream file(
            git_dir_ / "MERGE_ORIG_HEAD"
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to create MERGE_ORIG_HEAD"
            );
        }

        file << original_head << '\n';
    }

    {
        std::ofstream file(
            git_dir_ / "MERGE_MSG"
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to create MERGE_MSG"
            );
        }

        file << message;
    }

    {
        std::ofstream file(
            git_dir_ / "MERGE_CONFLICTS"
        );

        if (!file) {
            throw std::runtime_error(
                "Failed to create MERGE_CONFLICTS"
            );
        }

        for (const auto& path : conflicts) {
            file << path << '\n';
        }
    }
}

void Repository::resolve_merge_conflict(
    const std::string& path
) const
{
    if (!merge_in_progress()) {
        throw std::runtime_error(
            "No merge is in progress"
        );
    }

    const auto conflicts =
        merge_conflicts();

    if (
        std::find(
            conflicts.begin(),
            conflicts.end(),
            path
        ) == conflicts.end()
    ) {
        return;
    }

    std::ofstream file(
        git_dir_ / "MERGE_CONFLICTS"
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to update merge conflicts"
        );
    }

    for (const auto& conflict : conflicts) {
        if (conflict != path) {
            file << conflict << '\n';
        }
    }
}

void Repository::clear_merge_state() const
{
    std::filesystem::remove(
        git_dir_ / "MERGE_HEAD"
    );

    std::filesystem::remove(
        git_dir_ / "MERGE_ORIG_HEAD"
    );

    std::filesystem::remove(
        git_dir_ / "MERGE_MSG"
    );

    std::filesystem::remove(
        git_dir_ / "MERGE_CONFLICTS"
    );
}

void Repository::checkout(
    const std::string& branch
)
{
    if (branch.empty()) {
        throw std::invalid_argument(
            "Branch name cannot be empty"
        );
    }

    Reference reference(
        git_dir_,
        "refs/heads/" + branch
    );

    if (!reference.exists()) {
        throw std::runtime_error(
            "Branch does not exist: " +
            branch
        );
    }

    const std::string target_commit =
        reference.read();

    if (target_commit.empty()) {
        throw std::runtime_error(
            "Branch has no commit: " +
            branch
        );
    }

    if (current_branch() == branch) {
        return;
    }

    verify_checkout_is_safe(
        target_commit
    );

    ObjectDatabase database(
        git_dir_
    );

    const std::string commit_data =
        database.read(target_commit);

    const Commit commit =
        Commit::deserialize(
            commit_data
        );

    write_head(
        "ref: refs/heads/" + branch
    );

    checkout_tree(
        commit.tree_id()
    );

    rebuild_index_from_tree(
        commit.tree_id()
    );
}
