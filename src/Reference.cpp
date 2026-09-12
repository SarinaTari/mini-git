#include "Reference.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

Reference::Reference(
    const std::filesystem::path& git_directory,
    std::string name
)
    : git_directory_(git_directory),
      name_(std::move(name))
{
    validate_name(name_);
}

const std::string& Reference::name() const
{
    return name_;
}

std::filesystem::path Reference::path() const
{
    return git_directory_ / name_;
}

bool Reference::exists() const
{
    return std::filesystem::is_regular_file(path());
}

std::string Reference::read() const
{
    if (!exists()) {
        throw std::runtime_error(
            "Reference does not exist: " + name_
        );
    }

    std::ifstream file(path());

    if (!file) {
        throw std::runtime_error(
            "Failed to open reference: " + name_
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    std::string value = buffer.str();

    while (!value.empty() &&
           (value.back() == '\n' || value.back() == '\r')) {
        value.pop_back();
    }

    return value;
}

void Reference::write(const std::string& object_id) const
{
    if (object_id.empty()) {
        throw std::invalid_argument(
            "Reference cannot point to an empty object ID"
        );
    }

    const auto reference_path = path();

    std::filesystem::create_directories(
        reference_path.parent_path()
    );

    std::ofstream file(reference_path);

    if (!file) {
        throw std::runtime_error(
            "Failed to write reference: " + name_
        );
    }

    file << object_id << '\n';
}

void Reference::validate_name(const std::string& name)
{
    if (name.empty()) {
        throw std::invalid_argument(
            "Reference name cannot be empty"
        );
    }

    std::filesystem::path path(name);

    if (path.is_absolute()) {
        throw std::invalid_argument(
            "Reference name must be relative"
        );
    }

    for (const auto& component : path) {
        const std::string part = component.string();

        if (part.empty() || part == ".") {
            throw std::invalid_argument(
                "Invalid reference name: " + name
            );
        }

        if (part == "..") {
            throw std::invalid_argument(
                "Reference name cannot contain '..': " + name
            );
        }
    }
}
