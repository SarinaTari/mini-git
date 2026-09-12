#include "Status.hpp"

#include "Blob.hpp"
#include "Hash.hpp"

#include <filesystem>
#include <fstream>
#include <string>

Status::Status(
    const std::filesystem::path& root,
    Index& index
)
    : root_(root),
      index_(index) {
}

StatusResult Status::collect() {
    StatusResult result;

    collect_modified(result);
    collect_untracked(result);
    collect_merge_state(result);

    return result;
}

void Status::collect_modified(
    StatusResult& result
) {
    for (const auto& entry : index_.entries()) {
        const std::filesystem::path file_path =
            root_ / entry.path;

        if (!std::filesystem::exists(file_path)) {
            result.deleted.push_back(entry.path);
            continue;
        }

        if (!std::filesystem::is_regular_file(file_path)) {
            continue;
        }

        Blob blob =
            Blob::from_file(file_path);

        const std::string current_object_id =
            Hash::sha256(
                blob.serialize()
            );

        if (
            current_object_id !=
            entry.object_id
        ) {
            result.modified.push_back(
                entry.path
            );
        }
    }
}

void Status::collect_untracked(
    StatusResult& result
) {
    std::filesystem::recursive_directory_iterator iterator(
        root_
    );

    const std::filesystem::recursive_directory_iterator end;

    while (iterator != end) {
        const auto& entry = *iterator;

        if (
            entry.is_directory() &&
            entry.path().filename() ==
            ".mini-git"
        ) {
            iterator.disable_recursion_pending();
            ++iterator;
            continue;
        }

        if (!entry.is_regular_file()) {
            ++iterator;
            continue;
        }

        const std::filesystem::path relative_path =
            std::filesystem::relative(
                entry.path(),
                root_
            );

        const std::string path =
            relative_path.generic_string();

        if (!index_.contains(path)) {
            result.untracked.push_back(path);
        }

        ++iterator;
    }
}

void Status::collect_merge_state(
    StatusResult& result
) {
    const auto git_directory =
        root_ / ".mini-git";

    const auto merge_head =
        git_directory / "MERGE_HEAD";

    if (!std::filesystem::exists(
            merge_head
        )) {
        return;
    }

    result.merge_in_progress = true;

    std::ifstream conflicts_file(
        git_directory / "MERGE_CONFLICTS"
    );

    if (!conflicts_file) {
        return;
    }

    std::string path;

    while (std::getline(
        conflicts_file,
        path
    )) {
        if (!path.empty()) {
            result.conflicts.push_back(path);
        }
    }
}