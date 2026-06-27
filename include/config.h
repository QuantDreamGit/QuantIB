#pragma once

#include "quantib/risk_manager/risk_manager.h"

// In logger.hpp you can choose if tick data should be displayed with
// ENABLE_TICK_LOGGING

class DefaultRiskManager;

using DefaultRiskManagerBase = RiskManager<DefaultRiskManager, ContractReady<
	                                           DefaultRiskManager, true, FixedRetry<10, 500>>, MktDataReady<
	                                           DefaultRiskManager, true, FixedRetry<10, 500>>, MarketIsOpen<
	                                           DefaultRiskManager, true>, MaxOrderNotional<
	                                           DefaultRiskManager, 1000, Side::Buy>, MaxOrderNotional<
	                                           DefaultRiskManager, 1000, Side::Sell>>;

class DefaultRiskManager : public DefaultRiskManagerBase {
public:
	using DefaultRiskManagerBase::DefaultRiskManagerBase;
	using DefaultRiskManagerBase::validate;
	using DefaultRiskManagerBase::start;
	using DefaultRiskManagerBase::stop;
	using DefaultRiskManagerBase::executeAction;
};

class DefaultStrategy {
public:
	DefaultStrategy() = default;
};

struct DefaultProfile {
	using RiskManager = DefaultRiskManager;
	using Strategy = DefaultStrategy;
};
