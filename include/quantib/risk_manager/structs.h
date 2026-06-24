#pragma once

#include "retry.h"
#include "quantib/core/contract_manager.h"
#include "quantib/core/data_manager.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/order_manager.h"
#include "quantib/core/position_manager.h"

template <typename RiskManagerT>
struct RiskContext {
	ObjectHub& obj;
	Logger& logger_;
	OrderManager<RiskManagerT>& order;
	DataManager& data;
	PositionManager& position;
	ContractManager& contract;

	RiskContext(ObjectHub& hub_, Logger& logger_, OrderManager<RiskManagerT>& order_, DataManager& data_,
	            PositionManager& position_, ContractManager& contract_)
		: obj(hub_), logger_(logger_), order(order_), data(data_), position(position_), contract(contract_) {}
};

struct OrderIntent {
	Contract contract;
	Order order;

	std::string group_id;
	int leg_id = 0; // For multi-leg orders, identifies the leg number (starting from 0)
	bool required = true;

	std::string strategy_id;
	std::string reason;

	OrderIntent(const Contract& contract_, const Order& order_) : contract(contract_), order(order_) {}
};

struct OrderIntentBatch {
	std::string batch_id;
	std::vector<OrderIntent> orders;

	bool all_or_none = true;
};


enum class RiskDecision {
	Pass,
	Pending,
	Reject
};

enum class RiskAction {
	NoAction,
	Wait,
	RegisterInstrument, // Needed when either contract or market data are missing
	RegisterContract, // Needed when contract details are missing
	RegisterMktData, // Needed when market data are missing
};

struct RiskResult {
	RiskDecision decision;
	std::string reason;
	RiskAction action;
	RetryPolicy retry_policy;

	RiskResult(RiskDecision decision_, std::string reason_ = "", RiskAction action_ = RiskAction::NoAction,
	           RetryPolicy retry_policy_ = RetryPolicy{})
		: decision(decision_), reason(std::move(reason_)), action(action_), retry_policy(retry_policy_) {}

	static RiskResult pass(const std::string& reason = "", RiskAction action = RiskAction::NoAction) {
		return {RiskDecision::Pass, reason, action};
	}

	template <typename Retry>
	static RiskResult pending(const std::string& reason, RiskAction action, Retry) {
		return {RiskDecision::Pending, reason, action, RetryPolicy{Retry::max_attempts, Retry::delayForAttempt(1)}};
	}

	static RiskResult pending(const std::string& reason, RiskAction action = RiskAction::Wait) {
		return {RiskDecision::Pending, reason, action, RetryPolicy{}};
	}

	static RiskResult reject(const std::string& reason, RiskAction action = RiskAction::NoAction) {
		return {RiskDecision::Reject, reason, action};
	}
};
