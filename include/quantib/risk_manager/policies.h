#pragma once

#include "Order.h"
#include "quantib/core/contract_manager.h"

#include "quantib/core/data_manager.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/order_manager.h"
#include "quantib/core/position_manager.h"

enum class Side {
	Buy,
	Sell
};

struct RiskContext {
	ObjectHub &obj;
	Logger &logger_;
	OrderManager &order;
	DataManager &data;
	PositionManager &position;
	ContractManager &contract;

	RiskContext(ObjectHub &hub_, Logger &logger_, OrderManager &order_, DataManager &data_, PositionManager
		&position_, ContractManager &contract_)
		: obj(hub_), logger_(logger_), order(order_), data(data_), position(position_), contract(contract_) {}
};

template<bool RequireContractDetails = true>
struct ContractReady {
	static bool check(const Order& order,
					  const Contract& contract,
					  const RiskContext& ctx) {
		if constexpr (RequireContractDetails) {
			if (const auto contract_info = ctx.contract.getContractInfo(contract.symbol);
				contract_info.has_value() && contract_info.value().pending_details == false) return true;

			ctx.LOG_WARN_TAG(RISK, "Rejected: invalid/missing contract for {}.", contract.symbol);
			return false;
		}
		return true;
	}
};


template <bool TradingHours = true>
struct OnlyTradingHours {
	static bool check(const Order& order,
					  const Contract& contract,
					  const RiskContext& ctx) {
		if constexpr (TradingHours) {
			if (const auto contract_info = ctx.contract.getContractInfo(contract.symbol);
				contract_info.has_value() && contract_info.value().isInLiquidHours() == true) return true;
			ctx.LOG_WARN_TAG(RISK, "Rejected: order outside trading hours for {}.", contract.symbol);
			return false;
		}
		return true;
	}
};


template<int MaxNotional, Side OrderSide>
struct MaxOrderNotional {
	static bool check(const Order& order,
					  const Contract& contract,
					  const RiskContext& ctx) {
		double price{};

		if constexpr (OrderSide == Side::Buy) {
			if (order.action != "BUY") return true;
			price = ctx.data.getAsk(contract);
		} else if constexpr (OrderSide == Side::Sell) {
			if (order.action != "SELL") return true;
			price = ctx.data.getBid(contract);
		}

		if (price <= 0.0) {
			ctx.LOG_WARN_TAG(RISK, "Rejected: invalid/missing price for {}.", contract.symbol);
			return false;
		}

		const double qty = DecimalFunctions::decimalToDouble(order.totalQuantity);

		if (const double notional = price * qty; notional > MaxNotional) {
			ctx.LOG_WARN_TAG(RISK,"Rejected: notional {} exceeds max {} for {}.",
				notional, MaxNotional, contract.symbol);
			return false;
		}

		return true;
	}
};

