#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Mini Git\n";
        return 0;
    }

    std::string command = argv[1];

    if (command == "--version") {
        std::cout << "mygit version 0.1.0\n";
        return 0;
    }

    std::cout << "Unknown command: " << command << '\n';
    return 1;
}