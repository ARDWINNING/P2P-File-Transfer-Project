
#include "logger.h"
#include <ctime>
#include <format>
#include <iomanip>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>

Logger::Logger(uint32_t SelfID, const std::string& RootDir) : SID(SelfID), 
	Out(RootDir + "/log_peer_" + std::to_string(SID) + ".log") {
	if (!Out) {
		throw std::runtime_error("Could not open log_peer_" + std::to_string(SID) + ".log");
	}
	Worker = std::thread(&Logger::run, this);
}

Logger::~Logger(){
	{
		std::lock_guard<std::mutex> lock(Mutex);
		Stopping = true;
	}
	Ready.notify_one(); // Awake if in wait() to avoid deadlock
	
	if(Worker.joinable()) { // Waits for Queue to drain
		Worker.join();
	}
}

void Logger::log(std::string Type, std::vector<uint32_t> Arguments) {
	Event e;
	e.Type = std::move(Type);
	e.Arguments = std::move(Arguments);
	{
		std::lock_guard<std::mutex> lock(Mutex);
		Queue.push(std::move(e));
	}
	Ready.notify_one();
}

void Logger::run() {
	std::unique_lock<std::mutex> lock(Mutex);

	while(true) {
		Ready.wait(lock, [this] {return !Queue.empty() || Stopping; });
		
		if(Queue.empty() && Stopping) {
			break;
		}

		std::queue<Event> batch;
		std::swap(batch, Queue); // Allows producers to continue pushing whilst we do I/O
		
		lock.unlock();

		while (!batch.empty()) {
			Out << FormatEvent(batch.front()) << '\n';
			batch.pop();
		}
		Out.flush(); // Push to file
		
		lock.lock(); // Needs to be held for wait
	}
}

std::string Logger::FormatEvent(const Event &E) const {
	// Local time to the second. std::localtime's shared buffer is safe here:
	// FormatEvent is only ever called from the single worker thread.
	std::time_t t = std::chrono::system_clock::to_time_t(E.Time);
	std::ostringstream stamp;
	stamp << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
	std::string prefix = stamp.str() + ": ";

        auto it = LogFormats.find(E.Type);
        if (it == LogFormats.end()) {
                return prefix + "Invalid log type " + E.Type;
        }

        try {
                if (E.Type == "PreferredNeighbors") {
                        std::string joined;
                        for (size_t i = 0; i < E.Arguments.size(); ++i) {
                                if (i > 0) {
					joined += ',';
				}
                                joined += std::to_string(E.Arguments[i]);
                        }
                        return prefix + std::vformat(it->second, std::make_format_args(SID, joined));
                }

                uint32_t a = E.Arguments.size() > 0 ? E.Arguments[0] : 0; // Controls for the different arg requirements so not undefined
                uint32_t b = E.Arguments.size() > 1 ? E.Arguments[1] : 0;
                uint32_t c = E.Arguments.size() > 2 ? E.Arguments[2] : 0;
                return prefix + std::vformat(it->second, std::make_format_args(SID, a, b, c));
        } catch (const std::format_error&) {
                return prefix + "bad arguments for " + E.Type;
        }
}

void Logger::logMadeConnection(uint32_t PeerID) { log("MadeConnection", {PeerID}); }
void Logger::logConnectedFrom(uint32_t PeerID) { log("ConnectedFrom", {PeerID}); }
void Logger::logPreferredNeighbors(const std::vector<uint32_t>& PeerIDs) { log("PreferredNeighbors", PeerIDs); }
void Logger::logOptimisticUnchoke(uint32_t PeerID) { log("OptimisticUnchoke", {PeerID}); }
void Logger::logUnchokedBy(uint32_t PeerID) { log("Unchoked", {PeerID}); }
void Logger::logChokedBy(uint32_t PeerID) { log("Choked", {PeerID}); }
void Logger::logReceivedHave(uint32_t PeerID, uint32_t PieceIndex) { log("ReceivedHave", {PeerID, PieceIndex}); }
void Logger::logReceivedInterested(uint32_t PeerID) { log("ReceivedInterested", {PeerID}); }
void Logger::logReceivedNotInterested(uint32_t PeerID) { log("ReceivedNotInterested", {PeerID}); }
void Logger::logDownloadedPiece(uint32_t PeerID, uint32_t PieceIndex, uint32_t PieceCount) {
	log("DownloadedPiece", {PeerID, PieceIndex, PieceCount});
}
void Logger::logDownloadComplete() { log("DownloadComplete"); }

