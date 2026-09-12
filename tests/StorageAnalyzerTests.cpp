#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "StorageAnalyzer.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-storage-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob("storage test");

    const auto blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry({
        "file.txt",
        blob_id,
        false
    });

    const auto tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Storage test"
    );

    database.store(commit);

    StorageAnalyzer analyzer(repository);

    const auto output =
        analyzer.render();

    std::cout
        << "===== STORAGE ANALYZER OUTPUT =====\n"
        << output
        << "===================================\n";

    assert(
        output.find("Total objects:")
        != std::string::npos
    );

    assert(
        output.find("blob")
        != std::string::npos
    );

    assert(
        output.find("commit")
        != std::string::npos
    );

    assert(
        output.find("Total storage:")
        != std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Storage analyzer tests passed.\n";

    return 0;
}