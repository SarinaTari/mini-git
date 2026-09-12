#include "Blob.hpp"
#include "Commit.hpp"
#include "Doctor.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main() {

    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-doctor-tests";

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

    Commit commit(
        tree_id,
        "",
        "Test Author",
        "Initial commit"
    );

    const auto commit_id =
        database.store(commit);

    repository.update_branch(
        "main",
        commit_id
    );

    Doctor doctor(repository);

    const auto output =
        doctor.render();

    assert(
        output.find("[OK]") !=
        std::string::npos
    );

    assert(
        output.find("Health:") !=
        std::string::npos
    );

    std::filesystem::remove_all(root);

    std::cout
        << "Doctor tests passed.\n";

    return 0;
}