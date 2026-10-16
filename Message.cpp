#include "Message.h"

#include <cstring>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>

namespace {

void appendUint32(std::vector<uint8_t>& buffer, uint32_t value) {
    uint32_t nValue = htonl(value);
    uint8_t bytes[4];
    std::memcpy(bytes, &nValue, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

uint32_t readUint32(const uint8_t* data) {
    uint32_t nValue;
    std::memcpy(&nValue, data, 4);
    return ntohl(nValue);
}

std::vector<uint8_t> indexPayload(uint32_t pieceIndex) {
    std::vector<uint8_t> payload;
    appendUint32(payload, pieceIndex);
    return payload;
}

}

std::vector<uint8_t> encodeMessage(MessageType type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> frame;
    frame.reserve(5 + payload.size());
    appendUint32(frame, static_cast<uint32_t>(1 + payload.size()));
    frame.push_back(static_cast<uint8_t>(type));
    frame.insert(frame.end(), payload.begin(), payload.end());
    return frame;
}

std::vector<uint8_t> encodeChoke() { return encodeMessage(MessageType::Choke); }
std::vector<uint8_t> encodeUnchoke() { return encodeMessage(MessageType::Unchoke); }
std::vector<uint8_t> encodeInterested() { return encodeMessage(MessageType::Interested); }
std::vector<uint8_t> encodeNotInterested() { return encodeMessage(MessageType::NotInterested); }

std::vector<uint8_t> encodeHave(uint32_t pieceIndex) {
    return encodeMessage(MessageType::Have, indexPayload(pieceIndex));
}

std::vector<uint8_t> encodeBitfield(const BitField& bitfield) {
    return encodeMessage(MessageType::Bitfield, bitfield.toBytes());
}

std::vector<uint8_t> encodeRequest(uint32_t pieceIndex) {
    return encodeMessage(MessageType::Request, indexPayload(pieceIndex));
}

std::vector<uint8_t> encodePiece(uint32_t pieceIndex, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> payload = indexPayload(pieceIndex);
    payload.insert(payload.end(), data.begin(), data.end());
    return encodeMessage(MessageType::Piece, payload);
}

uint32_t decodeLength(const uint8_t* header) {
    return readUint32(header);
}

Message decodeMessage(const std::vector<uint8_t>& frame) {
    if (frame.size() < 5) {
        throw std::runtime_error("Message frame too short: " + std::to_string(frame.size()) + " bytes");
    }

    uint32_t length = decodeLength(frame.data());
    if (length != frame.size() - 4) {
        throw std::runtime_error("Message length field " + std::to_string(length) +
                                 " does not match frame size " + std::to_string(frame.size()));
    }

    uint8_t rawType = frame[4];
    if (rawType > static_cast<uint8_t>(MessageType::Piece)) {
        throw std::runtime_error("Unknown message type: " + std::to_string(rawType));
    }

    Message msg{static_cast<MessageType>(rawType), std::vector<uint8_t>(frame.begin() + 5, frame.end())};
    size_t payloadSize = msg.payload.size();

    switch (msg.type) {
        case MessageType::Choke:
        case MessageType::Unchoke:
        case MessageType::Interested:
        case MessageType::NotInterested:
            if (payloadSize != 0) {
                throw std::runtime_error("Message type " + std::to_string(rawType) + " must have no payload");
            }
            break;
        case MessageType::Have:
        case MessageType::Request:
            if (payloadSize != 4) {
                throw std::runtime_error("Message type " + std::to_string(rawType) + " must have a 4-byte payload");
            }
            break;
        case MessageType::Piece:
            if (payloadSize < 4) {
                throw std::runtime_error("Piece message missing piece index");
            }
            break;
        case MessageType::Bitfield:
            break;
    }

    return msg;
}

uint32_t getPieceIndex(const Message& msg) {
    if (msg.type != MessageType::Have && msg.type != MessageType::Request && msg.type != MessageType::Piece) {
        throw std::runtime_error("Message type has no piece index");
    }
    if (msg.payload.size() < 4) {
        throw std::runtime_error("Payload too short for piece index");
    }
    return readUint32(msg.payload.data());
}

std::vector<uint8_t> getPieceData(const Message& msg) {
    if (msg.type != MessageType::Piece) {
        throw std::runtime_error("Message type has no piece data");
    }
    if (msg.payload.size() < 4) {
        throw std::runtime_error("Payload too short for piece message");
    }
    return std::vector<uint8_t>(msg.payload.begin() + 4, msg.payload.end());
}

BitField getBitfield(const Message& msg, std::size_t pieceCount) {
    if (msg.type != MessageType::Bitfield) {
        throw std::runtime_error("Message type is not bitfield");
    }
    return BitField::fromBytes(msg.payload, pieceCount);
}
