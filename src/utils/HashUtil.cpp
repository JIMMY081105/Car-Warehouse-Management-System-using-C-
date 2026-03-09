#include "utils/HashUtil.hpp"

#include <iomanip>
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

} 
