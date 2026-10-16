#include "peer.h"

#include <cstring>
#include <cstddef>
#include <stdexcept>
#include <arpa/inet.h>

std::array<uint8_t, 32> client::handshakeMsg(uint32_t PeerID){
	std::array<uint8_t, 32> message{};
	std::memcpy(message.data(), ident, sizeof(ident)-1);
	uint32_t nPeerID = htonl(PeerID);
	std::memcpy(message.data() + 28, &nPeerID, 4);
	return message;
}

uint32_t client::parseHandshake(const std::array<uint8_t, 32>& message){
	if (std::memcmp(message.data(), ident, sizeof(ident)-1) != 0) {
		throw std::runtime_error("Invalid handshake header");
	}
	for (size_t i = sizeof(ident)-1; i < 28; i++) {
		if (message[i] != 0) {
			throw std::runtime_error("Invalid handshake padding");
		}
	}
	uint32_t nPeerID;
	std::memcpy(&nPeerID, message.data() + 28, 4);
	return ntohl(nPeerID);
}
