#pragma once
#include "quantib/risk_manager/risk_manager.h"

// In logger.hpp you can choose if tick data should be displayed with ENABLE_TICK_LOGGING

using MyRiskManager = RiskManager<
	ContractReady<true, FixedRetry<10, 500>>,
	MarketIsOpen<false>,
	MaxOrderNotional<1000, Side::Buy>,
	MaxOrderNotional<1000, Side::Sell>
>;