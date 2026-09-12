#include "Hash.hpp"

#include <openssl/evp.h>

#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {

struct EvpContextDeleter
{
    void operator()(
        EVP_MD_CTX* context
    ) const noexcept
    {
        EVP_MD_CTX_free(context);
    }
};

using EvpContextPtr =
    std::unique_ptr<
        EVP_MD_CTX,
        EvpContextDeleter
    >;

} // namespace

std::string Hash::sha256(
    const std::string& data
)
{
    EvpContextPtr context(
        EVP_MD_CTX_new()
    );

    if (!context) {
        throw std::runtime_error(
            "Failed to create hash context"
        );
    }

    if (
        EVP_DigestInit_ex(
            context.get(),
            EVP_sha256(),
            nullptr
        ) != 1
    ) {
        throw std::runtime_error(
            "Failed to initialize SHA-256"
        );
    }

    if (
        EVP_DigestUpdate(
            context.get(),
            data.data(),
            data.size()
        ) != 1
    ) {
        throw std::runtime_error(
            "Failed to update SHA-256"
        );
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_length = 0;

    if (
        EVP_DigestFinal_ex(
            context.get(),
            digest,
            &digest_length
        ) != 1
    ) {
        throw std::runtime_error(
            "Failed to finalize SHA-256"
        );
    }

    std::ostringstream result;

    for (unsigned int i = 0; i < digest_length; ++i) {
        result
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(digest[i]);
    }

    return result.str();
}