#pragma once

#include <string>

enum class ObjectType {
    Blob,
    Tree,
    Commit
};

ObjectType detect_object_type(
    const std::string& serialized
);

std::string object_type_name(
    ObjectType type
);