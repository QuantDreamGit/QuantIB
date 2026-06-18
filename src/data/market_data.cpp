#include "quantib/core/data_manager.h"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::tickGeneric(int reqId, TickType tickType, double value) {
	auto *map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketData>>();
	(*map)[reqId].generic_map[tickType] = value;


	LOG_TICK(WRAPPER, "Received tick generic for reqId {}: tickType {}, value {}", reqId,
				 std::to_string(tickType),
				 value);
}

void ResponseWrapper::tickPrice(int reqId, const TickType field, double price, const TickAttrib &attrib) {
	auto *map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketData>>();
	(*map)[reqId].price_map[field] = price;
	LOG_TICK(WRAPPER, "Received tick price for reqId {}: field {}, price {}, attrib {}", reqId,
	             std::to_string(field),
	             price,
	             std::to_string(attrib.canAutoExecute));
}

void ResponseWrapper::tickSize(int reqId, TickType field, Decimal size) {
	auto *map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketData>>();
	(*map)[reqId].size_map[field] = size;
	LOG_TICK(WRAPPER, "Received tick size for reqId {}: field {}, size {}", reqId,
				 std::to_string(field),
				 DecimalFunctions::decimalToString(size));
}

void ResponseWrapper::tickString(int reqId, TickType tickType, const std::string &value) {
	auto *map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketData>>();
	(*map)[reqId].string_map[tickType] = value;
	LOG_TICK(WRAPPER, "Received tick string for reqId {}: tickType {}, value {}", reqId,
	             std::to_string(tickType),
	             value);
}