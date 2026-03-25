#include "blockchain/Block.hpp"

#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>
#include <utility>

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
    sha3Hash_    = computeSha3Hash();

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
    out << "Block(index=" << index_
        << ", hash=" << currentHash_.substr(0, 12) << "..."
        << ", prev=" << (previousHash_.size() > 12 ? previousHash_.substr(0, 12) + "..." : previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

void Block::debugOverrideCurrentHash(std::string forgedHash) {
    currentHash_ = std::move(forgedHash);
}

void Block::debugTamperPayloadForSimulation(const std::string& marker) {
    
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

std::uint64_t Block::generateNonce() {
    // Seed std::rand once using current time (spec requirement: std::rand for hashes).
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // Combine two std::rand() calls to build a 64-bit nonce.
    // std::rand() returns at least 15 bits, so four calls cover 60+ bits.
    auto r = []() -> std::uint64_t { return static_cast<std::uint64_t>(std::rand()); };
    std::uint64_t nonce = (r() << 45) ^ (r() << 30) ^ (r() << 15) ^ r();

    return nonce;
}

} 
