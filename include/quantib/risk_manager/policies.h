#pragma once

#include "Order.h"
#include "retry.h"
#include "structs.h"

// TODO: Add policies
/*
 * SpreadWithinLimit
 * PriceNotStale
 * MaxPosition
 * CashAvailable / BuyingPowerAvailable
 * TradingEnabled
 * MaxOrdersPerMinute
 * LimitPriceReasonable
 */

template <typename RiskManagerT, bool RequireContractDetails = true, typename Retry = NoRetry>
struct ContractReady {
	static RiskResult check(const Order&, const Contract& contract, const RiskContext<RiskManagerT>& ctx) {
		if constexpr (RequireContractDetails) {
			const auto contract_info = ctx.contract.getContractInfo(contract.symbol);
			if (contract_info.has_value()) {
				if (contract_info.value().pending_details == false) { return RiskResult::pass(); }
				// We have to wait data
				return RiskResult::pending("Contract details pending for " + contract.symbol, RiskAction::Wait,
				                           Retry{});
			}
			// We have no data contract
			return RiskResult::pending("Contract data missing for " + contract.symbol, RiskAction::RegisterContract,
			                           Retry{});
		}
		// If RequireContractDetails = false
		return RiskResult::pass();
	}
};

template <typename RiskManagerT, bool RequireMktDetails = true, typename Retry = NoRetry>
struct MktDataReady {
	static RiskResult check(const Order&, const Contract& contract, const RiskContext<RiskManagerT>& ctx) {
		if constexpr (RequireMktDetails) {
			// If not subscribed
			if (const bool has_sub = ctx.data.hasSubscribed(contract); !has_sub) {
				return RiskResult::pending(
					"Contract " + contract.symbol + "has not subscribed for market data. Trying to subscribe...",
					RiskAction::RegisterMktData, Retry{});
			}
			// If it has already prices
			if (const auto price_info = ctx.data.getPrice(contract); price_info.has_value()) {
				return RiskResult::pass();
			}

			// Otherwise
			return RiskResult::pending("Waiting missing prices for " + contract.symbol, RiskAction::Wait, Retry{});
		}

		return RiskResult::pass();
	}
};

template <typename RiskManagerT, bool TradingHours = true>
struct MarketIsOpen {
	static RiskResult check(const Order&, const Contract& contract, const RiskContext<RiskManagerT>& ctx) {
		if constexpr (TradingHours) {
			const auto contract_info = ctx.contract.getContractInfo(contract.symbol);
			if (contract_info.has_value()) {
				if (contract_info.value().isInLiquidHours() == true) return RiskResult::pass();
				return RiskResult::reject("Order outside trading hours for " + contract.symbol);
			}
			// We have no data contract
			return RiskResult::pending("Contract data missing for " + contract.symbol, RiskAction::RegisterContract);
		}
		return RiskResult::pass();
	}
};

template <typename RiskManagerT, int MaxNotional, Side OrderSide>
struct MaxOrderNotional {
	static RiskResult check(const Order& order, const Contract& contract, const RiskContext<RiskManagerT>& ctx) {
		double price{};

		if constexpr (OrderSide == Side::Buy) {
			if (order.action != "BUY") return RiskResult::pass();
			price = ctx.data.getAsk(contract);
		}
		else if constexpr (OrderSide == Side::Sell) {
			if (order.action != "SELL") return RiskResult::pass();
			price = ctx.data.getBid(contract);
		}

		if (price <= 0.0) {
			return RiskResult::pending("Market data missing for " + contract.symbol, RiskAction::RegisterMktData);
		}

		const double qty = DecimalFunctions::decimalToDouble(order.totalQuantity);
		if (const double notional = price * qty; notional > MaxNotional) {
			return RiskResult::reject(
				"Order notional " + std::to_string(notional) + " exceeds max notional of " +
				std::to_string(MaxNotional));
		}

		return RiskResult::pass();
	}
};
