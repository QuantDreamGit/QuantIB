#pragma once
#include <vector>

#include "Order.h"
#include "trade_request.h"
#include "config.h"
#include "quantib/risk_manager/risk_manager.h"
#include "quantib/risk_manager/risk_manager.h"

enum class OrderBatchType {
    SingleOrder,
    LimitOrder,
    BracketOrder
};

struct OrderBatch {
    OrderBatchType batch_type;
    std::vector<Order> orders;

    bool isEmpty() const { return orders.empty(); }
    std::size_t size() const { return orders.size(); }
};

class OrderFactory {
public:
    OrderFactory(RiskManager &riskManager)
    OrderBatch make(const TradeRequest& request) const;
private:
    MyRiskManager risk_manager_;
};
