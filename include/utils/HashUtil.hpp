// These helpers keep the blockchain hashing code in one place instead of spreading OpenSSL calls around.
#pragma once

#include <string>

namespace cw1::HashUtil {

// This is the main hash that links blocks together and supports integrity checks.
std::string sha256(const std::string& input);

// This shorter digest gives the UI and validation code a second hash to compare.
std::string sha3_128(const std::string& input);

}
