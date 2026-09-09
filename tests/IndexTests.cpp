#include "Index.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

void test_add_entry() {
    Index index("index");

    index.add({
        "main.cpp",
        "abc123"
    });

    assert(index.contains("main.cpp"));
    assert(!index.contains("README.md"));

    assert(index.entries().size() == 1);
    assert(index.entries()[0].path == "main.cpp");
    assert(index.entries()[0].object_id == "abc123");
}

void test_update_entry() {
    Index index("index");

    index.add({
        "main.cpp",
        "abc123"
    });

    index.add({
        "main.cpp",
        "def456"
    });

    assert(index.entries().size() == 1);
    assert(index.entries()[0].object_id == "def456");
}

void test_multiple_entries() {
    Index index("index");

    index.add({
        "main.cpp",
        "abc123"
    });

    index.add({
        "README.md",
        "def456"
    });

    index.add({
        "src/App.cpp",
        "ghi789"
    });

    assert(index.entries().size() == 3);

    assert(index.contains("main.cpp"));
    assert(index.contains("README.md"));
    assert(index.contains("src/App.cpp"));
}

void test_persistence() {
    const std::filesystem::path path =
        "index-test.txt";

    std::filesystem::remove(path);

    Index index(path);

    index.add({
        "main.cpp",
        "abc123"
    });

    index.add({
        "README.md",
        "def456"
    });

    index.save();

    Index loaded(path);

    loaded.load();

    assert(loaded.entries().size() == 2);

    assert(loaded.contains("main.cpp"));
    assert(loaded.contains("README.md"));

    assert(
        loaded.entries()[0].object_id == "abc123"
    );

    assert(
        loaded.entries()[1].object_id == "def456"
    );

    std::filesystem::remove(path);
}

void test_update_persistence() {
    const std::filesystem::path path =
        "index-update-test.txt";

    std::filesystem::remove(path);

    Index index(path);

    index.add({
        "main.cpp",
        "abc123"
    });

    index.add({
        "main.cpp",
        "def456"
    });

    index.save();

    Index loaded(path);

    loaded.load();

    assert(loaded.entries().size() == 1);

    assert(
        loaded.entries()[0].object_id == "def456"
    );

    std::filesystem::remove(path);
}

int main() {
    test_add_entry();
    test_update_entry();
    test_multiple_entries();
    test_persistence();
    test_update_persistence();

    std::cout << "All Index tests passed.\n";

    return 0;
}