#include "Blob.hpp"
#include "ObjectDatabase.hpp"
#include "TreeBuilder.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

void test_build_directory() {
    const std::filesystem::path test_root =
        std::filesystem::temp_directory_path() /
        "mini-git-tree-builder-test";

    std::filesystem::remove_all(test_root);

    const std::filesystem::path git_dir =
        test_root / ".mini-git";

    std::filesystem::create_directories(
        git_dir / "objects"
    );

    const std::filesystem::path source_dir =
        test_root / "project";

    std::filesystem::create_directories(source_dir);

    // Create root-level files.
    {
        std::ofstream file(
            source_dir / "main.cpp",
            std::ios::binary
        );

        file << "int main() {}\n";
    }

    {
        std::ofstream file(
            source_dir / "README.md",
            std::ios::binary
        );

        file << "Hello Mini Git!\n";
    }

    // Create a nested directory.
    const std::filesystem::path src_dir =
        source_dir / "src";

    std::filesystem::create_directories(src_dir);

    // Create files inside the nested directory.
    {
        std::ofstream file(
            src_dir / "App.cpp",
            std::ios::binary
        );

        file << "void app() {}\n";
    }

    {
        std::ofstream file(
            src_dir / "Utils.cpp",
            std::ios::binary
        );

        file << "void utils() {}\n";
    }

    // Create an empty directory.
    const std::filesystem::path empty_dir =
        source_dir / "empty";

    std::filesystem::create_directories(empty_dir);

    // Create a fake .mini-git directory.
    // TreeBuilder must ignore it.
    const std::filesystem::path fake_git_dir =
        source_dir / ".mini-git";

    std::filesystem::create_directories(
        fake_git_dir / "objects"
    );

    {
        std::ofstream file(
            fake_git_dir / "should-not-be-included.txt",
            std::ios::binary
        );

        file << "This must not appear in the Tree.\n";
    }

    // Create the object database.
    ObjectDatabase database(git_dir);

    // Create the TreeBuilder.
    TreeBuilder builder(database);

    // Build the complete directory tree.
    const std::string tree_id =
        builder.build(source_dir);

    // The root Tree should exist.
    assert(database.exists(tree_id));

    // Read the serialized root Tree.
    const std::string tree_data =
        database.read(tree_id);

    // The Tree should contain data.
    assert(!tree_data.empty());

    // The root Tree should contain the nested src directory.
    assert(
        tree_data.find("tree ") != std::string::npos
    );

    assert(
        tree_data.find(" src\n") != std::string::npos
    );

    // The root Tree should contain the empty directory.
    assert(
        tree_data.find(" empty\n") != std::string::npos
    );

    // .mini-git must not be included.
    assert(
        tree_data.find(".mini-git") == std::string::npos
    );

    // Clean up.
    std::filesystem::remove_all(test_root);
}

int main() {
    test_build_directory();

    std::cout << "All TreeBuilder tests passed.\n";

    return 0;
}