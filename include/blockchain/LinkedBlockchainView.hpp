// Defines a lightweight linked-list mirror of the blockchain. The main coursework chain remains vector-backed for indexing, search, validation, and database persistence, while this helper exposes explicit Node* next traversal so a marker can see pointer-based chaining clearly.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"

namespace cw1 {

// Each node owns only the link structure. The Block object itself remains owned by Blockchain::chain_, so the linked view must be rebuilt whenever the vector storage is structurally modified and block addresses may change.
struct LinkedBlockNode {
    const Block* block = nullptr;
    LinkedBlockNode* next = nullptr;

    explicit LinkedBlockNode(const Block& source) : block(&source) {}
};

// Companion linked-list representation of the blockchain used for coursework visibility and pointer traversal. It intentionally mirrors the authoritative vector rather than replacing it.
class LinkedBlockchainView {
public:
    LinkedBlockchainView() = default;
    ~LinkedBlockchainView();

    LinkedBlockchainView(const LinkedBlockchainView&) = delete;
    LinkedBlockchainView& operator=(const LinkedBlockchainView&) = delete;

    // Rebuilds the mirror by clearing all existing nodes and appending one node per block from the authoritative vector.
    void rebuild(const std::vector<Block>& blocks);

    // Deletes every dynamically allocated node and resets head_/tail_ so no stale raw pointers remain after cleanup.
    void clear();

    // Returns the head pointer used for traversal. Clients walk from head() through next until the chain terminates at nullptr.
    const LinkedBlockNode* head() const noexcept;

    // Stored node count maintained during rebuild/clear operations.
    std::size_t size() const noexcept;
    bool empty() const noexcept;

    // Counts nodes by explicit pointer traversal from head_ to nullptr.
    std::size_t nodeCountByTraversal() const noexcept;

    // Returns a short marker-friendly summary such as "head -> #0 -> #1 -> ... -> nullptr".
    std::string describeTraversal(std::size_t previewNodes = 6) const;

private:
    // Allocates one node with new, links it to the tail, and updates the head/tail pointers for singly linked-list insertion at the end.
    LinkedBlockNode* appendNode(const Block& block);

    LinkedBlockNode* head_ = nullptr;
    LinkedBlockNode* tail_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace cw1
