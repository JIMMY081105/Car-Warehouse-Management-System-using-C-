#pragma once

#include <string>

namespace cw1::HashUtil {

std::string sha256(const std::string& input);

std::string sha3_128(const std::string& input);

}
