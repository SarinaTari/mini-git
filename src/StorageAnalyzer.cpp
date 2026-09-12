#include "StorageAnalyzer.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <filesystem>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct ObjectInfo {

    std::string id;

    std::string type;

    std::uintmax_t size = 0;

};

std::string object_type(
    const std::string& data
) {

    if (data.rfind("blob ", 0) == 0) {
        return "blob";
    }

    if (data.rfind("tree ", 0) == 0) {

        try {
            (void)Commit::deserialize(data);
            return "commit";
        }
        catch (...) {
            return "tree";
        }
    }

    return "unknown";
}

}

StorageAnalyzer::StorageAnalyzer(
    const Repository& repository
)
    : repository_(repository) {
}

std::string StorageAnalyzer::render() const {

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::vector<ObjectInfo> objects;

    std::map<
        std::string,
        std::size_t
    > counts;

    std::map<
        std::string,
        std::uintmax_t
    > bytes;

    for (const auto& id :
         database.object_ids()) {

        const auto path =
            repository_.git_directory()
            / "objects"
            / id;

        std::error_code error;

        const auto size =
            std::filesystem::file_size(
                path,
                error
            );

        if (error) {
            continue;
        }

        const std::string data =
            database.read(id);

        const std::string type =
            object_type(data);

        objects.push_back({
            id,
            type,
            size
        });

        ++counts[type];

        bytes[type] += size;
    }

    std::sort(
        objects.begin(),
        objects.end(),
        [](const ObjectInfo& left,
           const ObjectInfo& right) {

            if (left.size != right.size) {
                return left.size > right.size;
            }

            return left.id < right.id;
        }
    );

    std::uintmax_t total = 0;

    for (const auto& object :
         objects) {

        total += object.size;
    }

    std::ostringstream output;

    output
        << "Mini Git Storage Intelligence\n"
        << "==============================\n\n";

    output
        << "Total objects: "
        << objects.size()
        << '\n';

    output
        << "Total storage: "
        << total
        << " bytes\n\n";

    output << "By type:\n";

    const std::vector<std::string> types = {
        "blob",
        "tree",
        "commit",
        "unknown"
    };

    for (const auto& type :
         types) {

        if (counts[type] == 0) {
            continue;
        }

        output
            << "  "
            << type
            << ": "
            << counts[type]
            << " object";

        if (counts[type] != 1) {
            output << 's';
        }

        output
            << ", "
            << bytes[type]
            << " bytes\n";
    }

    output
        << "\nLargest objects:\n";

    const std::size_t limit =
        std::min<std::size_t>(
            10,
            objects.size()
        );

    for (std::size_t i = 0;
         i < limit;
         ++i) {

        const auto& object =
            objects[i];

        output
            << "  "
            << object.id
            << "  "
            << object.type
            << "  "
            << object.size
            << " bytes\n";
    }

    if (objects.empty()) {
        output << "  (none)\n";
    }

    return output.str();
}