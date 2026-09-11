#include "Blob.hpp"
#include "Commit.hpp"
#include "FileReader.hpp"
#include "Hash.hpp"
#include "Index.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Status.hpp"
#include "TreeBuilder.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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

            if (repository.is_detached()) {
                std::cout
                    << "HEAD detached at "
                    << repository.head_commit()
                    << "\n\n";
            } else {
                std::cout
                    << "On branch "
                    << repository.current_branch()
                    << "\n\n";
            }

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

    if (command == "commit") {
        if (
            argc < 4 ||
            std::string(argv[2]) != "-m"
        ) {
            std::cerr
                << "mini-git: usage: "
                << "mini-git commit -m \"message\"\n";

            return 1;
        }

        try {
            const std::string message = argv[3];

            if (message.empty()) {
                throw std::runtime_error(
                    "Commit message cannot be empty"
                );
            }

            Repository repository(
                std::filesystem::current_path()
            );

            if (!std::filesystem::exists(
                    repository.git_directory())) {
                throw std::runtime_error(
                    "Not a Mini Git repository"
                );
            }

            if (repository.is_detached()) {
                throw std::runtime_error(
                    "Cannot commit while HEAD is detached"
                );
            }

            Index index(
                repository.git_directory() /
                "index"
            );

            index.load();

            if (index.entries().empty()) {
                throw std::runtime_error(
                    "Nothing to commit"
                );
            }

            ObjectDatabase database(
                repository.git_directory()
            );

            TreeBuilder tree_builder(database);

            const std::string tree_id =
                tree_builder.build_from_index(
                    index,
                    std::filesystem::current_path()
                );

            const std::string parent_id =
                repository.head_commit();

            const char* user =
                std::getenv("USER");

            const std::string author =
                user && *user
                    ? user
                    : "unknown";

            Commit commit(
                tree_id,
                parent_id,
                author,
                message
            );

            const std::string commit_id =
                database.store(commit);

            const std::string branch =
                repository.current_branch();

            repository.update_branch(
                branch,
                commit_id
            );

            std::cout
                << "["
                << branch
                << " "
                << commit_id.substr(0, 7)
                << "] "
                << message
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

    if (command == "log") {
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

            const std::string current_commit_id =
                repository.head_commit();

            if (current_commit_id.empty()) {
                std::cout
                    << "No commits yet.\n";

                return 0;
            }

            ObjectDatabase database(
                repository.git_directory()
            );

            std::string commit_id =
                current_commit_id;

            while (!commit_id.empty()) {
                const std::string data =
                    database.read(commit_id);

                Commit commit =
                    Commit::deserialize(data);

                std::cout
                    << "commit "
                    << commit_id
                    << '\n';

                std::cout
                    << "Author: "
                    << commit.author()
                    << '\n';

                std::cout << '\n';

                std::cout
                    << "    "
                    << commit.message()
                    << '\n';

                std::cout << '\n';

                commit_id =
                    commit.parent_id();
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

    if (command == "branch") {
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

            const std::vector<std::string> branches =
                repository.branches();

            const std::string current_branch =
                repository.current_branch();

            if (branches.empty()) {
                if (!current_branch.empty()) {
                    std::cout
                        << "* "
                        << current_branch
                        << '\n';
                }

                return 0;
            }

            for (const auto& branch : branches) {
                if (branch == current_branch) {
                    std::cout
                        << "* ";
                } else {
                    std::cout
                        << "  ";
                }

                std::cout
                    << branch
                    << '\n';
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