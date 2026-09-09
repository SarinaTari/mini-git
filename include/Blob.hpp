#pragma once

#include "Object.hpp"

#include <string>

class Blob : public Object {
public:
    explicit Blob(std::string content);

    std::string serialize() const override;

private:
    std::string content_;
};