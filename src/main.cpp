#include "Blob.hpp"
#include "FileReader.hpp"
#include "Hash.hpp"
#include "Index.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Status.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Mini Git\n";
        return 0;
    }

    const std::string command = argv[1];

    if (command == "--version") {
        std::cout
            << "mini-git version 0.1.0\n";

        return 0;
    }

    if (command == "init") {
        try {
            Repository repository(
                std::filesystem::current_path()
            );

            repository.initialize();

            std::cout
                << "Initialized empty Mini Git repository.\n";

            return 0;

        } catch (const std::exception& e) {
            std::cerr
                << "mini-git: "
                << e.what()
                << '\n';

            return 1;
        }
    }

    if (command == "hash-file") {
        if (argc < 3) {
            std::cerr
                << "mini-git: missing file path\n";

            return 1;
        }

        try {
            Blob blob =
                Blob::from_file(argv[2]);

            const std::string object_id =
                Hash::sha256(blob.serialize());

            std::cout
                << object_id
                << '\n';

            return 0;

        } catch (const std::exception& e) {
            std::cerr
                << "mini-git: "
                << e.what()
                << '\n';

            return 1;
        }
    }

    if (command == "hash-object") {
        if (argc < 3) {
            std::cerr
                << "mini-git: missing file path\n";

            return 1;
        }

        try {
            const std::filesystem::path file_path =
                argv[2];

            Blob blob =
                Blob::from_file(file_path);

            Repository repository(
                std::filesystem::current_path()
            );

            ObjectDatabase database(
                repository.git_directory()
            );

            const std::string object_id =
                database.store(blob);

            std::cout
                << object_id
                << '\n';

            return 0;

        } catch (const std::exception& e) {
            std::cerr
                << "mini-git: "
                << e.what()
                << '\n';

            return 1;
        }
    }

    if (command == "add") {
        if (argc < 3) {
            std::cerr
                << "mini-git: missing file path\n";

            return 1;
        }

        try {
            const std::filesystem::path file_path =
                argv[2];

            Repository repository(
                std::filesystem::current_path()
            );

            if (!std::filesystem::exists(
                    repository.git_directory())) {
                throw std::runtime_error(
                    "Not a Mini Git repository"
                );
            }

            if (!std::filesystem::is_regular_file(
                    file_path)) {
                throw std::runtime_error(
                    "Not a regular file: " +
                    file_path.string()
                );
            }

            Blob blob =
                Blob::from_file(file_path);

            ObjectDatabase database(
                repository.git_directory()
            );

            const std::string object_id =
                database.store(blob);

            Index index(
                repository.git_directory() /
                "index"
            );

            index.load();

            index.add({
                file_path.string(),
                object_id
            });

            index.save();

            std::cout
                << "Staged: "
                << file_path
                << '\n';

            return 0;

        } catch (const std::exception& e) {
            std::cerr
                << "mini-git: "
                << e.what()
                << '\n';

            return 1;
        }
    }

    if (command == "status") {
        try {
            Repository repository(
                std::filesystem::current_path()
            );

            if (!std::filesystem::exists(
                    repository.git_directory())) {
                throw std::runtime_error(
                    "Not a Mini Git repository"
                );
            }

            Index index(
                repository.git_directory() /
                "index"
            );

            index.load();

            Status status(
                std::filesystem::current_path(),
                index
            );

            const StatusResult result =
                status.collect();

            std::cout
                << "On branch main\n\n";

            if (!result.modified.empty()) {
                std::cout
                    << "Changes not staged for commit:\n";

                for (const auto& path : result.modified) {
                    std::cout
                        << "  modified: "
                        << path
                        << '\n';
                }

                std::cout << '\n';
            }

            if (!result.deleted.empty()) {
                std::cout
                    << "Deleted files:\n";

                for (const auto& path : result.deleted) {
                    std::cout
                        << "  deleted: "
                        << path
                        << '\n';
                }

                std::cout << '\n';
            }

            if (!result.untracked.empty()) {
                std::cout
                    << "Untracked files:\n";

                for (const auto& path : result.untracked) {
                    std::cout
                        << "  "
                        << path
                        << '\n';
                }

                std::cout << '\n';
            }

            if (
                result.modified.empty() &&
                result.deleted.empty() &&
                result.untracked.empty()
            ) {
                std::cout
                    << "Working tree clean.\n";
            }

            return 0;

        } catch (const std::exception& e) {
            std::cerr
                << "mini-git: "
                << e.what()
                << '\n';

            return 1;
        }
    }

    std::cout
        << "Unknown command: "
        << command
        << '\n';

    return 1;
}