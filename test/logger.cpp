#include "quantib/sync/ib.hpp"

int main() {
	// Initialize IB class
	// Default level for logging is trace
	// Output should be like:
	/*
	[2026-06-08 19:24:56] [debug] [Logger] Initialized correctly with level: trace.
	[2026-06-08 19:24:56] [debug] [ObjectHub] Initialized correctly.
	[2026-06-08 19:24:56] [debug] [ObjectHub] Initialized correctly.
	[2026-06-08 19:24:56] [debug] [Wrapper] Initialized correctly.
	[2026-06-08 19:24:56] [trace] [PerfTimer] Tag=ConnectTag latency=41 ms
	*/
	const auto ib = std::make_unique<IB>();
	// Sync operations always return performances at trace level!
	ib->connect();
}
