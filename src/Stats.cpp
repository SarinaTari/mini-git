#include "Stats.hpp"

#include "ObjectDatabase.hpp"
#include "ObjectType.hpp"
#include "Repository.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <string>

Stats::Stats(
    const Repository& repository
)
    : repository_(repository)
{
}

std::string Stats::render() const
{
    ObjectDatabase database(
        repository_.git_directory()
    );

    std::size_t blobs = 0;
    std::size_t trees = 0;
    std::size_t commits = 0;

    std::uintmax_t storage = 0;

    const auto object_ids =
        database.object_ids();

    for (const auto& object_id :
         object_ids) {
        const auto object_path =
            repository_.git_directory()
            / "objects"
            / object_id;

        std::error_code error;

        const auto size =
            std::filesystem::file_size(
                object_path,
                error
            );

        if (!error) {
            storage += size;
        }

        const std::string data =
            database.read(object_id);

        const ObjectType type =
            detect_object_type(data);

        switch (type) {
            case ObjectType::Blob:
                ++blobs;
                break;

            case ObjectType::Tree:
                ++trees;
                break;

            case ObjectType::Commit:
                ++commits;
                break;
        }
    }

    std::ostringstream output;

    output
        << "Mini Git Repository Statistics\n\n";

    output
        << "Objects:\n"
        << "  Total:   "
        << object_ids.size()
        << '\n'
        << "  Blobs:   "
        << blobs
        << '\n'
        << "  Trees:   "
        << trees
        << '\n'
        << "  Commits: "
        << commits
        << "\n\n";

    const auto branches =
        repository_.branches();

    const auto tags =
        repository_.tags();

    output
        << "References:\n"
        << "  Branches: "
        << branches.size()
        << '\n'
        << "  Tags:     "
        << tags.size()
        << '\n';

    const std::string head =
        repository_.head_commit();

    if (!head.empty()) {
        output
            << "  HEAD:     "
            << head
            << '\n';
    }
    else {
        output
            << "  HEAD:     (no commit)\n";
    }

    output
        << "\nObject storage:\n"
        << "  Size: "
        << storage
        << " bytes\n";

    return output.str();
}