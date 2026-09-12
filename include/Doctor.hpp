#pragma once

#include <string>

class Repository;

class Doctor {

public:

    explicit Doctor(
        const Repository& repository
    );

    std::string render() const;

private:

    const Repository& repository_;

};