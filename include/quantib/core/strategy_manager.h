#pragma once
#include <string>

#include "Contract.h"
#include "EClientSocket.h"
#include "object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "config.h"

template <typename StrategyT, typename RiskManagerT>
class StrategyManager {
public:
	StrategyManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger, RiskManagerT& risk)
		: logger_(logger), client_(client), obj_(obj), hub_(hub), risk_(risk), strategy_() {}

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;
	RiskManagerT& risk_;

	StrategyT strategy_;
};
