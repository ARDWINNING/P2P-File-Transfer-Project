
#include "logger.h"
#include <format>
#include <mutex>
#include <queue>
#include <stdexcept>

Logger::Logger(uint32_t SelfID, const std::string& RootDir) : SID(SelfID), 
	Out(RootDir + "/log_peer_" + std::to_string(SID) + ".log") {
	if (!Out) {
		throw std::runtime_error("Could not open log_peer_" + std::to_string(SID) + ".log");
	}
	Worker = std::thread(&Logger::run, this);
	//Something with Worker
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
	std::string prefix = "[" + std::format("{:%Y-%m-%d %H:%M:%S}", E.Time) + "]: ";

        auto it = LogFormats.find(E.Type);
        if (it == LogFormats.end()) {
                return prefix + " Invalid log type " + E.Type;
        }

        try {
                if (E.Type == "PreferredNeighbors") {
                        std::string joined;
                        for (size_t i = 0; i < E.Arguments.size(); ++i) {
                                if (i > 0) {
					joined += ',';
				} else {
                                	joined += std::to_string(E.Arguments[i]);
				}
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

