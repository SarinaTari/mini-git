#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

std::string create_commit(
    Repository& repository,
    ObjectDatabase& database,
    const std::string& message,
    const std::string& parent = ""
)
{
    Blob blob(message);

    const std::string blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry({
        "file.txt",
        blob_id,
        false
    });

    const std::string tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        parent,
        "Test Author",
        message
    );

    const std::string commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    return commit_id;
}

bool throws(
    const auto& function
)
{
    try {
        function();
    }
    catch (const std::exception&) {
        return true;
    }

    return false;
}

} // namespace

int main()
{
    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "mini-git-tag-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    Repository repository(test_directory);
    repository.initialize();

    assert(
        std::filesystem::exists(
            repository.git_directory()
            / "refs"
            / "tags"
        )
    );

    ObjectDatabase database(
        repository.git_directory()
    );

    assert(repository.tags().empty());
    assert(!repository.tag_exists("v1.0"));

    const std::string commit_a =
        create_commit(
            repository,
            database,
            "Initial commit"
        );

    repository.create_tag("v1.0");

    assert(repository.tag_exists("v1.0"));

    assert(
        repository.tag_commit("v1.0") ==
        commit_a
    );

    assert(
        std::filesystem::exists(
            repository.git_directory()
            / "refs"
            / "tags"
            / "v1.0"
        )
    );

    assert(
        throws([&]() {
            repository.create_tag("v1.0");
        })
    );

    const std::string commit_b =
        create_commit(
            repository,
            database,
            "Second commit",
            commit_a
        );

    assert(
        repository.head_commit() == commit_b
    );

    repository.create_tag(
        "v1.1",
        commit_a
    );

    assert(
        repository.tag_commit("v1.1") ==
        commit_a
    );

    repository.create_tag(
        "v2.0",
        commit_b
    );

    const auto tags =
        repository.tags();

    assert(tags.size() == 3);
    assert(tags[0] == "v1.0");
    assert(tags[1] == "v1.1");
    assert(tags[2] == "v2.0");

    assert(
        throws([&]() {
            repository.create_tag(
                "invalid",
                "does-not-exist"
            );
        })
    );

    const std::string blob_id =
        database.store(
            Blob("blob")
        );

    assert(
        throws([&]() {
            repository.create_tag(
                "blob-tag",
                blob_id
            );
        })
    );

    assert(
        throws([&]() {
            repository.create_tag("");
        })
    );

    assert(
        throws([&]() {
            repository.create_tag(
                "../outside"
            );
        })
    );

    assert(
        throws([&]() {
            (void)repository.tag_commit(
                "missing"
            );
        })
    );

    repository.delete_tag("v2.0");

    assert(!repository.tag_exists("v2.0"));
    assert(repository.tags().size() == 2);

    assert(database.exists(commit_b));

    const Commit restored =
        Commit::deserialize(
            database.read(commit_b)
        );

    assert(
        restored.message() ==
        "Second commit"
    );

    assert(
        throws([&]() {
            repository.delete_tag("missing");
        })
    );

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Tag tests passed.\n";

    return 0;
}