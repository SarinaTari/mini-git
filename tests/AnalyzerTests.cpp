#include "Analyzer.hpp"
#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-analyzer-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    Tree tree;

    const auto tree_id =
        database.store(tree);

    Commit first(
        tree_id,
        "",
        "Alice",
        "Initial commit"
    );

    const auto first_id =
        database.store(first);

    repository.update_branch(
        "main",
        first_id
    );

    repository.create_branch(
        "feature"
    );

    Commit second(
        tree_id,
        first_id,
        "Bob",
        "Second commit"
    );

    const auto second_id =
        database.store(second);

    repository.update_branch(
        "main",
        second_id
    );

    Analyzer analyzer(repository);

    const auto output =
        analyzer.render();

    assert(
        output.find("Commits:") !=
        std::string::npos
    );

    assert(
        output.find("Branches:") !=
        std::string::npos
    );

    assert(
        output.find("Alice") !=
        std::string::npos
    );

    assert(
        output.find("Bob") !=
        std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Analyzer tests passed.\n";

    return 0;
}