#include "quantib/wrappers/base_wrapper.hpp"

#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/core/historical_data_manager.h"
#include "quantib/network/connection.hpp"
#include "quantib/options/option_chain.hpp"
#include "quantib/utils/logger.hpp"

ResponseWrapper::ResponseWrapper(BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: hub_(hub), obj_(obj), logger_(logger) { LOG_DEBUG_TAG(WRAPPER, "Initialized correctly."); }

void ResponseWrapper::nextValidId(const int orderId) {
	if (hub_.containsKey<ConnectTag>()) { hub_.send<ConnectTag>(orderId); }
	else { hub_.send<NextValidIdTag>(orderId); }
}

void ResponseWrapper::securityDefinitionOptionalParameter(const int reqId, const std::string& exchange,
                                                          const int underlyingConId, const std::string& tradingClass,
                                                          const std::string& multiplier,
                                                          const std::set<std::string>& expirations,
                                                          const std::set<double>& strikes) {
	LOG_DEBUG_TAG(
		WRAPPER,
		"OptionChain received: reqId={}, underlyingConId={}, tradingClass={}, multiplier={}, exchange={}, expirations.size()={}, strikes.size()={}",
		reqId, underlyingConId, tradingClass, multiplier, exchange, expirations.size(), strikes.size());
	obj_.try_append<SecurityDefinitionOptionalParameterTag, OptionChain>(
		OptionChain(reqId, underlyingConId, tradingClass, multiplier, exchange, expirations, strikes));
}

void ResponseWrapper::securityDefinitionOptionalParameterEnd(const int reqId) {
	auto* object = obj_.try_get<SecurityDefinitionOptionalParameterTag, std::vector<OptionChain>>();
	if (object) {
		LOG_DEBUG_TAG(WRAPPER, "OptionChain received: reqId={}, Number of exchanges: {}", reqId, object->size());
	}
	else { LOG_DEBUG_TAG(WRAPPER, "OptionChain not received: reqId={}", reqId); }
	hub_.send<SecurityDefinitionOptionalParameterTag, std::vector<OptionChain>>(*object);
}

void ResponseWrapper::historicalData(const int reqId, const Bar& bar) {
	std::cout << "historical data: reqId: " << reqId << " {time: " << bar.time << ", open: " << bar.open << ", high: "
		<< bar.high << ", low: " << bar.low << ", close: " << bar.close << ", wap: " <<
		DecimalFunctions::decimalToString(bar.wap) << ", volume: " << DecimalFunctions::decimalToString(bar.volume) <<
		", count: " << bar.count << "}" << std::endl;
}

void ResponseWrapper::historicalSchedule(int reqId, const std::string& startDateTime, const std::string& endDateTime,
                                         const std::string& timeZone, const std::vector<HistoricalSession>& sessions) {
	std::cout << "reqId: " << reqId << " startDateTime: " << startDateTime << " endDateTime: " << endDateTime <<
		"timeZone: " << timeZone << " sessions.size(): " << sessions.size() << std::endl;
}

void ResponseWrapper::historicalDataUpdate(const int reqId, const Bar& bar) {
	std::cout << "historical update: reqId: " << reqId << " {time: " << bar.time << ", open: " << bar.open << ", high: "
		<< bar.high << ", low: " << bar.low << ", close: " << bar.close << ", wap: " <<
		DecimalFunctions::decimalToString(bar.wap) << ", volume: " << DecimalFunctions::decimalToString(bar.volume) <<
		", count: " << bar.count << "}" << std::endl;
}

void ResponseWrapper::historicalDataEnd(const int reqId, const std::string& startDateStr,
                                        const std::string& endDateStr) {
	std::cout << "reqId :" << reqId << " startDateStr: " << startDateStr << " endDateStr: " << endDateStr << std::endl;
}

void ResponseWrapper::headTimestamp(const int reqId, const std::string& headTimestamp) {
	std::cout << "reqId: " << reqId << " headTimestamp: " << headTimestamp << std::endl;
}
