#include "Ancestry.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

int main()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-ancestry-tests";

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
        "Test Author",
        "First"
    );

    const auto first_id =
        database.store(first);

    Commit second(
        tree_id,
        first_id,
        "Test Author",
        "Second"
    );

    const auto second_id =
        database.store(second);

    Commit third(
        tree_id,
        second_id,
        "Test Author",
        "Third"
    );

    const auto third_id =
        database.store(third);

    Ancestry ancestry(
        repository.git_directory()
    );

    assert(
        ancestry.is_ancestor(
            first_id,
            second_id
        )
    );

    assert(
        ancestry.is_ancestor(
            first_id,
            third_id
        )
    );

    assert(
        ancestry.is_ancestor(
            second_id,
            third_id
        )
    );

    assert(
        !ancestry.is_ancestor(
            third_id,
            first_id
        )
    );

    const auto base =
        ancestry.merge_base(
            second_id,
            third_id
        );

    assert(base);
    assert(*base == second_id);

    std::filesystem::remove_all(root);

    std::cout
        << "Ancestry tests passed.\n";

    return 0;
}