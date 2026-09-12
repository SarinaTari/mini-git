#include "Commit.hpp"
#include "Graph.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

namespace {

std::string create_commit(
    Repository& repository,
    ObjectDatabase& database,
    const std::string& message,
    const std::string& parent = ""
) {
    Tree tree;

    const auto tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        parent,
        "Test Author",
        message
    );

    const auto commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    return commit_id;
}

}

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-graph-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    Repository repository(root);
    repository.initialize();

    ObjectDatabase database(
        repository.git_directory()
    );

    const auto first =
        create_commit(
            repository,
            database,
            "Initial commit"
        );

    const auto second =
        create_commit(
            repository,
            database,
            "Second commit",
            first
        );

    repository.create_tag(
        "v1.0",
        first
    );

    Graph graph(repository);

    const auto output =
        graph.render();

    assert(
        output.find(
            second.substr(0, 8)
        ) != std::string::npos
    );

    assert(
        output.find(
            "Initial commit"
        ) != std::string::npos
    );

    assert(
        output.find("HEAD")
        != std::string::npos
    );

    assert(
        output.find("tag:v1.0")
        != std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Graph tests passed.\n";

    return 0;
}