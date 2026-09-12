#include "Hash.hpp"

#include <cassert>
#include <iostream>
#include <string>

void test_empty_string() {
    const std::string result = Hash::sha256("");

    assert(
        result ==
        "e3b0c44298fc1c149afbf4c8996fb924"
        "27ae41e4649b934ca495991b7852b855"
    );
}

void test_hello() {
    const std::string result = Hash::sha256("hello");

    assert(
        result ==
        "2cf24dba5fb0a30e26e83b2ac5b9e29e"
        "1b161e5c1fa7425e73043362938b9824"
    );
}

void test_deterministic() {
    const std::string first = Hash::sha256("Mini Git");
    const std::string second = Hash::sha256("Mini Git");

    assert(first == second);
}

void test_different_inputs() {
    const std::string first = Hash::sha256("hello");
    const std::string second = Hash::sha256("Hello");

    assert(first != second);
}

void test_binary_data() {
    const std::string data("\0abc", 4);

    const std::string result = Hash::sha256(data);

    assert(!result.empty());
    assert(result.size() == 64);
}

int main() {
    test_empty_string();
    test_hello();
    test_deterministic();
    test_different_inputs();
    test_binary_data();

    std::cout << "All Hash tests passed.\n";

    return 0;
}
