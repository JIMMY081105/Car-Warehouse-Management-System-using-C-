// Helpers for request signatures and masked display values.

#pragma once

#include <string>

#include "security/Role.hpp"

namespace cw1 {
namespace SecurityUtil {

// Builds a SHA-256 signature from the request metadata and the creator's
// secret key. The signature proves that the request was created by the
// claimed user and has not been altered since submission.
std::string signRequest(const std::string& username,
                        const std::string& timestamp,
                        const std::string& stage,
                        const std::string& serializedRecord,
                        const std::string& secretKey);

// Recomputes the expected signature and compares it against the stored value.
// Returns true if the signature matches, confirming request authenticity.
bool verifySignature(const std::string& signature,
                     const std::string& username,
                     const std::string& timestamp,
                     const std::string& stage,
                     const std::string& serializedRecord,
                     const std::string& secretKey);

// Replaces the middle portion of a sensitive value with asterisks so only
// the prefix and suffix are visible (e.g. "BUYER-1234" -> "BUY****34").
std::string maskSensitive(const std::string& value);

// Returns either the full value or a masked version depending on the user's
// role and the sensitivity of the named field. Admin sees everything;
// Auditor sees partially masked values; other roles see fully masked values
// for identity fields like buyerId, retailerId, and supplierId.
std::string visibleValueForRole(Role role,
                                const std::string& fieldName,
                                const std::string& actualValue);

}  // namespace SecurityUtil
}  // namespace cw1
