#pragma once

#include <mutex>
#include <ctime>
#include <queue>
#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <condition_variable>

inline const std::unordered_map<std::string_view, std::string_view> LogFormats = {
	{"MadeConnection",        "Peer {0} makes a connection to Peer {1}."},
	{"ConnectedFrom",         "Peer {0} is connected from Peer {1}."},
	{"PreferredNeighbors",    "Peer {0} has the preferred neighbors {1}."},
	{"OptimisticUnchoke",     "Peer {0} has the optimistically unchoked neighbor {1}."},
	{"Unchoked",              "Peer {0} is unchoked by {1}."},
	{"Choked",                "Peer {0} is choked by {1}."},
	{"ReceivedHave",          "Peer {0} received the 'have' message from {1} for the piece {2}."},
	{"ReceivedInterested",    "Peer {0} received the 'interested' message from {1}."},
	{"ReceivedNotInterested", "Peer {0} received the 'not interested' message from {1}."},
	{"DownloadedPiece",       "Peer {0} has downloaded the piece {2} from {1}. Now the number of pieces it has is {3}."},
	{"DownloadComplete",      "Peer {0} has downloaded the complete file."},
};

struct Event {
	std::string Type;                  // key into LogFormats
	std::vector<uint32_t> Arguments;   // {peer}, {peer, piece}, {peer, piece, count}, or the neighbor list
	std::chrono::system_clock::time_point Time = std::chrono::system_clock::now();
};

class Logger {
	Logger(uint32_t SelfID, const std::string& RootDir);

	~Logger();

	void log(std::string Type, std::vector<uint32_t> Arguments = {});

private:
	std::string FormatEvent(const Event& E) const;
	void run();

	uint32_t SID;
	std::ofstream Out;
	std::queue<Event> Queue;
	std::mutex Mutex;
	std::condition_variable Ready;
	bool Stopping = false;
	std::thread Worker;
};
