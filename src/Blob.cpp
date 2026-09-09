#include "Blob.hpp"

#include "FileReader.hpp"

#include <sstream>
#include <utility>

Blob::Blob(std::string content)
    : content_(std::move(content)) {
}

Blob Blob::from_file(const std::filesystem::path& path) {
    return Blob(FileReader::read(path));
}

std::string Blob::serialize() const {
    std::ostringstream output;

    output << "blob "
           << content_.size()
           << '\0'
           << content_;

    return output.str();
}