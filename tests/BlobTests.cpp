#include "Blob.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void write_binary_file(
    const std::filesystem::path& path,
    const std::string& data
)
{
    std::ofstream file(
        path,
        std::ios::binary
    );

    assert(file);

    file.write(
        data.data(),
        static_cast<std::streamsize>(data.size())
    );

    assert(file.good());
}

void test_blob_from_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-blob-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "blob-test.txt";

    write_binary_file(
        path,
        "Hello Mini Git!"
    );

    const Blob blob =
        Blob::from_file(path);

    const std::string expected =
        std::string("blob 15\0", 8)
        + "Hello Mini Git!";

    assert(
        blob.serialize() == expected
    );

    std::filesystem::remove_all(root);
}

void test_blob_from_binary_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-blob-binary-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "blob-binary-test.bin";

    const std::string data{
        '\0',
        '\x01',
        '\x7F',
        static_cast<char>(0xFF)
    };

    write_binary_file(path, data);

    const Blob blob =
        Blob::from_file(path);

    const std::string expected =
        std::string("blob 4\0", 7)
        + data;

    assert(
        blob.serialize() == expected
    );

    std::filesystem::remove_all(root);
}

void test_missing_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-blob-missing-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    bool failed = false;

    try {
        (void)Blob::from_file(
            root / "does-not-exist.txt"
        );
    }
    catch (const std::exception&) {
        failed = true;
    }

    assert(failed);

    std::filesystem::remove_all(root);
}

} // namespace

int main()
{
    test_blob_from_file();
    test_blob_from_binary_file();
    test_missing_file();

    std::cout
        << "All Blob tests passed.\n";

    return 0;
}