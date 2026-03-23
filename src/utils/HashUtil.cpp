#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>

namespace cw1::HashUtil {

std::string sha256(const std::string& input) {
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
    // Use SHAKE128 (SHA-3 family XOF) with 128-bit / 16-byte output.
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_shake128(), nullptr);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    unsigned char digest[16]; // 128-bit output
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
