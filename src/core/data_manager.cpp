#include "quantib/core/data_manager.h"

#include "EClientSocket.h"
#include "EWrapper.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

GreeksData::GreeksData(double implied_volatility_, double delta_, double price_, double pv_dividend_, double gamma_,
                       double vega_, double theta_, double und_price_)
	: implied_volatility(implied_volatility_), delta(delta_), price(price_), pv_dividend(pv_dividend_),
	  gamma(gamma_), vega(vega_), theta(theta_), und_price(und_price_) {}

bool MarketData::has_price() const {
	if (const auto price = getPrice(); price.has_value()) return true;
	return false;
}

std::optional<double> MarketData::getPrice() const {
	if (price_map.empty()) return std::nullopt;

	static constexpr TickType kPriority[] = {
		TickType::LAST, TickType::BID, TickType::ASK, TickType::DELAYED_LAST, TickType::DELAYED_BID,
		TickType::DELAYED_ASK, TickType::MARK_PRICE, TickType::CLOSE, TickType::DELAYED_CLOSE, TickType::OPEN,
		TickType::DELAYED_OPEN
	};

	for (TickType t : kPriority) {
		auto it = price_map.find(t);
		if (it != price_map.end()) { return it->second; }
	}

	return -1.0;
}

DataManager::DataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger,
                         int market_data_type)
	: logger_(logger), client_(client), obj_(obj), hub_(hub) {
	setMarketDataType(market_data_type);
	data_map_ = &obj_.create<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
}

void DataManager::setMarketDataType(const int market_data_type) const {
	client_.reqMarketDataType(market_data_type);
}

int DataManager::getMarketDataType() const {
	return *obj_.try_get<MarketDataTypeTag, int>();
}

void DataManager::marketDataSub(const Contract& contract, const std::string& tick_list, bool snap,
                                bool reg_snap) {
	conId_to_id_map_[contract.conId] = getNextId();
	client_.reqMktData(getCurrentId(), contract, tick_list, snap, reg_snap, TagValueListSPtr());
	subscribed_conId_.insert(getCurrentId());
}

MarketData DataManager::getMarketData(const Contract& contract) {
	return getMarketData(conId_to_id_map_[contract.conId]);
}

MarketData DataManager::getMarketData(const int req_id) const {
	const auto it = data_map_->find(req_id);
	if (it == data_map_->end()) return {};

	return it->second.data;
}

bool DataManager::hasPrice(const Contract& contract) {
	if (const MarketData res = getMarketData(contract); res.has_price()) return true;
	return false;
}

std::optional<double> DataManager::getPrice(const Contract& contract) {
	const MarketData res = getMarketData(contract);
	return res.getPrice();
}

bool DataManager::hasSubscribed(const Contract& contract) {
	if (subscribed_conId_.contains(conId_to_id_map_[contract.conId])) return true;
	return false;
}

int DataManager::getNextId() const {
	return obj_.get_increment_int<NextIdTag>();
}

int DataManager::getCurrentId() const {
	return *obj_.try_get<NextIdTag, int>();
}
