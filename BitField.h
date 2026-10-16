#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include <filesystem>

class BitField {
private:
    std::vector<uint8_t> bits;
    std::size_t pieceCount;

public:
    BitField(std::size_t pieceCount, bool hasFile = false);

    size_t getPieceCount() const;
    bool hasPiece(std::size_t index) const;
    void setPiece(std::size_t index);
    bool isComplete() const;

    std::vector<std::size_t> missingFrom(const BitField& other) const;
    std::vector<uint8_t> toBytes() const;
    static BitField fromBytes(const std::vector<uint8_t>& data, std::size_t pieceCount);
};
