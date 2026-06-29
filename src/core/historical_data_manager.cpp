#include "quantib/core/historical_data_manager.h"

#include <ctime>

#include "EClientSocket.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/utils/logger.hpp"

HistoricalDataManager::HistoricalDataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: logger_(logger), client_(client), obj_(obj), hub_(hub) {
	LOG_DEBUG_TAG(WRAPPER, "HistoricalDataManager initialized");
}

void HistoricalDataManager::requestHistoricalBars(
	const Contract& contract,
	const std::string& end_datetime,
	const std::string& duration,
	const std::string& bar_size,
	const std::string& what_to_show,
	const bool keep_up_to_date) {

	client_.reqHistoricalData(getNextId(), contract, end_datetime, duration, bar_size, what_to_show, 1, 1, false, TagValueListSPtr());
}

int HistoricalDataManager::getNextId() const {
	return obj_.get_increment_int<NextIdTag>();
}

int HistoricalDataManager::getCurrentId() const {
	return *obj_.try_get<NextIdTag, int>();
}