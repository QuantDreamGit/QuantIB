#pragma once

#include <string>
#include <vector>

#include "retry.h"
#include "Contract.h"
#include "Order.h"

enum class RiskDecision {
	Pass,
	Pending,
	Reject
};

enum class RiskAction {
	NoAction,
	Wait,
	RegisterInstrument,
	RegisterContract,
	RegisterMktData,
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
