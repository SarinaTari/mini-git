#include "Blob.hpp"
#include "Commit.hpp"
#include "Inspector.hpp"
#include "ObjectDatabase.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-inspector-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto git_dir =
        root / ".mini-git";

    ObjectDatabase database(git_dir);

    Blob blob("Hello Inspector\n");
    const auto blob_id =
        database.store(blob);

    Inspector inspector(git_dir);

    const auto blob_output =
        inspector.inspect(blob_id);

    assert(
        blob_output.find("Type: blob")
        != std::string::npos
    );

    assert(
        blob_output.find("Hello Inspector")
        != std::string::npos
    );

    Tree tree;

    tree.add_entry({
        "hello.txt",
        blob_id,
        false
    });

    const auto tree_id =
        database.store(tree);

    const auto tree_output =
        inspector.inspect(tree_id);

    assert(
        tree_output.find("Type: tree")
        != std::string::npos
    );

    assert(
        tree_output.find("hello.txt")
        != std::string::npos
    );

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const auto commit_id =
        database.store(commit);

    const auto commit_output =
        inspector.inspect(commit_id);

    assert(
        commit_output.find("Type: commit")
        != std::string::npos
    );

    assert(
        commit_output.find("Initial commit")
        != std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Inspector tests passed.\n";

    return 0;
}