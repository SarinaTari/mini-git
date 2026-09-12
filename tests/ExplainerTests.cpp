#include "Explainer.hpp"

#include <cassert>
#include <exception>
#include <iostream>
#include <string>

int main()
{
    const std::string add =
        Explainer::explain("add");

    assert(
        add.find("FileReader") !=
        std::string::npos
    );

    assert(
        add.find("SHA-256") !=
        std::string::npos
    );

    const std::string merge =
        Explainer::explain("merge");

    assert(
        merge.find("Three-Way Merge") !=
        std::string::npos
    );

    bool threw = false;

    try {
        (void)Explainer::explain(
            "unknown-command"
        );
    }
    catch (const std::exception&) {
        threw = true;
    }

    assert(threw);

    std::cout
        << "Explainer tests passed.\n";

    return 0;
}