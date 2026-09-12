#include "Stats.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <filesystem>
#include <sstream>
#include <string>

Stats::Stats(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Stats::render() const {

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::size_t blobs = 0;
    std::size_t trees = 0;
    std::size_t commits = 0;
    std::uintmax_t storage = 0;

    for (const auto& object_id :
         database.object_ids()) {

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

        if (data.rfind("blob ", 0) == 0) {
            ++blobs;
            continue;
        }

        if (data.rfind("tree ", 0) == 0) {

            try {
                (void)Commit::deserialize(data);
                ++commits;
            }
            catch (...) {
                ++trees;
            }
        }
    }

    std::ostringstream output;

    output
        << "Mini Git Repository Statistics\n\n";

    output
        << "Objects:\n"
        << "  Total:   "
        << database.object_ids().size()
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

    output
        << "References:\n"
        << "  Branches: "
        << repository_.branches().size()
        << '\n'
        << "  Tags:     "
        << repository_.tags().size()
        << '\n';

    if (!repository_.head_commit().empty()) {
        output
            << "  HEAD:     "
            << repository_.head_commit()
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