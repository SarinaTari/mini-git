#include "Reference.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

Reference::Reference(
    const std::filesystem::path& git_dir,
    std::string name
)
    : git_dir_(git_dir),
      name_(std::move(name)) {

    validate_name();
}

bool Reference::exists() const {
    return std::filesystem::exists(path());
}

std::string Reference::read() const {
    const std::filesystem::path reference_path =
        path();

    if (!std::filesystem::exists(reference_path)) {
        throw std::runtime_error(
            "Reference does not exist: " +
            name_
        );
    }

    std::ifstream file(reference_path);

    if (!file) {
        throw std::runtime_error(
            "Failed to read reference: " +
            name_
        );
    }

    std::string object_id;

    if (!std::getline(file, object_id)) {
        return "";
    }

    return object_id;
}

void Reference::write(
    const std::string& object_id
) const {
    if (object_id.empty()) {
        throw std::invalid_argument(
            "Reference cannot point to an empty object ID"
        );
    }

    const std::filesystem::path reference_path =
        path();

    std::filesystem::create_directories(
        reference_path.parent_path()
    );

    std::ofstream file(
        reference_path,
        std::ios::trunc
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to write reference: " +
            name_
        );
    }

    file << object_id << '\n';

    if (!file) {
        throw std::runtime_error(
            "Failed to write reference: " +
            name_
        );
    }
}

const std::string& Reference::name() const {
    return name_;
}

std::filesystem::path Reference::path() const {
    return git_dir_ / name_;
}

void Reference::validate_name() const {
    if (name_.empty()) {
        throw std::invalid_argument(
            "Reference name cannot be empty"
        );
    }

    const std::filesystem::path reference_path(name_);

    if (reference_path.is_absolute()) {
        throw std::invalid_argument(
            "Reference name must be relative"
        );
    }

    for (const auto& component : reference_path) {
        if (component == "..") {
            throw std::invalid_argument(
                "Reference name cannot contain '..'"
            );
        }
    }

    if (
        name_ == "." ||
        name_ == ".."
    ) {
        throw std::invalid_argument(
            "Invalid reference name"
        );
    }
}