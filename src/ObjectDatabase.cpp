#include "ObjectDatabase.hpp"

#include "Hash.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

ObjectDatabase::ObjectDatabase(
    const std::filesystem::path& git_dir
)
    : objects_dir_(git_dir / "objects") {

    std::filesystem::create_directories(objects_dir_);
}

std::string ObjectDatabase::store(
    const Object& object
) {
    const std::string data =
        object.serialize();

    const std::string object_id =
        Hash::sha256(data);

    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    if (std::filesystem::exists(object_path)) {
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
) const {
    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    return std::filesystem::exists(object_path);
}

std::string ObjectDatabase::read(
    const std::string& object_id
) const {
    const std::filesystem::path object_path =
        objects_dir_ / object_id;

    if (!std::filesystem::exists(object_path)) {
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

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}
