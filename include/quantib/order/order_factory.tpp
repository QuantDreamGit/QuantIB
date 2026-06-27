#pragma once
#include "quantib/concepts/risk_manager_concepts.hpp"

template <RiskManagerConcept RiskManagerT>
OrderBatch OrderFactory<RiskManagerT>::make(const TradeRequest& request) const {
	// First we have to check if order respect RiskManager
	if (check(request)) {
		const int ids_needed = std::visit([](const auto& spec) {
			return order_count(spec);
		}, request.trade_spec);
		const int order_id = getNextId();
		for (int i = 1; i < ids_needed; ++i) {
			(void)getNextId();
		}

		auto batch = std::visit([&](const auto& spec) {
			return build_order(request, spec, order_id);
		}, request.trade_spec);

		return batch;
	}
	return OrderBatch{};
}

template <RiskManagerConcept RiskManagerT>
bool OrderFactory<RiskManagerT>::check(const TradeRequest& request) const {
	// First we have to check if order respect RiskManager
	if (risk_ == nullptr) { return true; }

	const RiskResult result = risk_->validate(request).get();
	return result.decision == RiskDecision::Pass;
}