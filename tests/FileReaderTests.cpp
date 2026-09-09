#include "FileReader.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

void test_text_file() {
    const std::filesystem::path path =
        "file-reader-test.txt";

    {
        std::ofstream file(path, std::ios::binary);
        file << "Hello Mini Git!";
    }

    const std::string result =
        FileReader::read(path);

    assert(result == "Hello Mini Git!");

    std::filesystem::remove(path);
}

void test_binary_file() {
    const std::filesystem::path path =
        "file-reader-binary-test.bin";

    {
        std::ofstream file(path, std::ios::binary);

        const char data[] = {
            '\0',
            '\x01',
            '\x7F',
            static_cast<char>(0xFF)
        };

        file.write(data, sizeof(data));
    }

    const std::string result =
        FileReader::read(path);

    assert(result.size() == 4);
    assert(static_cast<unsigned char>(result[0]) == 0x00);
    assert(static_cast<unsigned char>(result[1]) == 0x01);
    assert(static_cast<unsigned char>(result[2]) == 0x7F);
    assert(static_cast<unsigned char>(result[3]) == 0xFF);

    std::filesystem::remove(path);
}

void test_missing_file() {
    bool threw = false;

    try {
        FileReader::read(
            "this-file-does-not-exist.bin"
        );
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
}

int main() {
    test_text_file();
    test_binary_file();
    test_missing_file();

    std::cout << "All FileReader tests passed.\n";

    return 0;
}