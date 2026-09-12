#include "Blob.hpp"
#include "Commit.hpp"
#include "Inspector.hpp"
#include "ObjectDatabase.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

int main()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-inspector-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto git_dir =
        root / ".mini-git";

    ObjectDatabase database(git_dir);

    Blob blob("Hello Inspector\n");

    const std::string blob_id =
        database.store(blob);

    Inspector inspector(git_dir);

    const std::string blob_output =
        inspector.inspect(blob_id);

    assert(
        blob_output.find("Type: blob") !=
        std::string::npos
    );

    assert(
        blob_output.find("Hello Inspector") !=
        std::string::npos
    );

    Tree tree;

    tree.add_entry({
        "hello.txt",
        blob_id,
        false
    });

    const std::string tree_id =
        database.store(tree);

    const std::string tree_output =
        inspector.inspect(tree_id);

    assert(
        tree_output.find("Type: tree") !=
        std::string::npos
    );

    assert(
        tree_output.find("hello.txt") !=
        std::string::npos
    );

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const std::string commit_id =
        database.store(commit);

    const std::string commit_output =
        inspector.inspect(commit_id);

    assert(
        commit_output.find("Type: commit") !=
        std::string::npos
    );

    assert(
        commit_output.find("Initial commit") !=
        std::string::npos
    );

    bool failed = false;

    try {
        (void)inspector.inspect("");
    }
    catch (const std::invalid_argument&) {
        failed = true;
    }

    assert(failed);

    failed = false;

    try {
        (void)inspector.inspect(
            "does-not-exist"
        );
    }
    catch (const std::runtime_error&) {
        failed = true;
    }

    assert(failed);

    std::filesystem::remove_all(root);

    std::cout
        << "Inspector tests passed.\n";

    return 0;
}