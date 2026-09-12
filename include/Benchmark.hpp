#pragma once

#include <string>

class Repository;

class Benchmark {

public:

    explicit Benchmark(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};