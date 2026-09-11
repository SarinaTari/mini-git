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

namespace {

void print_usage()
{
    std::cout
        << "Mini Git\n\n"
        << "Usage:\n"
        << "  mini-git --version\n"
        << "  mini-git init\n"
        << "  mini-git hash-file <file>\n"
        << "  mini-git hash-object <file>\n"
        << "  mini-git add <file>\n"
        << "  mini-git status\n"
        << "  mini-git commit -m <message>\n"
        << "  mini-git log\n"
        << "  mini-git branch\n"
        << "  mini-git branch <name>\n"
        << "  mini-git checkout <name>\n";
}

Repository open_repository()
{
    return Repository(
        std::filesystem::current_path()
    );
}

void command_init()
{
    Repository repository(
        std::filesystem::current_path()
    );

    repository.initialize();

    std::cout
        << "Initialized empty Mini Git repository in "
        << repository.git_directory()
        << '\n';
}

void command_branch(
    Repository& repository,
    int argc,
    char* argv[]
)
{
    if (argc == 2) {
        const auto branches =
            repository.branches();

        if (branches.empty()) {
            std::cout << "No branches yet.\n";
            return;
        }

        const std::string current =
            repository.current_branch();

        for (const auto& branch : branches) {
            if (branch == current) {
                std::cout << "* ";
            }
            else {
                std::cout << "  ";
            }

            std::cout << branch << '\n';
        }

        return;
    }

    if (argc == 3) {
        repository.create_branch(argv[2]);

        std::cout
            << "Created branch '"
            << argv[2]
            << "'\n";

        return;
    }

    throw std::runtime_error(
        "Usage: mini-git branch [<name>]"
    );
}

void command_checkout(
    Repository& repository,
    const std::string& branch
)
{
    repository.checkout(branch);

    std::cout
        << "Switched to branch '"
        << branch
        << "'\n";
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        if (argc < 2) {
            print_usage();
            return 1;
        }

        const std::string command = argv[1];

        if (command == "--version") {
            std::cout << "Mini Git version 0.1.0\n";
            return 0;
        }

        if (command == "init") {
            command_init();
            return 0;
        }

        if (command == "branch") {
            Repository repository = open_repository();

            command_branch(
                repository,
                argc,
                argv
            );

            return 0;
        }

        if (command == "checkout") {
            if (argc != 3) {
                throw std::runtime_error(
                    "Usage: mini-git checkout <branch>"
                );
            }

            Repository repository = open_repository();

            command_checkout(
                repository,
                argv[2]
            );

            return 0;
        }

        if (command == "hash-file") {
            if (argc != 3) {
                throw std::runtime_error(
                    "Usage: mini-git hash-file <file>"
                );
            }

            std::cout
                << Hash::sha256(
                    FileReader::read(argv[2])
                )
                << '\n';

            return 0;
        }

        if (command == "hash-object") {
            if (argc != 3) {
                throw std::runtime_error(
                    "Usage: mini-git hash-object <file>"
                );
            }

            const Blob blob =
                Blob::from_file(argv[2]);

            Repository repository =
                open_repository();

            ObjectDatabase database(
                repository.git_directory()
            );

            std::cout
                << database.store(blob)
                << '\n';

            return 0;
        }

        if (command == "add") {
            if (argc != 3) {
                throw std::runtime_error(
                    "Usage: mini-git add <file>"
                );
            }

            Repository repository =
                open_repository();

            const auto file =
                std::filesystem::absolute(argv[2]);

            const auto relative =
                std::filesystem::relative(
                    file,
                    std::filesystem::current_path()
                );

            Blob blob =
                Blob::from_file(file);

            ObjectDatabase database(
                repository.git_directory()
            );

            const std::string object_id =
                database.store(blob);

            Index index(
                repository.git_directory()
            );

            index.load();

            index.add(
                IndexEntry{
                    relative.generic_string(),
                    object_id
                }
            );

            index.save();

            std::cout
                << "Added "
                << relative.generic_string()
                << '\n';

            return 0;
        }

        if (command == "status") {
            Repository repository =
                open_repository();

            if (!repository.is_detached_head()) {
                std::cout
                    << "On branch "
                    << repository.current_branch()
                    << '\n';
            }
            else {
                const std::string commit =
                    repository.head_commit();

                if (commit.empty()) {
                    std::cout
                        << "HEAD detached\n";
                }
                else {
                    std::cout
                        << "HEAD detached at "
                        << commit.substr(
                            0,
                            7
                        )
                        << '\n';
                }
            }

            return 0;
        }

        if (command == "commit") {
            if (
                argc != 4 ||
                std::string(argv[2]) != "-m"
            ) {
                throw std::runtime_error(
                    "Usage: mini-git commit -m <message>"
                );
            }

            Repository repository =
                open_repository();

            if (repository.is_detached_head()) {
                throw std::runtime_error(
                    "Cannot commit on detached HEAD "
                    "in Phase 13"
                );
            }

            Index index(
                repository.git_directory()
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

            TreeBuilder tree_builder(
                database
            );

            const std::string tree_id =
                tree_builder.build_from_index(
                    index,
                    repository.root()
                );

            const std::string parent =
                repository.head_commit();

            const char* user =
                std::getenv("USER");

            const std::string author =
                user ? user : "unknown";

            Commit commit(
                tree_id,
                parent,
                author,
                argv[3]
            );

            const std::string commit_id =
                database.store(commit);

            repository.update_branch(
                repository.current_branch(),
                commit_id
            );

            std::cout
                << "["
                << repository.current_branch()
                << " "
                << commit_id.substr(0, 7)
                << "] "
                << argv[3]
                << '\n';

            return 0;
        }

        if (command == "log") {
            Repository repository =
                open_repository();

            ObjectDatabase database(
                repository.git_directory()
            );

            std::string current =
                repository.head_commit();

            if (current.empty()) {
                std::cout
                    << "No commits yet.\n";

                return 0;
            }

            while (!current.empty()) {
                const std::string data =
                    database.read(current);

                Commit commit =
                    Commit::deserialize(data);

                std::cout
                    << "commit "
                    << current
                    << '\n';

                std::cout
                    << "Author: "
                    << commit.author()
                    << '\n';

                std::cout
                    << '\n'
                    << "    "
                    << commit.message()
                    << '\n'
                    << '\n';

                current =
                    commit.parent_id();
            }

            return 0;
        }

        print_usage();
        return 1;
    }
    catch (const std::exception& error) {
        std::cerr
            << "Error: "
            << error.what()
            << '\n';

        return 1;
    }
}