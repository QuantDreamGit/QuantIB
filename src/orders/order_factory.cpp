#include "quantib/order/order_factory.h"

OrderBatch OrderFactory::make(const TradeRequest& request) const {
    auto future = risk_->validate(OrderIntent(contract, order));
    const RiskResult result = future.get();

    if (result.decision == RiskDecision::Reject) {
        LOG_WARN_TAG(RISK, "Order rejected for {}. Reason: {}", contract.symbol, result.reason);
        return;
    }

    if (result.decision == RiskDecision::Pending) {
        LOG_WARN_TAG(RISK, "Order still pending for {}. Reason: {}", contract.symbol, result.reason);
        return;
    }
}
