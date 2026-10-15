#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PeerInfo {
	uint32_t PeerID = 0;
	std::string HostName;
	uint16_t Port = 0;
	bool HasFile = false;
};

struct Config {
	uint32_t NumberPreferred = 0;              // k
	uint32_t UnchokingInterval = 0;            // p (seconds)
	uint32_t OptimisticUnchokingInterval = 0;  // m (seconds)
	std::string FileName;
	uint64_t FileSize = 0;
	uint32_t PieceSize = 0;
	uint32_t NumberPieces = 0;                 // computed: ceil(FileSize / PieceSize)
	std::vector<PeerInfo> Peers;               // in PeerInfo.cfg order

	// Reads Common.cfg and PeerInfo.cfg from Dir.
	static Config load(const std::string& Dir);
	// nullptr if no peer has this ID.
	const PeerInfo* findPeer(uint32_t PeerID) const;
};
