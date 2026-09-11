#include "Blob.hpp"

#include "FileReader.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

Blob::Blob(
    std::string content
)
    : content_(std::move(content)) {
}

Blob Blob::from_file(
    const std::filesystem::path& path
) {
    return Blob(
        FileReader::read(path)
    );
}

Blob Blob::deserialize(
    const std::string& data
) {
    const std::size_t separator =
        data.find('\0');

    if (separator == std::string::npos) {
        throw std::runtime_error(
            "Invalid blob object"
        );
    }

    const std::string header =
        data.substr(
            0,
            separator
        );

    if (header.rfind("blob ", 0) != 0) {
        throw std::runtime_error(
            "Invalid blob header"
        );
    }

    const std::string size_text =
        header.substr(5);

    std::size_t expected_size = 0;

    try {
        expected_size =
            std::stoull(size_text);
    }
    catch (...) {
        throw std::runtime_error(
            "Invalid blob size"
        );
    }

    const std::string content =
        data.substr(
            separator + 1
        );

    if (content.size() != expected_size) {
        throw std::runtime_error(
            "Blob size mismatch"
        );
    }

    return Blob(content);
}

const std::string& Blob::content() const {
    return content_;
}

std::string Blob::serialize() const {
    std::ostringstream output;

    output << "blob "
           << content_.size()
           << '\0'
           << content_;

    return output.str();
}