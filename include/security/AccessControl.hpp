// Permission checks shared by the GUI and backend.

#pragma once

#include "security/Role.hpp"
#include "blockchain/BlockStage.hpp"

namespace cw1 {
namespace AccessControl {

// Returns true if the given role is allowed to create a pending block
// request for the specified lifecycle stage.
bool canCreateStage(Role role, BlockStage stage);

// Returns true if the role is authorised to approve or reject pending
// block requests (only Admin).
bool canApprove(Role role);

// Returns true if the role may soft-delete or restore blocks (only Admin).
bool canDelete(Role role);

// Returns true if the role may run the tamper simulation feature (only Admin).
bool canTamper(Role role);

// Returns true if the role may view sensitive identity fields such as
// buyer ID, retailer ID, and supplier ID without masking.
bool canViewSensitive(Role role);

// Returns true if the role may view the audit log page.
bool canViewAuditLog(Role role);

// Returns true if the role may view the blockchain integrity page.
bool canViewIntegrity(Role role);

// Returns true if the role has purely read-only access (Auditor).
bool isReadOnly(Role role);

}  // namespace AccessControl
}  // namespace cw1
