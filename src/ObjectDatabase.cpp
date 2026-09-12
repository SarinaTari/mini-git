#include "ObjectDatabase.hpp"

#include "Hash.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool is_valid_object_id(const std::string& object_id)
{
    if (object_id.empty()) {
        return false;
    }

    for (const char character : object_id) {
        const bool is_hex =
            (character >= '0' && character <= '9') ||
            (character >= 'a' && character <= 'f') ||
            (character >= 'A' && character <= 'F');

        if (!is_hex) {
            return false;
        }
    }

    return true;
}

}

ObjectDatabase::ObjectDatabase(
    const std::filesystem::path& git_dir
)
    : objects_dir_(git_dir / "objects")
{
    std::filesystem::create_directories(
        objects_dir_
    );
}

std::string ObjectDatabase::store(
    const Object& object
)
{
    const std::string data =
        object.serialize();

    const std::string object_id =
        Hash::sha256(data);

    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    if (std::filesystem::exists(object_path)) {
        if (!std::filesystem::is_regular_file(object_path)) {
            throw std::runtime_error(
                "Object path is not a regular file: " +
                object_id
            );
        }

        return object_id;
    }

    std::ofstream file(
        object_path,
        std::ios::binary
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to create object: " +
            object_path.string()
        );
    }

    file.write(
        data.data(),
        static_cast<std::streamsize>(data.size())
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to write object: " +
            object_path.string()
        );
    }

    return object_id;
}

bool ObjectDatabase::exists(
    const std::string& object_id
) const
{
    if (!is_valid_object_id(object_id)) {
        return false;
    }

    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    return std::filesystem::is_regular_file(
        object_path
    );
}

std::string ObjectDatabase::read(
    const std::string& object_id
) const
{
    if (!is_valid_object_id(object_id)) {
        throw std::invalid_argument(
            "Invalid object ID: " +
            object_id
        );
    }

    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    if (!std::filesystem::is_regular_file(object_path)) {
        throw std::runtime_error(
            "Object not found: " +
            object_id
        );
    }

    std::ifstream file(
        object_path,
        std::ios::binary
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to open object: " +
            object_id
        );
    }

    const std::string data{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    if (file.bad()) {
        throw std::runtime_error(
            "Failed to read object: " +
            object_id
        );
    }

    return data;
}

std::vector<std::string>
ObjectDatabase::object_ids() const
{
    std::vector<std::string> result;

    if (!std::filesystem::exists(objects_dir_)) {
        return result;
    }

    if (!std::filesystem::is_directory(objects_dir_)) {
        throw std::runtime_error(
            "Object storage path is not a directory: " +
            objects_dir_.string()
        );
    }

    for (
        const auto& entry :
        std::filesystem::directory_iterator(
            objects_dir_
        )
    ) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string object_id =
            entry.path().filename().string();

        if (!is_valid_object_id(object_id)) {
            continue;
        }

        result.push_back(object_id);
    }

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}