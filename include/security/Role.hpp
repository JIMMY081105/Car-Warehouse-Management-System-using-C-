// Defines the user roles used by the Role-Based Access Control (RBAC) system.
// Each role maps to a specific set of permissions that govern what actions a
// user can perform inside the car warehouse blockchain system.

#pragma once

#include <string>

namespace cw1 {

// The five system roles mirror real warehouse organisational positions.
// Permission checks throughout the GUI and backend reference these values
// so access decisions are centralised rather than scattered.
enum class Role {
    ADMIN,             // Full system access including approvals and tamper simulation
    WAREHOUSE_STAFF,   // Can create Production and Warehouse Intake blocks
    QC_INSPECTOR,      // Can create Quality Check blocks
    DEALER,            // Can create Dealer Dispatch and Customer Sale blocks
    AUDITOR            // Read-only access to blockchain, audit log, and integrity
};

// Converts a Role enum value into a human-readable label for the GUI.
std::string roleToString(Role role);

// Parses a stored or displayed role name back into the corresponding enum value.
Role stringToRole(const std::string& text);

}  // namespace cw1
