#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>

#include "Contract.h"
#include "object_hub.hpp"

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

enum class BarLifecycle {
	HistoricalClosed, // received from historicalData()
	LiveUpdating, // current bar, updated by historicalDataUpdate()
	LiveClosed // previous live bar after rollover to a new timestamp
};

struct OhlcBar {
	std::string timestamp; // IB bar.time
	double open = 0.0;
	double high = 0.0;
	double low = 0.0;
	double close = 0.0;
	double wap = 0.0;
	long long volume = 0;
	int count = 0;

	BarLifecycle lifecycle = BarLifecycle::HistoricalClosed;
	std::chrono::system_clock::time_point updated_at = std::chrono::system_clock::now();

	bool operator<(const OhlcBar& other) const { return timestamp < other.timestamp; }
	bool operator==(const OhlcBar& other) const { return timestamp == other.timestamp; }
};

struct HistoricalSeriesConfig {
	int req_id = -1;
	std::string contract_symbol;
	std::string end_datetime; // empty => now
	std::string duration;
	std::string bar_size;
	std::string what_to_show;
	bool keep_up_to_date = false;
	std::chrono::system_clock::time_point requested_at = std::chrono::system_clock::now();

	HistoricalSeriesConfig() = default;
	HistoricalSeriesConfig(int req_id_in, std::string contract_symbol_in, std::string end_datetime_in,
	                       std::string duration_in, std::string bar_size_in, std::string what_to_show_in,
	                       bool keep_up_to_date_in)
		: req_id(req_id_in), contract_symbol(std::move(contract_symbol_in)), end_datetime(std::move(end_datetime_in)),
		  duration(std::move(duration_in)), bar_size(std::move(bar_size_in)), what_to_show(std::move(what_to_show_in)),
		  keep_up_to_date(keep_up_to_date_in) {}
};

struct HistoricalSeriesSnapshot {
	HistoricalSeriesConfig config;
	std::vector<OhlcBar> bars;
	std::optional<OhlcBar> active_bar;
	std::string last_timestamp;
	bool historical_complete = false;
	std::chrono::system_clock::time_point last_update;
};

struct HistoricalSeries {
	HistoricalSeriesConfig config;
	// finalized bars only
	std::vector<OhlcBar> bars;
	// currently forming bar (from historicalDataUpdate)
	std::optional<OhlcBar> active_bar;
	// last observed timestamp, used to detect rollover
	std::string last_timestamp;
	bool historical_complete = false;
	std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
	// useful if wrapper callback thread and consumer thread differ
	mutable std::shared_mutex mtx;

	explicit HistoricalSeries(const HistoricalSeriesConfig& cfg) : config(cfg) {}

	HistoricalSeriesSnapshot snapshot_unlocked() const {
		return HistoricalSeriesSnapshot{config, bars, active_bar, last_timestamp, historical_complete, last_update};
	}
};


// Callback types
using BarClosedCallback = std::function<void(int req_id, const OhlcBar& bar)>;
using BarUpdateCallback = std::function<void(int req_id, const OhlcBar& bar)>;
using BarUpdateCompleteCallback = std::function<void(int req_id, const OhlcBar& bar)>;
using BarSeriesCompleteCallback = std::function<void(int req_id)>;

class HistoricalDataManager {
public:
	explicit HistoricalDataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger);

	void requestHistoricalBars(const Contract& contract, const std::string& end_datetime, const std::string& duration,
	                           const std::string& bar_size, const std::string& what_to_show);

	int subscribeHistoricalBars(const Contract& contract, const std::string& end_datetime, const std::string& duration,
	                             const std::string& bar_size, const std::string& what_to_show);

	[[nodiscard]] int getNextId() const;
	[[nodiscard]] int getCurrentId() const;

	void onBarClosed(const BarClosedCallback& callback) const { obj_.create<onBarClosedTag, BarClosedCallback>(callback); }
	void onBarUpdate(const BarUpdateCallback& callback) const { obj_.create<onBarUpdateTag, BarUpdateCallback>(callback); }

	void onBarSeriesComplete(const BarSeriesCompleteCallback& callback) const {
		obj_.create<onBarSeriesCompleteTag, BarSeriesCompleteCallback>(callback);
	}

	std::optional<HistoricalSeriesSnapshot> snapshot(const Contract& contract);
	std::optional<HistoricalSeriesSnapshot> snapshot(int req_id) const;

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::unordered_map<int, HistoricalSeries>& historical_series_; // key is reqId
	std::unordered_map<int, int> conId_to_req_id_;
};
