#include "quantib/core/data_manager.h"

[[nodiscard]] double DataManager::getAsk(const Contract& contract) {
	const auto market_data = getMarketData(contract);
	for (const auto& type : {TickType::ASK, TickType::DELAYED_ASK}) {
		if (market_data.price_map.contains(type)) { return market_data.price_map.at(type); }
	}
	return -1;
}

[[nodiscard]] double DataManager::getBid(const Contract& contract) {
	const auto market_data = getMarketData(contract);
	for (const auto& type : {TickType::BID, TickType::DELAYED_BID}) {
		if (market_data.price_map.contains(type)) { return market_data.price_map.at(type); }
	}
	return -1;
}

[[nodiscard]] double DataManager::getLast(const Contract& contract) {
	const auto market_data = getMarketData(contract);
	for (const auto& type : {TickType::LAST, TickType::DELAYED_LAST}) {
		if (market_data.price_map.contains(type)) { return market_data.price_map.at(type); }
	}
	return -1;
}
