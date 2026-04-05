// Hash helpers used by block creation and validation.
#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>

namespace cw1::HashUtil {

std::string sha256(const std::string& input) {
    // SHA-256 stays as the full chain hash because it is the main integrity link between blocks.
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        digest
    );

    std::ostringstream output;
    output << std::hex << std::setfill('0');

    for (unsigned char byte : digest) {
        output << std::setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

std::string sha3_128(const std::string& input) {
    // SHAKE128 gives a shorter digest that is still useful for a visible secondary check in the project.
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_shake128(), nullptr);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    unsigned char digest[16];
    EVP_DigestFinalXOF(ctx, digest, sizeof(digest));
    EVP_MD_CTX_free(ctx);

    std::ostringstream output;
    output << std::hex << std::setfill('0');

    for (unsigned char byte : digest) {
        output << std::setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

}
