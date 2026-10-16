#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include "BitField.h"

enum class MessageType : uint8_t {
    Choke = 0,
    Unchoke = 1,
    Interested = 2,
    NotInterested = 3,
    Have = 4,
    Bitfield = 5,
    Request = 6,
    Piece = 7
};

struct Message {
    MessageType type;
    std::vector<uint8_t> payload; // raw payload bytes (excludes length + type)
};

// Encoding: each returns the full frame [4-byte length][1-byte type][payload],
// where length counts the type byte + payload.
std::vector<uint8_t> encodeMessage(MessageType type, const std::vector<uint8_t>& payload = {});
std::vector<uint8_t> encodeChoke();
std::vector<uint8_t> encodeUnchoke();
std::vector<uint8_t> encodeInterested();
std::vector<uint8_t> encodeNotInterested();
std::vector<uint8_t> encodeHave(uint32_t pieceIndex);
std::vector<uint8_t> encodeBitfield(const BitField& bitfield);
std::vector<uint8_t> encodeRequest(uint32_t pieceIndex);
std::vector<uint8_t> encodePiece(uint32_t pieceIndex, const std::vector<uint8_t>& data);

// Decoding
uint32_t decodeLength(const uint8_t* header); // reads the 4-byte length prefix
Message decodeMessage(const std::vector<uint8_t>& frame); // full frame including length prefix
uint32_t getPieceIndex(const Message& msg); // Have / Request / Piece
std::vector<uint8_t> getPieceData(const Message& msg); // Piece
BitField getBitfield(const Message& msg, std::size_t pieceCount); // Bitfield
