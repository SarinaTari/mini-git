#include "ObjectType.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "Tree.hpp"

#include <stdexcept>

ObjectType detect_object_type(
    const std::string& serialized
) {
    if (serialized.rfind("blob ", 0) == 0) {

        /*
         * A Tree entry also begins with "blob ", but a complete
         * Blob has the form:
         *
         *     blob <size>\0<content>
         *
         * Therefore we first try to deserialize it as a Blob.
         */
        try {
            (void)Blob::deserialize(serialized);
            return ObjectType::Blob;
        }
        catch (...) {
        }
    }

    /*
     * A Commit starts with:
     *
     *     tree <object-id>
     *
     * A Tree entry can also start with "tree ", so try Commit
     * before Tree.
     */
    if (serialized.rfind("tree ", 0) == 0) {

        try {
            (void)Commit::deserialize(serialized);
            return ObjectType::Commit;
        }
        catch (...) {
        }

        try {
            (void)Tree::deserialize(serialized);
            return ObjectType::Tree;
        }
        catch (...) {
        }
    }

    /*
     * A Tree can contain blob entries, so an object beginning
     * with "blob " may also be a Tree. This is checked after
     * attempting Blob deserialization.
     */
    if (
        serialized.rfind("blob ", 0) == 0
        || serialized.rfind("tree ", 0) == 0
    ) {
        try {
            (void)Tree::deserialize(serialized);
            return ObjectType::Tree;
        }
        catch (...) {
        }
    }

    throw std::runtime_error(
        "Unknown or invalid object format"
    );
}

std::string object_type_name(
    ObjectType type
) {
    switch (type) {

        case ObjectType::Blob:
            return "blob";

        case ObjectType::Tree:
            return "tree";

        case ObjectType::Commit:
            return "commit";
    }

    throw std::runtime_error(
        "Unknown object type"
    );
}