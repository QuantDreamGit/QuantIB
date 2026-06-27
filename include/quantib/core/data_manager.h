#pragma once
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Contract.h"
#include "EWrapper.h"

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

struct GreeksData {
	double implied_volatility;
	double delta;
	double price;
	double pv_dividend;
	double gamma;
	double vega;
	double theta;
	double und_price;

	explicit GreeksData(double implied_volatility_, double delta_, double price_, double pv_dividend_, double gamma_,
	                    double vega_, double theta_, double und_price_);
};

struct MarketData {
	std::unordered_map<TickType, double> generic_map;
	std::unordered_map<TickType, double> price_map;
	std::unordered_map<TickType, Decimal> size_map;
	std::unordered_map<TickType, std::string> string_map;
	std::unordered_map<TickType, GreeksData> option_map;

	bool has_price() const;
	std::optional<double> getPrice() const;
};

struct MarketDataStore {
	mutable std::shared_mutex mtx;
	MarketData data;
};

class DataManager {
public:
	DataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger, int market_data_type = 3);

	void setMarketDataType(int market_data_type = 3) const;

	int getMarketDataType() const;

	void marketDataSub(const Contract& contract, const std::string& tick_list = "", bool snap = false,
	                   bool reg_snap = false);

	[[nodiscard]] MarketData getMarketData(const Contract& contract);

	[[nodiscard]] MarketData getMarketData(int req_id) const;

	[[nodiscard]] bool hasPrice(const Contract& contract);

	[[nodiscard]] std::optional<double> getPrice(const Contract& contract);

	[[nodiscard]] double getAsk(const Contract& contract);
	[[nodiscard]] double getBid(const Contract& contract);
	[[nodiscard]] double getLast(const Contract& contract);

	[[nodiscard]] bool hasSubscribed(const Contract& contract);

	[[nodiscard]] int getNextId() const;
	[[nodiscard]] int getCurrentId() const;

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::unordered_map<int, MarketDataStore>* data_map_;
	std::unordered_map<int, int> conId_to_id_map_;
	std::unordered_set<int> subscribed_conId_;
};
