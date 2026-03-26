// Implements small search helpers shared by blockchain queries and the GUI.

#include "utils/StringUtil.hpp"

#include <algorithm>
#include <cctype>

namespace cw1 {

bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;

    auto toLower = [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    };

    std::string h, n;
    h.resize(haystack.size());
    n.resize(needle.size());
    std::transform(haystack.begin(), haystack.end(), h.begin(), toLower);
    std::transform(needle.begin(), needle.end(), n.begin(), toLower);

    return h.find(n) != std::string::npos;
}  // namespace cw1

}
