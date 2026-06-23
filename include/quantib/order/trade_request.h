#pragma once
#include "Contract.h"
#include "order_types.hpp"
#include "quantib/risk_manager/structs.h"

struct TradeRequestContext
{
    Contract contract;
    std::optional<std::string> reason;
};

struct MarketTradeSpec{
    Side side;
    double qty;
    OrderType type;
};

struct LimitTradeSpec{
    Side side;
    double qty;
    OrderType type;
    double limit_price;
};

struct BracketOrderSpec {
    Side side;
    double quantity;
    double entry_price;
    double take_profit_price;
    double stop_loss_price;
};

// Runtime polymorphism but no vtable
using TradeSpec = std::variant<MarketTradeSpec, LimitTradeSpec, BracketOrderSpec>;
struct TradeRequest {
    TradeRequestContext context;
    TradeSpec trade_spec;
};