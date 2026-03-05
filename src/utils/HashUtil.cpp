#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

using namespace std;

namespace cw1::HashUtil {

string sha256(const string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        digest
    );

    ostringstream output;
    output << hex << setfill('0');

    for (unsigned char byte : digest) {
        output << setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

} // namespace cw1::HashUtil
