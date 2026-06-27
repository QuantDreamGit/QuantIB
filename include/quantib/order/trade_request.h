#pragma once
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "Contract.h"

enum class OrderType {
	Market,
	Limit,
	MarketIfTouched,
	MarketOnClose,
	MarketOnOpen,
	MidpointMatch,
	Midprice,
	PeggedToMarket,
	LimitWithCashQty,
	LimitIfTouched,
	LimitOnClose,
	LimitOnOpen,
	Bracket,
	MarketToLimit,
	MarketWithProtection,
	Stop,
	StopLimit,
	StopWithProtection,
	TrailingStop,
	TrailingStopLimit,
	ComboLimit,
	ComboMarket,
	LimitComboWithLegPrices
};

enum class Side {
	Buy,
	Sell
};

struct TradeRequestContext {
	Contract contract;
	Side side;
	std::optional<std::string> reason;

	TradeRequestContext(const Contract& contract, const Side side, std::optional<std::string> reason) :
		contract(contract), side(side), reason(std::move(reason)) {}

	TradeRequestContext(const Contract& contract, const Side side) : contract(contract), side(side),
	                                                                 reason(std::nullopt) {}
};

struct MarketTradeSpec {
	double qty;
	OrderType type;

	MarketTradeSpec(const double qty, const OrderType type) : qty(qty), type(type) {}
};

struct LimitTradeSpec {
	double qty;
	OrderType type;
	double limit_price;

	LimitTradeSpec(const double qty, const OrderType type, const double limit_price) : qty(qty), type(type), limit_price(limit_price) {}
};

struct BracketOrderSpec {
	double qty;
	double entry_price;
	double take_profit_price;
	double stop_loss_price;
	static constexpr OrderType type = OrderType::Bracket;

	BracketOrderSpec(const double qty, const double entry_price, const double take_profit_price, const double stop_loss_price) :
		qty(qty), entry_price(entry_price), take_profit_price(take_profit_price), stop_loss_price(stop_loss_price) {}
};

struct MarketIfTouchedTradeSpec {
	double qty;
	double trigger_price;
	static constexpr OrderType type = OrderType::MarketIfTouched;

	MarketIfTouchedTradeSpec(const double qty, const double trigger_price) : qty(qty), trigger_price(trigger_price) {}
};

struct MidpriceTradeSpec {
	double qty;
	double price_cap;
	static constexpr OrderType type = OrderType::Midprice;

	MidpriceTradeSpec(const double qty, const double price_cap) : qty(qty), price_cap(price_cap) {}
};

struct PeggedToMarketTradeSpec {
	double qty;
	double market_offset;
	static constexpr OrderType type = OrderType::PeggedToMarket;

	PeggedToMarketTradeSpec(const double qty, const double market_offset) : qty(qty), market_offset(market_offset) {}
};

struct LimitWithCashQtyTradeSpec {
	double limit_price;
	double cash_qty;
	static constexpr OrderType type = OrderType::LimitWithCashQty;

	LimitWithCashQtyTradeSpec(const double limit_price, const double cash_qty) : limit_price(limit_price), cash_qty(cash_qty) {}
};

struct LimitIfTouchedTradeSpec {
	double qty;
	double limit_price;
	double trigger_price;
	static constexpr OrderType type = OrderType::LimitIfTouched;

	LimitIfTouchedTradeSpec(const double qty, const double limit_price, const double trigger_price) :
		qty(qty), limit_price(limit_price), trigger_price(trigger_price) {}
};

struct StopTradeSpec {
	double qty;
	double stop_price;
	OrderType type;

	StopTradeSpec(const double qty, const double stop_price, const OrderType type = OrderType::Stop) :
		qty(qty), stop_price(stop_price), type(type) {}
};

struct StopLimitTradeSpec {
	double qty;
	double limit_price;
	double stop_price;
	static constexpr OrderType type = OrderType::StopLimit;

	StopLimitTradeSpec(const double qty, const double limit_price, const double stop_price) :
		qty(qty), limit_price(limit_price), stop_price(stop_price) {}
};

struct TrailingStopTradeSpec {
	double qty;
	double trailing_percent;
	double trail_stop_price;
	static constexpr OrderType type = OrderType::TrailingStop;

	TrailingStopTradeSpec(const double qty, const double trailing_percent, const double trail_stop_price) :
		qty(qty), trailing_percent(trailing_percent), trail_stop_price(trail_stop_price) {}
};

struct TrailingStopLimitTradeSpec {
	double qty;
	double lmt_price_offset;
	double trailing_amount;
	double trail_stop_price;
	static constexpr OrderType type = OrderType::TrailingStopLimit;

	TrailingStopLimitTradeSpec(const double qty, const double lmt_price_offset, const double trailing_amount,
	                           const double trail_stop_price) :
		qty(qty), lmt_price_offset(lmt_price_offset), trailing_amount(trailing_amount), trail_stop_price(trail_stop_price) {}
};

struct ComboLimitTradeSpec {
	double qty;
	double limit_price;
	bool non_guaranteed;
	static constexpr OrderType type = OrderType::ComboLimit;

	ComboLimitTradeSpec(const double qty, const double limit_price, const bool non_guaranteed) :
		qty(qty), limit_price(limit_price), non_guaranteed(non_guaranteed) {}
};

struct ComboMarketTradeSpec {
	double qty;
	bool non_guaranteed;
	static constexpr OrderType type = OrderType::ComboMarket;

	ComboMarketTradeSpec(const double qty, const bool non_guaranteed) : qty(qty), non_guaranteed(non_guaranteed) {}
};

struct LimitComboWithLegPricesTradeSpec {
	double qty;
	std::vector<double> leg_prices;
	bool non_guaranteed;
	static constexpr OrderType type = OrderType::LimitComboWithLegPrices;

	LimitComboWithLegPricesTradeSpec(const double qty, std::vector<double> leg_prices, const bool non_guaranteed) :
		qty(qty), leg_prices(std::move(leg_prices)), non_guaranteed(non_guaranteed) {}
};

// Runtime polymorphism but no vtable
using TradeSpec = std::variant<
	MarketTradeSpec,
	LimitTradeSpec,
	BracketOrderSpec,
	MarketIfTouchedTradeSpec,
	MidpriceTradeSpec,
	PeggedToMarketTradeSpec,
	LimitWithCashQtyTradeSpec,
	LimitIfTouchedTradeSpec,
	StopTradeSpec,
	StopLimitTradeSpec,
	TrailingStopTradeSpec,
	TrailingStopLimitTradeSpec,
	ComboLimitTradeSpec,
	ComboMarketTradeSpec,
	LimitComboWithLegPricesTradeSpec>;

struct TradeRequest {
	TradeRequestContext context;
	TradeSpec trade_spec;
};
