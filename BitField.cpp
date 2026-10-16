#include "BitField.h"

BitField::BitField(std::size_t pieceCount, bool hasFile) {
    this->pieceCount = pieceCount; 
    this->bits.assign((pieceCount + 7) / 8, 0);

    if (hasFile) {
        for (size_t i = 0; i < pieceCount; i++) {
            setPiece(i);
        }
    }
}

size_t BitField::getPieceCount() const {
    return pieceCount;
}

bool BitField::hasPiece(std::size_t index) const {
    if (index >= pieceCount) {
        throw std::out_of_range("Invalid piece index for has piece: " + std::to_string(index));
    }

    size_t byteIndex = index / 8;
    size_t bitPosition = index % 8;

    auto mask = 0x80 >> bitPosition;
    return (bits[byteIndex] & mask) != 0;
}

void BitField::setPiece(std::size_t index) {
    if (index >= pieceCount) {
        throw std::out_of_range("Invalid piece index for set piece: " + std::to_string(index));
    }

    size_t byteIndex = index / 8;
    size_t bitPosition = index % 8;

    auto mask = 0x80 >> bitPosition;
    bits[byteIndex] |= mask;
}

bool BitField::isComplete() const {
    for (size_t i = 0; i < pieceCount; i++) {
        if (!hasPiece(i)) {
            return false;
        }
    }

    return true;
}

std::vector<std::size_t> BitField::missingFrom(const BitField& other) const {
    if (other.getPieceCount() != pieceCount) {
        throw std::runtime_error("Cannot compare bitfields with different piece counts");
    }

    std::vector<std::size_t> result;

    for (size_t i = 0; i < pieceCount; i++) {
        if (other.hasPiece(i) && !this->hasPiece(i)) {
            result.push_back(i);
        }
    }

    return result;
}

std::vector<uint8_t> BitField::toBytes() const {
    return bits;
}

BitField BitField::fromBytes(const std::vector<uint8_t>& data, std::size_t pieceCount) {
    size_t byteCount = (pieceCount + 7) / 8;
    if (data.size() != byteCount) {
        throw std::runtime_error("Byte size does not equal piece count");
    }
    size_t unusedBits = (8 - (pieceCount % 8)) % 8;

    if (unusedBits > 0) {
        auto mask = (1u << unusedBits) - 1;

        if ((data.back() & mask) != 0) {
            throw std::runtime_error("Unused bits do not equal zero");
        }
    }

    BitField bitfield(pieceCount);
    bitfield.bits = data;

    return bitfield;
}
