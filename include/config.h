#pragma once
#include "quantib/risk_manager/risk_manager.h"

// In logger.hpp you can choose if tick data should be displayed with ENABLE_TICK_LOGGING

using MyRiskManager = RiskManager<
		ContractReady<>,					// Check if contract is ready
		OnlyTradingHours<>,					// Check if order is within trading hours
		MaxOrderNotional<1000, Side::Buy>,	// Max Notional for long side
		MaxOrderNotional<1000, Side::Sell> 	// Max Notional for short side
	>;