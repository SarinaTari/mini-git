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
        / ("mini-git-merge-" + name);

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

    index.add(
        IndexEntry{
            path,
            object_id
        }
    );

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

void test_fast_forward_merge()
{
    const auto root =
        create_test_repository(
            "fast-forward"
        );

    Repository repository(root);

    write_file(
        root / "hello.txt",
        "A\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    const std::string first =
        commit(
            repository,
            "Initial commit"
        );

    repository.create_branch(
        "feature"
    );

    repository.checkout(
        "feature"
    );

    write_file(
        root / "hello.txt",
        "B\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    const std::string second =
        commit(
            repository,
            "Feature commit"
        );

    repository.checkout(
        "main"
    );

    Merge merge(repository);

    const std::string result =
        merge.merge(
            "feature",
            "test"
        );

    assert(result == second);
    assert(repository.head_commit() == second);

    assert(
        !repository.merge_in_progress()
    );

    std::ifstream file(
        root / "hello.txt"
    );

    std::string content;

    std::getline(
        file,
        content
    );

    assert(content == "B");

    std::filesystem::remove_all(root);

    (void)first;
}

void test_already_up_to_date()
{
    const auto root =
        create_test_repository(
            "already-up-to-date"
        );

    Repository repository(root);

    write_file(
        root / "file.txt",
        "A\n"
    );

    stage_file(
        repository,
        "file.txt"
    );

    const std::string first =
        commit(
            repository,
            "Initial commit"
        );

    repository.create_branch(
        "feature"
    );

    write_file(
        root / "file.txt",
        "B\n"
    );

    stage_file(
        repository,
        "file.txt"
    );

    const std::string second =
        commit(
            repository,
            "Main commit"
        );

    Merge merge(repository);

    const std::string result =
        merge.merge(
            "feature",
            "test"
        );

    assert(result == second);
    assert(repository.head_commit() == second);

    assert(
        !repository.merge_in_progress()
    );

    std::filesystem::remove_all(root);

    (void)first;
}

void test_non_conflicting_merge()
{
    const auto root =
        create_test_repository(
            "non-conflicting"
        );

    Repository repository(root);

    write_file(
        root / "common.txt",
        "A\n"
    );

    write_file(
        root / "main.txt",
        "A\n"
    );

    write_file(
        root / "feature.txt",
        "A\n"
    );

    stage_file(
        repository,
        "common.txt"
    );

    stage_file(
        repository,
        "main.txt"
    );

    stage_file(
        repository,
        "feature.txt"
    );

    const std::string base =
        commit(
            repository,
            "Initial commit"
        );

    repository.create_branch(
        "feature"
    );

    repository.checkout(
        "feature"
    );

    write_file(
        root / "feature.txt",
        "Feature change\n"
    );

    stage_file(
        repository,
        "feature.txt"
    );

    const std::string feature_commit =
        commit(
            repository,
            "Feature work"
        );

    repository.checkout(
        "main"
    );

    write_file(
        root / "main.txt",
        "Main change\n"
    );

    stage_file(
        repository,
        "main.txt"
    );

    const std::string main_commit =
        commit(
            repository,
            "Main work"
        );

    Merge merge(repository);

    const std::string merge_id =
        merge.merge(
            "feature",
            "test"
        );

    assert(
        merge_id != main_commit
    );

    assert(
        merge_id != feature_commit
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

    std::ifstream main_file(
        root / "main.txt"
    );

    std::string main_content;

    std::getline(
        main_file,
        main_content
    );

    assert(
        main_content ==
        "Main change"
    );

    std::ifstream feature_file(
        root / "feature.txt"
    );

    std::string feature_content;

    std::getline(
        feature_file,
        feature_content
    );

    assert(
        feature_content ==
        "Feature change"
    );

    std::filesystem::remove_all(root);

    (void)base;
}

void test_conflicting_merge()
{
    const auto root =
        create_test_repository(
            "conflict"
        );

    Repository repository(root);

    write_file(
        root / "hello.txt",
        "A\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    commit(
        repository,
        "Initial commit"
    );

    repository.create_branch(
        "feature"
    );

    repository.checkout(
        "feature"
    );

    write_file(
        root / "hello.txt",
        "Feature\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    const std::string feature_commit =
        commit(
            repository,
            "Feature change"
        );

    repository.checkout(
        "main"
    );

    write_file(
        root / "hello.txt",
        "Main\n"
    );

    stage_file(
        repository,
        "hello.txt"
    );

    const std::string main_commit =
        commit(
            repository,
            "Main change"
        );

    Merge merge(repository);

    bool conflict_detected = false;

    try {
        merge.merge(
            "feature",
            "test"
        );
    }
    catch (const std::runtime_error& error) {
        conflict_detected = true;

        const std::string message =
            error.what();

        assert(
            message.find("hello.txt") !=
            std::string::npos
        );
    }

    assert(conflict_detected);

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

    assert(
        repository.head_commit() ==
        main_commit
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

    /*
     * Clean up the merge state for this test.
     */
    merge.abort_merge();

    assert(
        !repository.merge_in_progress()
    );

    std::filesystem::remove_all(root);
}

void test_missing_branch()
{
    const auto root =
        create_test_repository(
            "missing-branch"
        );

    Repository repository(root);

    write_file(
        root / "file.txt",
        "A\n"
    );

    stage_file(
        repository,
        "file.txt"
    );

    commit(
        repository,
        "Initial commit"
    );

    Merge merge(repository);

    bool failed = false;

    try {
        merge.merge(
            "does-not-exist",
            "test"
        );
    }
    catch (const std::runtime_error& error) {
        failed = true;

        const std::string message =
            error.what();

        assert(
            message.find(
                "Branch does not exist"
            ) != std::string::npos
        );
    }

    assert(failed);

    assert(
        !repository.merge_in_progress()
    );

    std::filesystem::remove_all(root);
}

} // namespace

int main()
{
    test_fast_forward_merge();
    test_already_up_to_date();
    test_non_conflicting_merge();
    test_conflicting_merge();
    test_missing_branch();

    std::cout
        << "All Merge tests passed.\n";

    return 0;
}
