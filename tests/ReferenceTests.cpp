#include "Reference.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void test_reference_write_and_read() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-reference-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    Reference reference(
        root / ".mini-git",
        "refs/heads/main"
    );

    assert(!reference.exists());

    reference.write(
        "abcdef123456"
    );

    assert(reference.exists());

    assert(
        reference.read() ==
        "abcdef123456"
    );

    assert(
        reference.name() ==
        "refs/heads/main"
    );

    std::filesystem::remove_all(root);
}

void test_reference_update() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-reference-update-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    Reference reference(
        root / ".mini-git",
        "refs/heads/main"
    );

    reference.write("first");

    assert(
        reference.read() ==
        "first"
    );

    reference.write("second");

    assert(
        reference.read() ==
        "second"
    );

    std::filesystem::remove_all(root);
}

void test_nested_reference() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-nested-reference-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    Reference reference(
        root / ".mini-git",
        "refs/heads/feature/login"
    );

    reference.write("commit123");

    assert(reference.exists());

    assert(
        reference.read() ==
        "commit123"
    );

    std::filesystem::remove_all(root);
}

void test_invalid_reference_names() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-invalid-reference-test";

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    bool threw = false;

    try {
        Reference reference(
            root / ".mini-git",
            "../outside"
        );
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);

    threw = false;

    try {
        Reference reference(
            root / ".mini-git",
            "/absolute/path"
        );
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);

    std::filesystem::remove_all(root);
}

void test_empty_object_id_is_rejected() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "mini-git-empty-reference-test";

    std::filesystem::remove_all(root);

    std::filesystem::create_directories(
        root / ".mini-git"
    );

    Reference reference(
        root / ".mini-git",
        "refs/heads/main"
    );

    bool threw = false;

    try {
        reference.write("");
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);

    std::filesystem::remove_all(root);
}

int main() {
    test_reference_write_and_read();
    test_reference_update();
    test_nested_reference();
    test_invalid_reference_names();
    test_empty_object_id_is_rejected();

    std::cout
        << "All Reference tests passed.\n";

    return 0;
}