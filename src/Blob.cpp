#include "Blob.hpp"

#include <utility>

Blob::Blob(std::string content)
    : content_(std::move(content)) {
}

std::string Blob::serialize() const {
    return content_;
}