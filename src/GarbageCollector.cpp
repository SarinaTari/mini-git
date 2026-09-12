#include "GarbageCollector.hpp"

#include "ObjectDatabase.hpp"
#include "Reachability.hpp"

#include <filesystem>
#include <sstream>

GarbageCollector::GarbageCollector(
    const std::filesystem::path& git_directory
)
    : git_directory_(git_directory) {
}

GarbageCollectionReport
GarbageCollector::preview() const
{
    GarbageCollectionReport report;

    Reachability reachability(
        git_directory_
    );

    report.unreachable_objects =
        reachability.unreachable_objects();

    ObjectDatabase database(
        git_directory_
    );

    for (const auto& object_id :
         report.unreachable_objects) {

        const auto path =
            git_directory_
            / "objects"
            / object_id;

        std::error_code error;

        const auto size =
            std::filesystem::file_size(
                path,
                error
            );

        if (!error) {
            report.reclaimable_bytes += size;
        }
    }

    return report;
}

std::string GarbageCollector::render() const
{
    const auto report =
        preview();

    std::ostringstream output;

    output
        << "Mini Git Garbage Collection Preview\n\n";

    output
        << "Unreachable objects: "
        << report.unreachable_objects.size()
        << '\n';

    output
        << "Potentially reclaimable storage: "
        << report.reclaimable_bytes
        << " bytes\n";

    if (
        report.unreachable_objects.empty()
    ) {
        output
            << "\nNothing would be removed.\n";
    }
    else {
        output
            << "\nObjects eligible for cleanup:\n";

        for (const auto& object_id :
             report.unreachable_objects) {

            output
                << "  "
                << object_id
                << '\n';
        }

        output
            << "\nDry run only. No objects were deleted.\n";
    }

    return output.str();
}