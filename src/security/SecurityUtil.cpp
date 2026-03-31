// Implements lightweight request signing and sensitive-data masking for the
// security layer. The signature uses SHA-256 with a per-user secret key,
// and masking replaces middle characters with asterisks for display purposes.

#include "security/SecurityUtil.hpp"

#include "utils/HashUtil.hpp"

namespace cw1 {
namespace SecurityUtil {

// ---------------------------------------------------------------------------
// Request signing
// ---------------------------------------------------------------------------

std::string signRequest(const std::string& username,
                        const std::string& timestamp,
                        const std::string& stage,
                        const std::string& serializedRecord,
                        const std::string& secretKey) {
    // The signature input concatenates the key identity fields with the user's
    // secret key so the resulting digest proves both authorship and integrity.
    const std::string input = username + "|" + timestamp + "|" + stage + "|" +
                              serializedRecord + "|" + secretKey;
    return HashUtil::sha256(input);
}

bool verifySignature(const std::string& signature,
                     const std::string& username,
                     const std::string& timestamp,
                     const std::string& stage,
                     const std::string& serializedRecord,
                     const std::string& secretKey) {
    // Recompute the expected signature from the same inputs and compare.
    const std::string expected = signRequest(username, timestamp, stage,
                                             serializedRecord, secretKey);
    return signature == expected;
}

// ---------------------------------------------------------------------------
// Sensitive data masking
// ---------------------------------------------------------------------------

std::string maskSensitive(const std::string& value) {
    // Short values are fully masked to prevent information leakage.
    if (value.empty()) return "";
    if (value.size() <= 3) return std::string(value.size(), '*');

    // Keep the first three and last two characters visible, replacing the
    // middle portion with asterisks so the value is recognisable but not
    // fully readable.
    const std::size_t visiblePrefix = 3;
    const std::size_t visibleSuffix = 2;

    if (value.size() <= visiblePrefix + visibleSuffix) {
        // Value too short for the prefix+suffix scheme; mask the middle.
        std::string result = value;
        for (std::size_t i = 1; i < result.size() - 1; ++i) {
            result[i] = '*';
        }
        return result;
    }

    std::string result;
    result += value.substr(0, visiblePrefix);
    result += std::string(value.size() - visiblePrefix - visibleSuffix, '*');
    result += value.substr(value.size() - visibleSuffix);
    return result;
}

std::string visibleValueForRole(Role role,
                                const std::string& fieldName,
                                const std::string& actualValue) {
    if (actualValue.empty()) return actualValue;

    // Admin sees all values in cleartext.
    if (role == Role::ADMIN) {
        return actualValue;
    }

    // Determine whether this field contains sensitive identity data. Only
    // identity-related fields are masked; general business data like factory
    // locations and QC notes remain visible to all roles.
    bool isSensitive = (fieldName == "buyerId" ||
                        fieldName == "retailerId" ||
                        fieldName == "supplierId" ||
                        fieldName == "inspectorId" ||
                        fieldName == "dealerId" ||
                        fieldName == "manufacturerId");

    if (!isSensitive) {
        return actualValue;
    }

    // Auditor gets a partially masked view so they can correlate records
    // without seeing full identity values.
    if (role == Role::AUDITOR) {
        return maskSensitive(actualValue);
    }

    // All other roles see fully masked identity fields.
    return maskSensitive(actualValue);
}

}  // namespace SecurityUtil
}  // namespace cw1
