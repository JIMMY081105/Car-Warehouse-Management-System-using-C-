// Implements user authentication with hardcoded demo accounts. Passwords are
// stored as SHA-256 hashes so the coursework demonstrates a realistic security
// pattern without requiring an external authentication framework.

#include "security/Auth.hpp"

#include <algorithm>
#include <cctype>

#include "utils/HashUtil.hpp"

namespace cw1 {

// ---------------------------------------------------------------------------
// Role string conversion helpers
// ---------------------------------------------------------------------------

std::string roleToString(Role role) {
    switch (role) {
        case Role::ADMIN:           return "Admin";
        case Role::WAREHOUSE_STAFF: return "Warehouse Staff";
        case Role::QC_INSPECTOR:    return "QC Inspector";
        case Role::DEALER:          return "Dealer";
        case Role::AUDITOR:         return "Auditor";
    }
    return "Unknown";
}

Role stringToRole(const std::string& text) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "admin")           return Role::ADMIN;
    if (lower == "warehouse staff") return Role::WAREHOUSE_STAFF;
    if (lower == "qc inspector")    return Role::QC_INSPECTOR;
    if (lower == "dealer")          return Role::DEALER;
    if (lower == "auditor")         return Role::AUDITOR;

    return Role::AUDITOR;  // Safe default for unknown role strings
}

// ---------------------------------------------------------------------------
// AuthManager implementation
// ---------------------------------------------------------------------------

AuthManager::AuthManager() {
    initDemoUsers();
}

void AuthManager::initDemoUsers() {
    // Five demo accounts covering every role in the RBAC matrix. Each account
    // uses a simple password that is easy to type during a coursework demo.
    // The secret key is unique per user and used for request signatures.

    users_.push_back({
        "admin01",
        HashUtil::sha256("admin123"),
        Role::ADMIN,
        "Admin User",
        "secret-key-admin-01"
    });

    users_.push_back({
        "staff01",
        HashUtil::sha256("staff123"),
        Role::WAREHOUSE_STAFF,
        "staff MA YIXING",
        "secret-key-staff-01"
    });

    users_.push_back({
        "qc01",
        HashUtil::sha256("qc123"),
        Role::QC_INSPECTOR,
        "inspector MA YIXING",
        "secret-key-qc-01"
    });

    users_.push_back({
        "dealer01",
        HashUtil::sha256("dealer123"),
        Role::DEALER,
        "MA YIXING",
        "secret-key-dealer-01"
    });

    users_.push_back({
        "auditor01",
        HashUtil::sha256("audit123"),
        Role::AUDITOR,
        "MA YIXING",
        "secret-key-auditor-01"
    });
}

bool AuthManager::login(const std::string& username, const std::string& password) {
    // Hash the supplied plaintext password and compare it against the stored
    // digest so credentials are never compared in cleartext.
    const std::string hash = HashUtil::sha256(password);

    for (const User& user : users_) {
        if (user.username == username && user.passwordHash == hash) {
            currentUser_ = &user;
            loggedIn_ = true;
            return true;
        }
    }

    // Authentication failed. The caller logs the failure into the audit trail.
    currentUser_ = nullptr;
    loggedIn_ = false;
    return false;
}

void AuthManager::logout() {
    currentUser_ = nullptr;
    loggedIn_ = false;
}

bool AuthManager::isLoggedIn() const noexcept {
    return loggedIn_ && currentUser_ != nullptr;
}

const User& AuthManager::currentUser() const {
    // Precondition: isLoggedIn() must be true. The GUI always checks this
    // before rendering any authenticated page.
    return *currentUser_;
}

const std::vector<User>& AuthManager::getAllUsers() const noexcept {
    return users_;
}

const User* AuthManager::findUser(const std::string& username) const {
    for (const User& user : users_) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

}  // namespace cw1
