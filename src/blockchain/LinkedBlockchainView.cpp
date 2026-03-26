// Implements the pointer-based mirror used to demonstrate linked-list traversal alongside the production vector-backed blockchain storage.

#include "blockchain/LinkedBlockchainView.hpp"

#include <sstream>

namespace cw1 {

LinkedBlockchainView::~LinkedBlockchainView() {
    clear();
}

void LinkedBlockchainView::rebuild(const std::vector<Block>& blocks) {
    clear();

    // Node insertion is explicit so the coursework shows how new nodes are created and stitched together via the next pointer.
    for (const Block& block : blocks) {
        appendNode(block);
    }
}

void LinkedBlockchainView::clear() {
    // Cleanup traverses from the head pointer until nullptr so every node allocated with new is released exactly once with delete.
    LinkedBlockNode* current = head_;
    while (current != nullptr) {
        LinkedBlockNode* next = current->next;
        delete current;
        current = next;
    }

    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

const LinkedBlockNode* LinkedBlockchainView::head() const noexcept {
    return head_;
}

std::size_t LinkedBlockchainView::size() const noexcept {
    return size_;
}

bool LinkedBlockchainView::empty() const noexcept {
    return size_ == 0;
}

std::size_t LinkedBlockchainView::nodeCountByTraversal() const noexcept {
    std::size_t count = 0;
    const LinkedBlockNode* current = head_;
    while (current != nullptr) {
        ++count;
        current = current->next;
    }
    return count;
}

std::string LinkedBlockchainView::describeTraversal(std::size_t previewNodes) const {
    std::ostringstream out;
    out << "head";

    const LinkedBlockNode* current = head_;
    std::size_t shown = 0;
    while (current != nullptr && shown < previewNodes) {
        out << " -> #" << current->block->getIndex();
        current = current->next;
        ++shown;
    }

    if (current != nullptr) {
        out << " -> ...";
    }

    out << " -> nullptr";
    return out.str();
}

LinkedBlockNode* LinkedBlockchainView::appendNode(const Block& block) {
    LinkedBlockNode* node = new LinkedBlockNode(block);

    if (tail_ == nullptr) {
        head_ = node;
        tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }

    ++size_;
    return node;
}

}  // namespace cw1
