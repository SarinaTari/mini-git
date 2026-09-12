#pragma once

#include <string>

class Repository;

class Impact {

public:

    explicit Impact(
        const Repository& repository
    );

    std::string render(
        const std::string& commit_id
    ) const;

private:

    const Repository& repository_;

};