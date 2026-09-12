#include "ObjectDatabase.hpp"
#include "TreeBuilder.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

void write_file(
    const std::filesystem::path& path,
    const std::string& content
)
{
    std::filesystem::create_directories(
        path.parent_path()
    );

    std::ofstream file(
        path,
        std::ios::binary
    );

    assert(file);

    file << content;

    assert(file.good());
}

} // namespace

void test_build_directory()
{
    const auto test_root =
        std::filesystem::temp_directory_path()
        / "mini-git-tree-builder-test";

    std::filesystem::remove_all(test_root);

    const auto git_dir =
        test_root / ".mini-git";

    std::filesystem::create_directories(
        git_dir / "objects"
    );

    const auto source_dir =
        test_root / "project";

    std::filesystem::create_directories(
        source_dir
    );

    write_file(
        source_dir / "main.cpp",
        "int main() {}\n"
    );

    write_file(
        source_dir / "README.md",
        "Hello Mini Git!\n"
    );

    const auto src_dir =
        source_dir / "src";

    std::filesystem::create_directories(
        src_dir
    );

    write_file(
        src_dir / "App.cpp",
        "void app() {}\n"
    );

    write_file(
        src_dir / "Utils.cpp",
        "void utils() {}\n"
    );

    const auto empty_dir =
        source_dir / "empty";

    std::filesystem::create_directories(
        empty_dir
    );

    const auto fake_git_dir =
        source_dir / ".mini-git";

    std::filesystem::create_directories(
        fake_git_dir / "objects"
    );

    write_file(
        fake_git_dir / "should-not-be-included.txt",
        "This must not appear in the Tree.\n"
    );

    ObjectDatabase database(git_dir);
    TreeBuilder builder(database);

    const std::string tree_id =
        builder.build(source_dir);

    assert(database.exists(tree_id));

    const std::string tree_data =
        database.read(tree_id);

    assert(!tree_data.empty());

    assert(
        tree_data.find("tree ") !=
        std::string::npos
    );

    assert(
        tree_data.find(" src\n") !=
        std::string::npos
    );

    assert(
        tree_data.find(" empty\n") !=
        std::string::npos
    );

    assert(
        tree_data.find(".mini-git") ==
        std::string::npos
    );

    std::filesystem::remove_all(test_root);
}

int main()
{
    test_build_directory();

    std::cout
        << "All TreeBuilder tests passed.\n";

    return 0;
}