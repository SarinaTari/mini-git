#include "Blob.hpp"
#include "Hash.hpp"
#include "Index.hpp"
#include "Status.hpp"

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

std::string blob_object_id(
    const std::filesystem::path& path
)
{
    const Blob blob =
        Blob::from_file(path);

    return Hash::sha256(
        blob.serialize()
    );
}

} // namespace

void test_clean_working_tree()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-clean-test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto file_path =
        root / "main.cpp";

    write_file(
        file_path,
        "int main() {}\n"
    );

    Index index("unused-index");

    index.add({
        "main.cpp",
        blob_object_id(file_path)
    });

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.empty());
    assert(result.deleted.empty());
    assert(result.untracked.empty());

    std::filesystem::remove_all(root);
}

void test_modified_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-modified-test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto file_path =
        root / "main.cpp";

    write_file(
        file_path,
        "version one\n"
    );

    const std::string original_id =
        blob_object_id(file_path);

    Index index("unused-index");

    index.add({
        "main.cpp",
        original_id
    });

    write_file(
        file_path,
        "version two\n"
    );

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.size() == 1);
    assert(result.modified[0] == "main.cpp");
    assert(result.deleted.empty());
    assert(result.untracked.empty());

    std::filesystem::remove_all(root);
}

void test_untracked_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-untracked-test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto tracked =
        root / "main.cpp";

    const auto untracked =
        root / "notes.txt";

    write_file(
        tracked,
        "int main() {}\n"
    );

    write_file(
        untracked,
        "some notes\n"
    );

    Index index("unused-index");

    index.add({
        "main.cpp",
        blob_object_id(tracked)
    });

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.empty());
    assert(result.deleted.empty());
    assert(result.untracked.size() == 1);
    assert(result.untracked[0] == "notes.txt");

    std::filesystem::remove_all(root);
}

void test_deleted_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-deleted-test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto file_path =
        root / "main.cpp";

    write_file(
        file_path,
        "int main() {}\n"
    );

    Index index("unused-index");

    index.add({
        "main.cpp",
        blob_object_id(file_path)
    });

    std::filesystem::remove(file_path);

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.empty());
    assert(result.deleted.size() == 1);
    assert(result.deleted[0] == "main.cpp");
    assert(result.untracked.empty());

    std::filesystem::remove_all(root);
}

void test_nested_untracked_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-nested-test";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(
        root / "src"
    );

    write_file(
        root / "src" / "App.cpp",
        "void app() {}\n"
    );

    Index index("unused-index");

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.empty());
    assert(result.deleted.empty());
    assert(result.untracked.size() == 1);
    assert(result.untracked[0] == "src/App.cpp");

    std::filesystem::remove_all(root);
}

void test_mini_git_is_ignored()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-status-ignore-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git" / "objects"
    );

    write_file(
        root / ".mini-git" / "index",
        "internal data\n"
    );

    write_file(
        root / "main.cpp",
        "int main() {}\n"
    );

    Index index("unused-index");

    Status status(root, index);

    const StatusResult result =
        status.collect();

    assert(result.modified.empty());
    assert(result.deleted.empty());
    assert(result.untracked.size() == 1);
    assert(result.untracked[0] == "main.cpp");

    std::filesystem::remove_all(root);
}

int main()
{
    test_clean_working_tree();
    test_modified_file();
    test_untracked_file();
    test_deleted_file();
    test_nested_untracked_file();
    test_mini_git_is_ignored();

    std::cout
        << "All Status tests passed.\n";

    return 0;
}