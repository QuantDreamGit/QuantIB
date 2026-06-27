#pragma once

#include <concepts>
#include <future>

#include "Contract.h"
#include "quantib/risk_manager/risk_types.hpp"
#include "quantib/order/trade_request.h"

template <typename T>concept RiskManagerConcept = requires(T risk_manager, const RiskResult& result,
                                                           const TradeRequest& request, const Contract& contract) {
	{ risk_manager.start() } -> std::same_as<void>;
	{ risk_manager.stop() } -> std::same_as<void>;
	{ risk_manager.validate(request) } -> std::same_as<std::future<RiskResult>>;
	{ risk_manager.executeAction(result, contract) } -> std::same_as<void>;
};
