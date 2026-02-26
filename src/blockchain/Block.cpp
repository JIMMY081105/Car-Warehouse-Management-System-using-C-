#include "blockchain/Block.hpp"

#include <random>
#include <sstream>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

namespace cw1 {

Block::Block(std::size_t index, std::string previousHash, CarRecord record)
    : index_(index),
      previousHash_(std::move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(std::move(record)) {
    currentHash_ = computeHash();
}

std::size_t Block::getIndex() const noexcept {
    return index_;
}

const std::string& Block::getCurrentHash() const noexcept {
    return currentHash_;
}

const std::string& Block::getPreviousHash() const noexcept {
    return previousHash_;
}

const std::string& Block::getTimestamp() const noexcept {
    return timestamp_;
}

std::uint64_t Block::getNonce() const noexcept {
    return nonce_;
}

const CarRecord& Block::getRecord() const noexcept {
    return record_;
}

std::string Block::computeHash() const {
    std::ostringstream payload;
    payload << index_ << previousHash_ << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

std::string Block::toString() const {
    std::ostringstream out;
    out << "Block(index=" << index_
        << ", currentHash=" << currentHash_
        << ", previousHash=" << previousHash_
        << ", timestamp=" << timestamp_
        << ", nonce=" << nonce_
        << ", record={" << record_.serialize() << "})";

    return out.str();
}

std::uint64_t Block::generateNonce() {
    static thread_local std::mt19937_64 engine{std::random_device{}()};
    static thread_local std::uniform_int_distribution<std::uint64_t> dist;
    return dist(engine);
}

} // namespace cw1
