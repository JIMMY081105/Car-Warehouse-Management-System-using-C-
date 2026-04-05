// Role checks for add, approve, delete, and tamper actions.

#include "security/AccessControl.hpp"

namespace cw1 {
namespace AccessControl {

// Stage creation follows the role's part of the workflow.

bool canCreateStage(Role role, BlockStage stage) {
    switch (role) {
        case Role::ADMIN:
            // Admin has unrestricted access to all lifecycle stages.
            return true;

        case Role::WAREHOUSE_STAFF:
            // Warehouse staff handle the intake pipeline from factory to storage.
            return stage == BlockStage::PRODUCTION ||
                   stage == BlockStage::WAREHOUSE_INTAKE;

        case Role::QC_INSPECTOR:
            // Quality control inspectors only create inspection records.
            return stage == BlockStage::QUALITY_CHECK;

        case Role::DEALER:
            // Dealers manage the outbound pipeline from dispatch to final sale.
            return stage == BlockStage::DEALER_DISPATCH ||
                   stage == BlockStage::CUSTOMER_SALE;

        case Role::AUDITOR:
            // Auditors have read-only access and cannot create any blocks.
            return false;
    }
    return false;
}

// Other action-level checks.

bool canApprove(Role role) {
    // Only Admin can approve or reject pending block requests. This ensures
    // a clear separation between block creation and block commitment.
    return role == Role::ADMIN;
}

bool canDelete(Role role) {
    // Only Admin can soft-delete or restore blocks to prevent accidental
    // or unauthorised removal of blockchain history.
    return role == Role::ADMIN;
}

bool canTamper(Role role) {
    // The tamper simulation is a debug/demo feature restricted to Admin
    // so non-privileged users cannot damage the chain even accidentally.
    return role == Role::ADMIN;
}

bool canViewSensitive(Role role) {
    // Admin sees all sensitive identity fields in cleartext. Other roles
    // see masked values to demonstrate the data-protection principle.
    return role == Role::ADMIN;
}

bool canViewAuditLog(Role role) {
    // All roles can view the audit log. Auditors and Admin get the most
    // value from it, but transparency is good for all users.
    (void)role;
    return true;
}

bool canViewIntegrity(Role role) {
    // All roles can view the integrity page, but only Admin can run the
    // tamper simulation controls on that page.
    (void)role;
    return true;
}

bool isReadOnly(Role role) {
    // Auditors can view data but cannot modify the blockchain in any way.
    return role == Role::AUDITOR;
}

}  // namespace AccessControl
}  // namespace cw1
