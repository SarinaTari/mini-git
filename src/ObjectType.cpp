#include "ObjectType.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "Tree.hpp"

#include <stdexcept>

ObjectType detect_object_type(
    const std::string& serialized
)
{
    /*
     * Blob objects have the form:
     *
     *     blob <size>\0<content>
     *
     * A tree entry can also begin with "blob ", so Blob
     * deserialization must be attempted rather than relying
     * only on the textual prefix.
     */
    if (serialized.rfind("blob ", 0) == 0) {
        try {
            (void)Blob::deserialize(serialized);
            return ObjectType::Blob;
        }
        catch (const std::exception&) {
            // It may be a tree beginning with a blob entry.
        }
    }

    /*
     * Commit objects begin with:
     *
     *     tree <object-id>
     *
     * A tree containing a tree entry also begins with "tree ",
     * so commit deserialization is attempted first.
     */
    if (serialized.rfind("tree ", 0) == 0) {
        try {
            (void)Commit::deserialize(serialized);
            return ObjectType::Commit;
        }
        catch (const std::exception&) {
            // Try interpreting it as a tree below.
        }
    }

    /*
     * Trees consist of entries beginning with either:
     *
     *     blob <object-id> <name>
     *     tree <object-id> <name>
     *
     * Therefore both prefixes can represent trees.
     */
    if (
        serialized.rfind("blob ", 0) == 0 ||
        serialized.rfind("tree ", 0) == 0
    ) {
        try {
            (void)Tree::deserialize(serialized);
            return ObjectType::Tree;
        }
        catch (const std::exception&) {
            // Fall through to the final error.
        }
    }

    throw std::runtime_error(
        "Unknown or invalid object format"
    );
}

std::string object_type_name(
    ObjectType type
)
{
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