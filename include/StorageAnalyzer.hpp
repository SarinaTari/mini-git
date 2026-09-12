#pragma once

#include <string>

class Repository;

class StorageAnalyzer {

public:

    explicit StorageAnalyzer(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};