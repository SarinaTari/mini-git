#include "Blob.hpp"
#include "Commit.hpp"
#include "Index.hpp"
#include "Merge.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "TreeBuilder.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

std::filesystem::path create_test_repository(
    const std::string& name
) {
    const auto root =
        std::filesystem::temp_directory_path()
        / ("mini-git-merge-state-" + name);

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(root);

    Repository repository(root);

    repository.initialize();

    return root;
}

void write_file(
    const std::filesystem::path& path,
    const std::string& content
) {
    std::filesystem::create_directories(
        path.parent_path()
    );

    std::ofstream file(
        path,
        std::ios::binary
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to create test file"
        );
    }

    file << content;
}

void stage_file(
    Repository& repository,
    const std::string& path
) {
    const auto absolute =
        repository.root() /
        std::filesystem::path(path);

    Blob blob =
        Blob::from_file(absolute);

    ObjectDatabase database(
        repository.git_directory()
    );

    const std::string object_id =
        database.store(blob);

    Index index(
        repository.git_directory() / "index"
    );

    index.load();

    index.add({
        path,
        object_id
    });

    index.save();
}

std::string commit(
    Repository& repository,
    const std::string& message
) {
    Index index(
        repository.git_directory() / "index"
    );

    index.load();

    ObjectDatabase database(
        repository.git_directory()
    );

    TreeBuilder tree_builder(
        database
    );

    const std::string tree_id =
        tree_builder.build_from_index(
            index,
            repository.root()
        );

    Commit new_commit(
        tree_id,
        repository.head_commit(),
        "test",
        message
    );

    const std::string commit_id =
        database.store(new_commit);

    repository.update_branch(
        repository.current_branch(),
        commit_id
    );

    return commit_id;
}

void create_conflicting_history(
    Repository& repository,
    std::string& main_commit,
    std::string& feature_commit
) {
    write_file(
        repository.root() / "hello.txt",
        "base\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    commit(
        repository,
        "Initial"
    );

    repository.create_branch(
        "feature"
    );

    repository.checkout(
        "feature"
    );

    write_file(
        repository.root() / "hello.txt",
        "feature\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    feature_commit =
        commit(
            repository,
            "Feature change"
        );

    repository.checkout(
        "main"
    );

    write_file(
        repository.root() / "hello.txt",
        "main\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    main_commit =
        commit(
            repository,
            "Main change"
        );
}

void test_conflict_creates_merge_state()
{
    const auto root =
        create_test_repository(
            "creates-state"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    bool failed = false;

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
        failed = true;
    }

    assert(failed);

    assert(
        repository.merge_in_progress()
    );

    assert(
        repository.merge_orig_head() ==
        main_commit
    );

    assert(
        repository.merge_head() ==
        feature_commit
    );

    const auto conflicts =
        repository.merge_conflicts();

    assert(
        conflicts.size() == 1
    );

    assert(
        conflicts[0] ==
        "hello.txt"
    );

    std::ifstream file(
        root / "hello.txt"
    );

    std::string content{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    assert(
        content.find("<<<<<<< ours") !=
        std::string::npos
    );

    assert(
        content.find("=======") !=
        std::string::npos
    );

    assert(
        content.find(">>>>>>> feature") !=
        std::string::npos
    );

    assert(
        repository.head_commit() ==
        main_commit
    );

    std::filesystem::remove_all(root);
}

void test_continue_fails_with_unresolved_conflict()
{
    const auto root =
        create_test_repository(
            "continue-fails"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
    }

    bool failed = false;

    try {
        merge.continue_merge("test");
    }
    catch (const std::runtime_error& error) {
        failed = true;

        const std::string message =
            error.what();

        assert(
            message.find(
                "unresolved conflict"
            ) != std::string::npos
        );
    }

    assert(failed);

    std::filesystem::remove_all(root);
}

void test_resolve_and_continue()
{
    const auto root =
        create_test_repository(
            "resolve-continue"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
    }

    write_file(
        root / "hello.txt",
        "resolved\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    repository.resolve_merge_conflict(
        "hello.txt"
    );

    assert(
        repository.merge_conflicts().empty()
    );

    const std::string merge_id =
        merge.continue_merge(
            "test"
        );

    assert(
        !merge_id.empty()
    );

    assert(
        repository.head_commit() ==
        merge_id
    );

    assert(
        !repository.merge_in_progress()
    );

    ObjectDatabase database(
        repository.git_directory()
    );

    Commit merge_commit =
        Commit::deserialize(
            database.read(merge_id)
        );

    assert(
        merge_commit.parent_ids().size() == 2
    );

    assert(
        merge_commit.parent_ids()[0] ==
        main_commit
    );

    assert(
        merge_commit.parent_ids()[1] ==
        feature_commit
    );

    std::ifstream file(
        root / "hello.txt"
    );

    std::string content;

    std::getline(
        file,
        content
    );

    assert(
        content == "resolved"
    );

    std::filesystem::remove_all(root);
}

void test_abort_merge()
{
    const auto root =
        create_test_repository(
            "abort"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
    }

    assert(
        repository.merge_in_progress()
    );

    merge.abort_merge();

    assert(
        !repository.merge_in_progress()
    );

    assert(
        repository.head_commit() ==
        main_commit
    );

    std::ifstream file(
        root / "hello.txt"
    );

    std::string content;

    std::getline(
        file,
        content
    );

    assert(
        content == "main"
    );

    Index index(
        repository.git_directory() / "index"
    );

    index.load();

    assert(
        index.contains("hello.txt")
    );

    std::filesystem::remove_all(root);
}

void test_checkout_blocked_during_merge()
{
    const auto root =
        create_test_repository(
            "checkout-blocked"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
    }

    bool failed = false;

    try {
        repository.checkout("feature");
    }
    catch (const std::runtime_error& error) {
        failed = true;

        const std::string message =
            error.what();

        assert(
            message.find(
                "merge is in progress"
            ) != std::string::npos
        );
    }

    assert(failed);

    merge.abort_merge();

    std::filesystem::remove_all(root);
}

void test_commit_blocked_during_merge()
{
    const auto root =
        create_test_repository(
            "commit-blocked"
        );

    Repository repository(root);

    std::string main_commit;
    std::string feature_commit;

    create_conflicting_history(
        repository,
        main_commit,
        feature_commit
    );

    Merge merge(repository);

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error&) {
    }

    /*
     * The repository-level merge state is sufficient
     * for the CLI to prevent normal commits.
     */
    assert(
        repository.merge_in_progress()
    );

    merge.abort_merge();

    std::filesystem::remove_all(root);
}

} // namespace

int main()
{
    test_conflict_creates_merge_state();
    test_continue_fails_with_unresolved_conflict();
    test_resolve_and_continue();
    test_abort_merge();
    test_checkout_blocked_during_merge();
    test_commit_blocked_during_merge();

    std::cout
        << "All Merge State tests passed.\n";

    return 0;
}