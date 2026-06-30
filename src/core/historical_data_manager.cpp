#include "quantib/core/historical_data_manager.h"

#include <ctime>

#include "EClientSocket.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/utils/logger.hpp"

HistoricalDataManager::HistoricalDataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: logger_(logger), client_(client), obj_(obj), hub_(hub),
	  historical_series_(obj_.create<HistoricalDataStoreTag, std::unordered_map<int, HistoricalSeries>>()) {
	LOG_DEBUG_TAG(WRAPPER, "HistoricalDataManager initialized");
}

void HistoricalDataManager::requestHistoricalBars(const Contract& contract, const std::string& end_datetime,
                                                  const std::string& duration, const std::string& bar_size,
                                                  const std::string& what_to_show) {
	client_.reqHistoricalData(getNextId(), contract, end_datetime, duration, bar_size, what_to_show, 1, 1, false,
	                          TagValueListSPtr());
}

int HistoricalDataManager::subscribeHistoricalBars(const Contract& contract, const std::string& end_datetime,
                                                    const std::string& duration, const std::string& bar_size,
                                                    const std::string& what_to_show) {
	// Insert into the map the new subscription
	const int reqId = getNextId();
	// Register conId
	conId_to_req_id_.try_emplace(contract.conId, reqId);
	HistoricalSeriesConfig cfg{reqId, contract.symbol, end_datetime, duration, bar_size, what_to_show, true};

	historical_series_.try_emplace(reqId, cfg);   // no move/copy of HistoricalSeries due to shared_mtx
	client_.reqHistoricalData(reqId, contract, end_datetime, duration, bar_size, what_to_show, 1, 1, true,
	                          TagValueListSPtr());

	return reqId;
}

int HistoricalDataManager::getNextId() const { return obj_.get_increment_int<NextIdTag>(); }

int HistoricalDataManager::getCurrentId() const { return *obj_.try_get<NextIdTag, int>(); }

std::optional<HistoricalSeriesSnapshot> HistoricalDataManager::snapshot(const int req_id) const {
	const auto* series = obj_.find_in_map<HistoricalDataStoreTag, int, HistoricalSeries>(req_id);
	if (!series) return std::nullopt;

	return series->snapshot_unlocked();
}

std::optional<HistoricalSeriesSnapshot> HistoricalDataManager::snapshot(const Contract& contract) {
	const int req_id = conId_to_req_id_[contract.conId];
	return snapshot(req_id);
}