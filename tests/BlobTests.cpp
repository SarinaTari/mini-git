#include "Blob.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

void test_blob_from_file() {
    const std::filesystem::path path =
        "blob-test.txt";

    {
        std::ofstream file(path, std::ios::binary);
        file << "Hello Mini Git!";
    }

    Blob blob = Blob::from_file(path);

    const std::string expected =
        std::string("blob 15\0", 8)
        + "Hello Mini Git!";

    assert(blob.serialize() == expected);

    std::filesystem::remove(path);
}

void test_blob_from_binary_file() {
    const std::filesystem::path path =
        "blob-binary-test.bin";

    const char data[] = {
        '\0',
        '\x01',
        '\x7F',
        static_cast<char>(0xFF)
    };

    {
        std::ofstream file(path, std::ios::binary);
        file.write(data, sizeof(data));
    }

    Blob blob = Blob::from_file(path);

    const std::string result = blob.serialize();

    const std::string expected =
        std::string("blob 4\0", 7)
        + std::string(data, sizeof(data));

    assert(result == expected);

    std::filesystem::remove(path);
}

int main() {
    test_blob_from_file();
    test_blob_from_binary_file();

    std::cout << "All Blob tests passed.\n";

    return 0;
}
