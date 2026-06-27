#include "quantib/core/data_manager.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/wrappers/base_wrapper.hpp"
#include "nameof.hpp"

void ResponseWrapper::tickGeneric(int reqId, TickType tickType, double value) {
	auto* store_map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
	if (!store_map) return;

	std::unique_lock lock((*store_map)[reqId].mtx);
	(*store_map)[reqId].data.generic_map[tickType] = value;

	LOG_TICK(WRAPPER, "Received tick generic for reqId {}: tickType {}, value {}", reqId, std::to_string(tickType),
	         value);
}

void ResponseWrapper::tickPrice(int reqId, const TickType field, double price, const TickAttrib&) {
	auto* store_map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
	if (!store_map) return;

	std::unique_lock lock((*store_map)[reqId].mtx);
	(*store_map)[reqId].data.price_map[field] = price;
	LOG_TICK(WRAPPER, "Received tick price for reqId {}: field {}, price {}", reqId, std::to_string(field), price);
}

void ResponseWrapper::tickSize(int reqId, TickType field, Decimal size) {
	auto* store_map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
	if (!store_map) return;

	std::unique_lock lock((*store_map)[reqId].mtx);
	(*store_map)[reqId].data.size_map[field] = size;
	LOG_TICK(WRAPPER, "Received tick size for reqId {}: field {}, size {}", reqId, std::to_string(field),
	         DecimalFunctions::decimalToString(size));
}

void ResponseWrapper::tickString(int reqId, TickType tickType, const std::string& value) {
	auto* store_map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
	if (!store_map) return;

	std::unique_lock lock((*store_map)[reqId].mtx);
	(*store_map)[reqId].data.string_map[tickType] = value;
	LOG_TICK(WRAPPER, "Received tick string for reqId {}: tickType {}, value {}", reqId, std::to_string(tickType),
	         value);
}

void ResponseWrapper::tickOptionComputation(int reqId, TickType tickType, int, double impliedVol,
                                            double delta, double optPrice, double pvDividend, double gamma, double vega,
                                            double theta, double undPrice) {
	auto* store_map = obj_.try_get<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
	if (!store_map) return;

	std::unique_lock lock((*store_map)[reqId].mtx);
	(*store_map)[reqId].data.option_map.insert_or_assign(
		tickType, GreeksData(impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice));
	LOG_TICK(WRAPPER, "Received tick option computation for reqId {}: tickType: {}", reqId, NAMEOF_ENUM(tickType));
}
