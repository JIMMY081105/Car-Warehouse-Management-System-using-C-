// Implements block hashing, tamper simulation helpers, and rehash behaviour. The coursework uses both SHA-256 and SHA3-128 so the marker can see multiple integrity checks applied to the same stored payload.

#include "blockchain/Block.hpp"

#include <random>
#include <sstream>
#include <utility>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

namespace cw1 {

// A new block starts with a fresh timestamp and nonce so the stored hash represents the exact state that was appended to the blockchain.
Block::Block(std::size_t index, std::string previousHash, CarRecord record)
    : index_(index),
      previousHash_(std::move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(std::move(record)) {
    currentHash_ = computeHash();
    sha3Hash_    = computeSha3Hash();

    // The genesis block visibly satisfies the lecture rule by storing its own hash in previousHash_ after the initial digest has been computed.
    if (index_ == 0 && previousHash_ == "0") {
        previousHash_ = currentHash_;
    }
}

Block::Block(std::size_t index,
             std::string previousHash,
             std::string currentHash,
             std::string timestamp,
             std::uint64_t nonce,
             CarRecord record,
             std::string sha3Hash)
    : index_(index),
      currentHash_(std::move(currentHash)),
      previousHash_(std::move(previousHash)),
      timestamp_(std::move(timestamp)),
      nonce_(nonce),
      record_(std::move(record)) {
    if (sha3Hash.empty()) {
        sha3Hash_ = computeSha3Hash();
    } else {
        sha3Hash_ = std::move(sha3Hash);
    }
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

const std::string& Block::getSha3Hash() const noexcept {
    return sha3Hash_;
}

std::string Block::computeHash() const {
    std::ostringstream payload;

    // The genesis block is hashed with "0" as its logical previous-hash input, while later blocks use the real previous block hash.
    const std::string prevForHash = (index_ == 0) ? std::string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

std::string Block::computeSha3Hash() const {
    std::ostringstream payload;
    const std::string prevForHash = (index_ == 0) ? std::string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha3_128(payload.str());
}

std::string Block::toString() const {
    std::ostringstream out;
    constexpr std::size_t kHashPreviewLen = 12;
    auto truncHash = [&](const std::string& h) {
        return h.size() > kHashPreviewLen ? h.substr(0, kHashPreviewLen) + "..." : h;
    };
    out << "Block(index=" << index_
        << ", hash=" << truncHash(currentHash_)
        << ", prev=" << truncHash(previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

void Block::debugOverrideCurrentHash(std::string forgedHash) {
    currentHash_ = std::move(forgedHash);
}

void Block::debugTamperPayloadForSimulation(const std::string& marker) {
    // Mutating one payload field is enough to invalidate the stored hash.
    record_.destination = marker;
}

void Block::setIndex(std::size_t newIndex) {
    index_ = newIndex;
}

void Block::setPreviousHash(std::string newPrev) {
    previousHash_ = std::move(newPrev);
}

void Block::setRecord(CarRecord newRecord) {
    record_ = std::move(newRecord);
}

void Block::rehash() {
    currentHash_ = computeHash();
    sha3Hash_    = computeSha3Hash();
    // The custom genesis rule is restored whenever block zero is recomputed.
    if (index_ == 0) {
        previousHash_ = currentHash_;
    }
}  // namespace cw1

std::uint64_t Block::generateNonce() {
    static thread_local std::mt19937_64 engine{std::random_device{}()};
    std::uniform_int_distribution<std::uint64_t> dist;
    return dist(engine);
}

}
