#include "Repository.hpp"
#include "Blob.hpp"
#include "FileReader.hpp"
#include "Hash.hpp"
#include "ObjectDatabase.hpp"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Mini Git\n";
        return 0;
    }

    std::string command = argv[1];

    if (command == "--version") {
        std::cout << "mini-git version 0.1.0\n";
        return 0;
    }

    if (command == "init") {
        try {
            Repository repository(std::filesystem::current_path());
            repository.initialize();

            std::cout << "Initialized empty Mini Git repository.\n";
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "mini-git: " << e.what() << '\n';
            return 1;
        }
    }

    if (command == "hash-object") {
        if (argc < 3) {
            std::cerr << "mini-git: missing file path\n";
            return 1;
        }

        try {
            const std::filesystem::path file_path = argv[2];

            Blob blob = Blob::from_file(file_path);

            Repository repository(
                std::filesystem::current_path()
            );

            ObjectDatabase database(
                repository.git_directory()
            );

            const std::string object_id =
                database.store(blob);

            std::cout << object_id << '\n';

            return 0;
        } catch (const std::exception& e) {
            std::cerr << "mini-git: " << e.what() << '\n';
            return 1;
        }
    }

    if (command == "hash-file") {
        if (argc < 3) {
            std::cerr << "mini-git: missing file path\n";
            return 1;
        }

        try {
            const std::string content =
                FileReader::read(argv[2]);

            Blob blob(content);

            const std::string object_id =
                Hash::sha256(blob.serialize());

            std::cout << object_id << '\n';

            return 0;
        } catch (const std::exception& e) {
            std::cerr << "mini-git: " << e.what() << '\n';
            return 1;
        }
    }

    std::cout << "Unknown command: " << command << '\n';
    return 1;
}