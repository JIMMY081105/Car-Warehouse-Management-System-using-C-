#pragma once

#include <string>

namespace cw1::HashUtil {

/// Compute the SHA-256 hash of the input string and return it as a
/// 64-character lowercase hex string.
std::string sha256(const std::string& input);

} // namespace cw1::HashUtil
