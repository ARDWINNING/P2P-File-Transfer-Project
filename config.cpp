#include "config.h"
#include <fstream>
#include <stdexcept>

Config Config::load(const std::string& Dir) {
	Config cfg;

	std::ifstream cfgIn(Dir + "/Common.cfg");
	if (!cfgIn){
		throw std::runtime_error("cannot open " + Dir + "/Common.cfg");
	}

	std::string key;
	while (cfgIn >> key) {
		if (key == "NumberOfPreferredNeighbors") {
			cfgIn >> cfg.NumberPreferred;
		} else if (key == "UnchokingInterval") {
			cfgIn >> cfg.UnchokingInterval;
		} else if (key == "OptimisticUnchokingInterval") {
			cfgIn >> cfg.OptimisticUnchokingInterval;
		} else if (key == "FileName") {
			cfgIn >> cfg.FileName;
		} else if (key == "FileSize") {
			cfgIn >> cfg.FileSize;
		} else if (key == "PieceSize") {
			cfgIn >> cfg.PieceSize;
		} else {
			throw std::runtime_error("unknown key in Common.cfg: " + key);
		}
	}
	if (cfg.PieceSize == 0) { 
		throw std::runtime_error("PieceSize missing or 0");
	}
	cfg.NumberPieces = static_cast<uint32_t>((cfg.FileSize + cfg.PieceSize - 1) / cfg.PieceSize);

	std::ifstream peerIn(Dir + "/PeerInfo.cfg");
	if (!peerIn) { 
		throw std::runtime_error("cannot open " + Dir + "/PeerInfo.cfg");
	}
	PeerInfo p;
	while (peerIn >> p.PeerID >> p.HostName >> p.Port >> p.HasFile) {
		cfg.Peers.push_back(p);
	}
	return cfg;
}

const PeerInfo* Config::findPeer(uint32_t PeerID) const {
	for (const PeerInfo& p : Peers) {
		if (p.PeerID == PeerID) {
			return &p;
		}
	}
	return nullptr;
}

