#pragma once

#include <deque>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

#include "nameof.hpp"

#include "Contract.h"
#include "quantib/order/trade_request.h"
#include "structs.h"

#include "quantib/utils/logger.hpp"
#include "quantib/risk_manager/policies.h"

class EClientSocket;
class ObjectHub;
class BlockingHub;

template <typename Derived, typename... RiskPolicies>
class RiskManager {
public:
	using Self = Derived;

	RiskManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger, PositionManager& position,
	            OrderManager<Self>& order, DataManager& data, ContractManager& contract)
		: logger_(logger), client_(client), obj_(obj), hub_(hub), position_(position), order_(order), data_(data),
		  contract_(contract) {}

	~RiskManager() { stop(); }

	void start() {
		if (riskRunning_) return;
		riskRunning_ = true;
		riskThread_ = std::thread(&RiskManager::workerLoop, this);
	}

	void stop() {
		riskRunning_ = false;

		if (riskThread_.joinable()) { riskThread_.join(); }
	}

	[[nodiscard]] std::future<RiskResult> validate(const TradeRequest& request) {
		std::promise<RiskResult> promise;
		auto future = promise.get_future();

		{
			std::lock_guard<std::mutex> lock(validation_mutex_);
			validation_queue_.push_back({request, std::move(promise)});
		}

		return future;
	}

	void executeAction(const RiskResult& result, const Contract& contract) const {
		switch (result.action) {
		case RiskAction::NoAction:
			break;

		case RiskAction::Wait: LOG_TRACE_TAG(RISK, "Waiting for contract: {}. Reason: {}", contract.symbol,
		                                     result.reason);
			break;

		case RiskAction::RegisterInstrument:
			contract_.registerContract(contract);
			data_.marketDataSub(contract);
			break;

		case RiskAction::RegisterContract:
			contract_.registerContract(contract);
			break;

		case RiskAction::RegisterMktData:
			data_.marketDataSub(contract);
			break;
		}
	}

private:
	struct ValidationRequest {
		TradeRequest request;
		std::promise<RiskResult> promise;
	};

	void workerLoop() {
		while (riskRunning_) {
			std::deque<ValidationRequest> local_queue;

			{
				std::lock_guard<std::mutex> lock(validation_mutex_);
				local_queue.swap(validation_queue_);
			}

			while (!local_queue.empty()) {
				auto request = std::move(local_queue.front());
				local_queue.pop_front();

				try {
					RiskResult result = validateImpl<RiskPolicies...>(request.request);

					request.promise.set_value(result);
				}
				catch (...) { request.promise.set_exception(std::current_exception()); }
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	template <typename Policy, typename... OtherPolicies>
	[[nodiscard]] RiskResult validateImpl(TradeRequest& request, int attempt = 1) const {
		RiskResult res = Policy::check(request, ctx_);
		Contract &contract = request.context.contract;

		if (res.decision == RiskDecision::Reject) {
			LOG_WARN_TAG(RISK, "Policy {} failed for contract: {}. Action: {}", NAMEOF_TYPE(Policy),
			             contract.symbol, NAMEOF_ENUM(res.action));
			return res;
		}

		if (res.decision == RiskDecision::Pending) {
			LOG_WARN_TAG(RISK, "Policy {} pending for contract: {}. Attempt {}/{}. Action: {}",
			             NAMEOF_TYPE(Policy), contract.symbol, attempt, res.retry_policy.max_attempts,
			             NAMEOF_ENUM(res.action));

			executeAction(res, contract);

			if (res.retry_policy.max_attempts <= 0 || attempt >= res.retry_policy.max_attempts) {
				res.decision = RiskDecision::Reject;
				LOG_WARN_TAG(
					RISK, "Max attempts reached for policy {} on contract: {}. Rejecting order. "
					"Action: {}", NAMEOF_TYPE(Policy), contract.symbol, NAMEOF_ENUM(res.action));
				return res;
			}

			std::this_thread::sleep_for(res.retry_policy.retry_delay);

			return validateImpl<Policy, OtherPolicies...>(request, attempt + 1);
		}

		if constexpr (sizeof...(OtherPolicies) > 0) { return validateImpl<OtherPolicies...>(request, 1); }
		else {
			LOG_TRACE_TAG(RISK, "All policies passed for contract: {}. Action: {}", contract.symbol, NAMEOF_ENUM(res.action));
			return RiskResult::pass();
		}
	}

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;
	PositionManager& position_;
	OrderManager<Self>& order_;
	DataManager& data_;
	ContractManager& contract_;

	RiskContext<Self> ctx_{obj_, logger_, order_, data_, position_, contract_};
	std::deque<ValidationRequest> validation_queue_;
	std::mutex validation_mutex_;
	std::thread riskThread_;
	std::atomic<bool> riskRunning_{false};
};
