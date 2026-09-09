#pragma once

#include "Object.hpp"

#include <string>

class Commit : public Object {
public:
    Commit(
        std::string tree_id,
        std::string parent_id,
        std::string author,
        std::string message
    );

    std::string serialize() const override;

private:
    std::string tree_id_;
    std::string parent_id_;
    std::string author_;
    std::string message_;
};