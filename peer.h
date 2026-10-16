#pragma once

#include <cstdint>
#include <array>

class client {
	static constexpr char ident[] = "P2PFILESHARINGPROJ";

public:
	static std::array<uint8_t, 32> handshakeMsg(uint32_t PeerID);

	// Returns the sender's peer ID; throws std::runtime_error if the handshake is malformed.
	static uint32_t parseHandshake(const std::array<uint8_t, 32>& message);
};
