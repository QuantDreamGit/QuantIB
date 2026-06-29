#pragma once

#include <chrono>
#include <functional>
#include <shared_mutex>
#include <string>

#include "Contract.h"

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

// Bar state: tracks lifecycle of OHLC bars
enum class BarState {
	HISTORICAL,      // From historicalData() callback - complete historical bar
	LIVE_UPDATING,   // From historicalDataUpdate() - most recent bar, being updated
	CLOSED           // Live bar closed, moved to next timestamp - ready for indicator computation
};

// OHLC bar data structure (renamed from Bar to avoid conflict with IB's Bar struct)
struct HistoricalBar {
	std::string timestamp;          // ISO format or epoch time
	double open = 0.0;
	double high = 0.0;
	double low = 0.0;
	double close = 0.0;
	long volume = 0;
	int bar_count = 0;
	double wap = 0.0;               // Weighted Average Price
	BarState state = BarState::HISTORICAL;
	std::chrono::system_clock::time_point received_at = std::chrono::system_clock::now();

	// Comparison for sorting by timestamp
	bool operator<(const HistoricalBar& other) const { return timestamp < other.timestamp; }
	bool operator==(const HistoricalBar& other) const { return timestamp == other.timestamp; }
};

// Configuration for a historical bar series request
struct BarSeriesConfig {
	int req_id = -1;
	std::string contract_symbol;
	std::string end_datetime;
	std::string duration;
	std::string bar_size;
	std::string what_to_show;
	bool keep_up_to_date = false;
	std::chrono::system_clock::time_point requested_at = std::chrono::system_clock::now();
};

// Metadata tracking the state of a bar series
struct BarSeriesMetadata {
	BarSeriesConfig config;
	int total_bars_received = 0;
	bool historical_complete = false;
	std::string historical_start_time;
	std::string historical_end_time;
	std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
};

// Callback types for bar events
using BarCompleteCallback = std::function<void(int req_id, const HistoricalBar& bar)>;
using BarUpdateCallback = std::function<void(int req_id, const HistoricalBar& bar)>;
using BarSeriesCompleteCallback = std::function<void(int req_id)>;

// Main manager for historical OHLC data with live updates
class HistoricalDataManager {
public:
	explicit HistoricalDataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger);

	// Request historical bars with optional live updates
	void requestHistoricalBars(
		const Contract& contract,
		const std::string& end_datetime,
		const std::string& duration,
		const std::string& bar_size,
		const std::string& what_to_show,
		bool keep_up_to_date = true
	);

	[[nodiscard]] int getNextId() const;
	[[nodiscard]] int getCurrentId() const;


private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;
};
