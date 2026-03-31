// Represents an authenticated user in the car warehouse system. Each user
// carries a role assignment that the RBAC layer uses to enforce permissions,
// and a secret key used for lightweight request signing.

#pragma once

#include <string>

#include "security/Role.hpp"

namespace cw1 {

// Holds the identity and credentials for one system user. The password field
// uses a simple SHA-256 hash for coursework demonstration, not a production
// password hashing scheme.
struct User {
    std::string username;       // Login identifier (e.g. "admin01")
    std::string passwordHash;   // SHA-256 of the plaintext password
    Role role;                  // Determines RBAC permissions
    std::string displayName;    // Friendly name shown in the GUI header
    std::string secretKey;      // Per-user secret used for request signatures
};

}  // namespace cw1
