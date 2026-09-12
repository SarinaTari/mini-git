#pragma once

#include <filesystem>
#include <string>

class Repository;

class Analyzer {

public:

    explicit Analyzer(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};