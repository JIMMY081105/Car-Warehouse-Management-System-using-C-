// Manages user authentication for the car warehouse blockchain system.
// Demo users are initialised at startup with hardcoded credentials so the
// coursework can demonstrate login, logout, and role-based access without
// requiring an external identity provider.

#pragma once

#include <string>
#include <vector>

#include "security/User.hpp"

namespace cw1 {

// Owns the user registry and tracks the current authenticated session.
// The GUI checks isLoggedIn() before allowing access to any system page,
// and currentUser() drives RBAC decisions throughout the application.
class AuthManager {
public:
    // Populates the demo user list so the system is ready for login immediately.
    AuthManager();

    // Attempts to authenticate using the supplied credentials. Returns true
    // if a matching username and password hash pair is found in the registry.
    bool login(const std::string& username, const std::string& password);

    // Ends the current session so the GUI returns to the login screen.
    void logout();

    // Returns true when a user has been authenticated and has not yet logged out.
    bool isLoggedIn() const noexcept;

    // Returns the authenticated user. Only valid when isLoggedIn() is true.
    const User& currentUser() const;

    // Exposes the full user list so the pending approval manager can look up
    // secret keys for signature verification.
    const std::vector<User>& getAllUsers() const noexcept;

    // Finds a user by username, or returns nullptr if not found.
    const User* findUser(const std::string& username) const;

private:
    // Creates the five demo accounts used for coursework demonstration.
    void initDemoUsers();

    std::vector<User> users_;
    const User* currentUser_ = nullptr;
    bool loggedIn_ = false;
};

}  // namespace cw1
