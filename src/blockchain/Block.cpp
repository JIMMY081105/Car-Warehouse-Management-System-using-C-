#include "blockchain/Block.hpp"

#include <random>
#include <sstream>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

using namespace std;

namespace cw1 {

Block::Block(size_t index, string previousHash, CarRecord record)
    : index_(index),
      previousHash_(move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(move(record)) {
    currentHash_ = computeHash();

    // Coursework spec: the genesis block's previousHash must equal its
    // currentHash. After computing the hash with "0" as previousHash,
    // we update previousHash_ to match currentHash_.
    // computeHash() detects this case (index_ == 0) and always uses "0"
    // so that re-computation during integrity checks remains consistent.
    if (index_ == 0 && previousHash_ == "0") {
        previousHash_ = currentHash_;
    }
}

size_t Block::getIndex() const noexcept {
    return index_;
}

const string& Block::getCurrentHash() const noexcept {
    return currentHash_;
}

const string& Block::getPreviousHash() const noexcept {
    return previousHash_;
}

const string& Block::getTimestamp() const noexcept {
    return timestamp_;
}

uint64_t Block::getNonce() const noexcept {
    return nonce_;
}

const CarRecord& Block::getRecord() const noexcept {
    return record_;
}

string Block::computeHash() const {
    ostringstream payload;
    // For the genesis block (index_ == 0), always use "0" as the previousHash
    // in the hash computation, regardless of the stored previousHash_ value.
    // This is necessary because the constructor sets previousHash_ = currentHash_
    // after the hash is first computed, so we must reproduce the original "0"
    // input to verify the hash correctly during integrity checks.
    const string prevForHash = (index_ == 0) ? string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

string Block::toString() const {
    ostringstream out;
    out << "Block(index=" << index_
        << ", hash=" << currentHash_.substr(0, 12) << "..."
        << ", prev=" << (previousHash_.size() > 12 ? previousHash_.substr(0, 12) + "..." : previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

uint64_t Block::generateNonce() {
    static thread_local mt19937_64 engine{random_device{}()};
    static thread_local uniform_int_distribution<uint64_t> dist;
    return dist(engine);
}

} // namespace cw1
