#pragma once

#include <filesystem>
#include <string>

class Repository;

class Stats {

public:

    explicit Stats(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};