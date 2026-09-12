#include "FileReader.hpp"

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

void test_text_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-file-reader-text-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "file-reader-test.txt";

    write_binary_file(
        path,
        "Hello Mini Git!"
    );

    const std::string result =
        FileReader::read(path);

    assert(
        result == "Hello Mini Git!"
    );

    std::filesystem::remove_all(root);
}

void test_binary_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-file-reader-binary-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "file-reader-binary-test.bin";

    const std::string data{
        '\0',
        '\x01',
        '\x7F',
        static_cast<char>(0xFF)
    };

    write_binary_file(path, data);

    const std::string result =
        FileReader::read(path);

    assert(result.size() == 4);
    assert(
        static_cast<unsigned char>(result[0])
        == 0x00
    );
    assert(
        static_cast<unsigned char>(result[1])
        == 0x01
    );
    assert(
        static_cast<unsigned char>(result[2])
        == 0x7F
    );
    assert(
        static_cast<unsigned char>(result[3])
        == 0xFF
    );

    std::filesystem::remove_all(root);
}

void test_missing_file()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-file-reader-missing-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    bool threw = false;

    try {
        (void)FileReader::read(
            root / "does-not-exist.bin"
        );
    }
    catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);

    std::filesystem::remove_all(root);
}

} // namespace

int main()
{
    test_text_file();
    test_binary_file();
    test_missing_file();

    std::cout
        << "All FileReader tests passed.\n";

    return 0;
}