#pragma once
#include <vector>

#include "Order.h"
#include "trade_request.h"

enum class OrderBatchType { SingleOrder, LimitOrder, BracketOrder };

struct OrderBatch {
	OrderBatchType batch_type;
	std::vector<Order> orders;

	bool isEmpty() const { return orders.empty(); }
	std::size_t size() const { return orders.size(); }
};

template <typename RiskManagerT>
class OrderFactory {
public:
	using RiskManager = RiskManagerT;

	explicit OrderFactory(Logger& logger) : logger_(logger) {}

	void setRiskManager(RiskManager& risk) { risk_ = &risk; }

	[[nodiscard]] OrderBatch make(const TradeRequest& request) const;

private:
	Logger& logger_;
	RiskManager* risk_ = nullptr;
};
