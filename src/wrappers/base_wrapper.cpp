#include "quantib/wrappers/base_wrapper.hpp"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/core/historical_data_manager.h"
#include "quantib/network/connection.hpp"
#include "quantib/options/option_chain.hpp"
#include "quantib/utils/logger.hpp"

#include <mutex>

namespace {
	OhlcBar toOhlcBar(const Bar& bar, BarLifecycle lifecycle) {
		OhlcBar out;
		out.timestamp = bar.time;
		out.open = bar.open;
		out.high = bar.high;
		out.low = bar.low;
		out.close = bar.close;
		out.wap = DecimalFunctions::decimalToDouble(bar.wap);
		out.volume = static_cast<long long>(DecimalFunctions::decimalToDouble(bar.volume));
		out.count = bar.count;
		out.lifecycle = lifecycle;
		out.updated_at = std::chrono::system_clock::now();
		return out;
	}
} // namespace

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

	auto* series = obj_.find_in_map<HistoricalDataStoreTag, int, HistoricalSeries>(reqId);
	if (!series) return;

	std::unique_lock<std::shared_mutex> lock(series->mtx);
	series->bars.emplace_back(toOhlcBar(bar, BarLifecycle::HistoricalClosed));
	series->last_timestamp = bar.time;
	series->last_update = std::chrono::system_clock::now();
}

void ResponseWrapper::historicalDataUpdate(const int reqId, const Bar& bar) {
	/*
	std::cout << "historical update: reqId: " << reqId << " {time: " << bar.time << ", open: " << bar.open << ", high: "
		<< bar.high << ", low: " << bar.low << ", close: " << bar.close << ", wap: " <<
		DecimalFunctions::decimalToString(bar.wap) << ", volume: " << DecimalFunctions::decimalToString(bar.volume) <<
		", count: " << bar.count << "}" << std::endl;
	*/
	auto* series = obj_.find_in_map<HistoricalDataStoreTag, int, HistoricalSeries>(reqId);
	if (!series) { return; }

	std::optional<OhlcBar> updated_bar;
	std::optional<OhlcBar> closed_bar;

	{
		std::unique_lock<std::shared_mutex> lock(series->mtx);
		OhlcBar live = toOhlcBar(bar, BarLifecycle::LiveUpdating);

		if (!series->active_bar.has_value()) {
			if (!series->bars.empty() && series->bars.back().timestamp == live.timestamp) {
				series->active_bar = series->bars.back();
				series->bars.pop_back();

				series->active_bar->lifecycle = BarLifecycle::LiveUpdating;
				series->active_bar->updated_at = std::chrono::system_clock::now();
				series->active_bar->open = live.open;
				series->active_bar->high = live.high;
				series->active_bar->low = live.low;
				series->active_bar->close = live.close;
				series->active_bar->wap = live.wap;
				series->active_bar->volume = live.volume;
				series->active_bar->count = live.count;
			}
			else { series->active_bar = std::move(live); }

			updated_bar = series->active_bar;
		}
		else if (series->active_bar->timestamp == live.timestamp) {
			series->active_bar->open = live.open;
			series->active_bar->high = live.high;
			series->active_bar->low = live.low;
			series->active_bar->close = live.close;
			series->active_bar->wap = live.wap;
			series->active_bar->volume = live.volume;
			series->active_bar->count = live.count;
			series->active_bar->updated_at = std::chrono::system_clock::now();

			updated_bar = series->active_bar;
		}
		else {
			series->active_bar->lifecycle = BarLifecycle::LiveClosed;

			closed_bar = *series->active_bar;
			series->bars.push_back(*series->active_bar);

			series->active_bar = std::move(live);
			updated_bar = series->active_bar;
		}

		series->last_timestamp = bar.time;
		series->last_update = std::chrono::system_clock::now();
	}

	// Callbacks happen after releasing series->mtx.
	if (closed_bar) {
		if (const auto* callback = obj_.try_get<onBarClosedTag, BarClosedCallback>()) {
			(*callback)(reqId, *closed_bar);
		}
	}

	if (updated_bar) {
		if (const auto* callback = obj_.try_get<onBarUpdateTag, BarUpdateCallback>()) {
			(*callback)(reqId, *updated_bar);
		}
	}
}

void ResponseWrapper::historicalDataEnd(const int reqId, const std::string& startDateStr,
                                        const std::string& endDateStr) {
	std::cout << "reqId :" << reqId << " startDateStr: " << startDateStr << " endDateStr: " << endDateStr << std::endl;
	auto* series = obj_.find_in_map<HistoricalDataStoreTag, int, HistoricalSeries>(reqId);
	if (!series) return;

	if (const auto* callback = obj_.try_get<onBarSeriesCompleteTag, BarSeriesCompleteCallback>())
		callback->operator( )(reqId);

	auto snapshot = [&] {
		std::unique_lock<std::shared_mutex> lock(series->mtx);

		series->historical_complete = true;
		series->last_update = std::chrono::system_clock::now();

		return series->snapshot_unlocked();
	}();
}
