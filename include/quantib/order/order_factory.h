#pragma once
#include <stdexcept>
#include <utility>
#include <vector>

#include "Order.h"
#include "quantib/concepts/risk_manager_concepts.hpp"
#include "quantib/order/order_types.hpp"
#include "trade_request.h"

enum class OrderBatchType { Single, AllOrNone, Multi };

struct OrderBatch {
	OrderBatchType batch_type;
	std::vector<Contract> contracts;
	std::vector<Order> orders;

	bool isEmpty() const { return orders.empty(); }
	std::size_t size() const { return orders.size(); }
};

template <RiskManagerConcept RiskManagerT>
class OrderFactory {
public:
	using RiskManager = RiskManagerT;

	explicit OrderFactory(Logger& logger, ObjectHub &hub) : logger_(logger), obj_(hub) {}

	void setRiskManager(RiskManager& risk) { risk_ = &risk; }

	[[nodiscard]] OrderBatch make(const TradeRequest& request) const;

	[[nodiscard]] bool check(const TradeRequest& request) const;

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const MarketTradeSpec &spec, const int order_id) {
		const std::string action = to_ib_action(request.context.side);
		const Decimal quantity = to_decimal(spec.qty);

		switch (spec.type) {
		case OrderType::Market:
			return make_single(request, OrderSamples::MarketOrder(action, quantity), order_id);
		case OrderType::MarketOnClose:
			return make_single(request, OrderSamples::MarketOnClose(action, quantity), order_id);
		case OrderType::MarketOnOpen:
			return make_single(request, OrderSamples::MarketOnOpen(action, quantity), order_id);
		case OrderType::MidpointMatch:
			return make_single(request, OrderSamples::MidpointMatch(action, quantity), order_id);
		case OrderType::MarketToLimit:
			return make_single(request, OrderSamples::MarketToLimit(action, quantity), order_id);
		case OrderType::MarketWithProtection:
			return make_single(request, OrderSamples::MarketWithProtection(action, quantity), order_id);
		default:
			throw std::invalid_argument("MarketTradeSpec requires a market-family OrderType.");
		}
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const LimitTradeSpec &spec, const int order_id) {
		const std::string action = to_ib_action(request.context.side);
		const Decimal quantity = to_decimal(spec.qty);

		switch (spec.type) {
		case OrderType::Limit:
			return make_single(request, OrderSamples::LimitOrder(action, quantity, spec.limit_price), order_id);
		case OrderType::LimitOnClose:
			return make_single(request, OrderSamples::LimitOnClose(action, quantity, spec.limit_price), order_id);
		case OrderType::LimitOnOpen:
			return make_single(request, OrderSamples::LimitOnOpen(action, quantity, spec.limit_price), order_id);
		default:
			throw std::invalid_argument("LimitTradeSpec requires a limit-family OrderType.");
		}
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const BracketOrderSpec &spec, const int order_id) {
		Order parent;
		Order take_profit;
		Order stop_loss;

		OrderSamples::BracketOrder(
			order_id,
			parent,
			take_profit,
			stop_loss,
			to_ib_action(request.context.side),
			to_decimal(spec.qty),
			spec.entry_price,
			spec.take_profit_price,
			spec.stop_loss_price);

		return make_multi(request, {parent, take_profit, stop_loss});
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const MarketIfTouchedTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::MarketIfTouched(to_ib_action(request.context.side), to_decimal(spec.qty), spec.trigger_price),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const MidpriceTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::Midprice(to_ib_action(request.context.side), to_decimal(spec.qty), spec.price_cap),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const PeggedToMarketTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::PeggedToMarket(to_ib_action(request.context.side), to_decimal(spec.qty), spec.market_offset),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const LimitWithCashQtyTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::LimitOrderWithCashQty(to_ib_action(request.context.side), spec.limit_price, spec.cash_qty),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const LimitIfTouchedTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::LimitIfTouched(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.limit_price,
				spec.trigger_price),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const StopTradeSpec &spec, const int order_id) {
		switch (spec.type) {
		case OrderType::Stop:
			return make_single(
				request,
				OrderSamples::Stop(to_ib_action(request.context.side), to_decimal(spec.qty), spec.stop_price),
				order_id);
		case OrderType::StopWithProtection:
			return make_single(
				request,
				OrderSamples::StopWithProtection(to_ib_action(request.context.side), to_decimal(spec.qty), spec.stop_price),
				order_id);
		default:
			throw std::invalid_argument("StopTradeSpec requires Stop or StopWithProtection.");
		}
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const StopLimitTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::StopLimit(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.limit_price,
				spec.stop_price),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const TrailingStopTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::TrailingStop(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.trailing_percent,
				spec.trail_stop_price),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const TrailingStopLimitTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::TrailingStopLimit(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.lmt_price_offset,
				spec.trailing_amount,
				spec.trail_stop_price),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const ComboLimitTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::ComboLimitOrder(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.limit_price,
				spec.non_guaranteed),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const ComboMarketTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::ComboMarketOrder(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.non_guaranteed),
			order_id);
	}

	[[nodiscard]] static OrderBatch build_order(const TradeRequest& request, const LimitComboWithLegPricesTradeSpec &spec, const int order_id) {
		return make_single(
			request,
			OrderSamples::LimitOrderForComboWithLegPrices(
				to_ib_action(request.context.side),
				to_decimal(spec.qty),
				spec.leg_prices,
				spec.non_guaranteed),
			order_id);
	}

private:
	[[nodiscard]] int getNextId() const { return obj_.get_increment_int<NextIdTag>(); }
	[[nodiscard]] int getCurrentId() const { return *obj_.try_get<NextIdTag, int>(); }

	template <typename T>
	[[nodiscard]] static constexpr int order_count(const T&) { return 1; }

	[[nodiscard]] static constexpr int order_count(const BracketOrderSpec&) { return 3; }

	static std::string to_ib_action(const Side side) {
		return side == Side::Buy ? "BUY" : "SELL";
	}

	static Decimal to_decimal(const double qty) {
		return DecimalFunctions::doubleToDecimal(qty);
	}

	static OrderBatch make_single(const TradeRequest& request, Order order, const int order_id) {
		order.orderId = order_id;
		return OrderBatch(OrderBatchType::Single, {request.context.contract}, {order});
	}

	static OrderBatch make_multi(const TradeRequest& request, std::vector<Order> orders) {
		std::vector<Contract> contracts(orders.size(), request.context.contract);
		return OrderBatch(OrderBatchType::Multi, std::move(contracts), std::move(orders));
	}

	Logger& logger_;
	ObjectHub &obj_;
	RiskManager* risk_ = nullptr;
};

#include "order_factory.tpp"