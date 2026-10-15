#include <cstring>
#include <cstddef>
#include <cstdint>
#include <array>
#include <arpa/inet.h>

class client {
	const char* ident = "P2PFILESHARINGPROJ";
	

	std::array<uint8_t, 32> handshakeMsg(uint32_t PeerID){
		std::array<uint8_t, 32> message{};
		std::memcpy(message.data(), ident, sizeof(ident)-1);
		uint32_t nPeerID = htonl(PeerID);
		std::memcpy(message.data() + 28, &nPeerID, 4);
		return message;
	}
	
	
}; 


