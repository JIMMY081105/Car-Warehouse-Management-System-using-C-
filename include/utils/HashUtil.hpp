// Hash helpers shared across the project.
#pragma once

#include <string>

namespace cw1::HashUtil {

// This is the main hash that links blocks together and supports integrity checks.
std::string sha256(const std::string& input);

// This shorter digest gives the UI and validation code a second hash to compare.
std::string sha3_128(const std::string& input);

}
