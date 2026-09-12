#include "Index.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void test_add_entry()
{
    Index index("unused-index");

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

void test_update_entry()
{
    Index index("unused-index");

    index.add({
        "main.cpp",
        "abc123"
    });

    index.add({
        "main.cpp",
        "def456"
    });

    assert(index.entries().size() == 1);
    assert(
        index.entries()[0].object_id ==
        "def456"
    );
}

void test_multiple_entries()
{
    Index index("unused-index");

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

void test_persistence()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-index-persistence-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "index";

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
        loaded.entries()[0].object_id ==
        "abc123"
    );

    assert(
        loaded.entries()[1].object_id ==
        "def456"
    );

    std::filesystem::remove_all(root);
}

void test_update_persistence()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-index-update-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "index";

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
        loaded.entries()[0].object_id ==
        "def456"
    );

    std::filesystem::remove_all(root);
}

void test_malformed_entry_is_rejected()
{
    const auto root =
        std::filesystem::temp_directory_path()
        / "mini-git-index-invalid-tests";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    const auto path =
        root / "index";

    {
        std::ofstream file(path);

        assert(file);

        file << "valid.cpp\tabc123\n";
        file << "malformed-entry\n";
    }

    Index index(path);

    bool failed = false;

    try {
        index.load();
    }
    catch (const std::runtime_error&) {
        failed = true;
    }

    assert(failed);

    std::filesystem::remove_all(root);
}

int main()
{
    test_add_entry();
    test_update_entry();
    test_multiple_entries();
    test_persistence();
    test_update_persistence();
    test_malformed_entry_is_rejected();

    std::cout
        << "All Index tests passed.\n";

    return 0;
}