#include <cstdint>
#include <string>
#include <vector>

struct PeerInfo {
	uint32_t PeerID;
	std::string HostName;
	uint32_t Port;
	bool File;
};

struct Config {
	uint32_t numberPreferred;
	uint32_t UnchokingInterval;
	uint32_t OptimisticUnchokingInterval; 
	std::string FileName;
	uint64_t FileSize;
	uint32_t PieceSize;
	uint32_t NumberPieces;
	std::vector<PeerInfo> Peers;
	
	static Config load(std::string Path);
	const PeerInfo findPeer(uint32_t PeerID);
};
