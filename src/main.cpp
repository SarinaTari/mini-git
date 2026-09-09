#include "Repository.hpp"

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

    std::cout << "Unknown command: " << command << '\n';
    return 1;
}