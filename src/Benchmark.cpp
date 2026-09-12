#include "Benchmark.hpp"

#include "Hash.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"

#include <chrono>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace {

using Clock =
    std::chrono::steady_clock;

double milliseconds(
    const Clock::time_point& start,
    const Clock::time_point& end
) {
    return std::chrono::duration<double, std::milli>(
        end - start
    ).count();
}

}

Benchmark::Benchmark(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Benchmark::render() const {

    const std::string sample =
        "Mini Git benchmark data\n"
        "This measures hashing and object lookup.\n";

    constexpr std::size_t iterations =
        1000;

    auto start =
        Clock::now();

    std::string last_hash;

    for (
        std::size_t i = 0;
        i < iterations;
        ++i
    ) {

        last_hash =
            Hash::sha256(sample);
    }

    auto end =
        Clock::now();

    const double hash_time =
        milliseconds(start, end);

    ObjectDatabase database(
        repository_.git_directory()
    );

    const auto object_ids =
        database.object_ids();

    double lookup_time = 0.0;

    if (!object_ids.empty()) {

        start =
            Clock::now();

        for (
            std::size_t i = 0;
            i < iterations;
            ++i
        ) {

            (void)database.exists(
                object_ids[
                    i % object_ids.size()
                ]
            );
        }

        end =
            Clock::now();

        lookup_time =
            milliseconds(start, end);
    }

    std::ostringstream output;

    output
        << "Mini Git Benchmark\n"
        << "==================\n\n";

    output
        << "Iterations: "
        << iterations
        << "\n\n";

    output
        << "SHA-256 hashing:\n"
        << "  Total: "
        << hash_time
        << " ms\n"
        << "  Average: "
        << (
            hash_time /
            static_cast<double>(
                iterations
            )
        )
        << " ms\n\n";

    output
        << "Object existence lookup:\n";

    if (object_ids.empty()) {

        output
            << "  No objects available\n";
    }
    else {

        output
            << "  Total: "
            << lookup_time
            << " ms\n"
            << "  Average: "
            << (
                lookup_time /
                static_cast<double>(
                    iterations
                )
            )
            << " ms\n";
    }

    output
        << "\nObject database:\n"
        << "  Objects: "
        << object_ids.size()
        << '\n';

    return output.str();
}