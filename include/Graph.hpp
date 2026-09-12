#pragma once

#include <filesystem>
#include <string>

class Repository;

class Graph {

public:

    explicit Graph(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};