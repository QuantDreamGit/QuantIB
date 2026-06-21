#pragma once

#include <deque>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

#include "nameof.hpp"

#include "Contract.h"
#include "EClientSocket.h"

#include "../core/object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"
#include "quantib/risk_manager/policies.h"

template<typename... RiskPolicies>
class RiskManager {
public:
	RiskManager(EClientSocket &client, BlockingHub &hub, ObjectHub &obj, Logger &logger, PositionManager &position,
	            OrderManager &order, DataManager &data, ContractManager &contract)
		: logger_(logger), client_(client), obj_(obj), hub_(hub), position_(position), order_(order), data_(data),
		  contract_(contract) {
	}

	~RiskManager() {
		stop();
	}

	void start() {
		if (riskRunning_) return;
		riskRunning_ = true;
		riskThread_ = std::thread(&RiskManager::workerLoop, this);
	}

	void stop() {
		riskRunning_ = false;

		if (riskThread_.joinable()) {
			riskThread_.join();
		}
	}

	[[nodiscard]] std::future<RiskResult> validate(const OrderIntent& order_intent) {
		std::promise<RiskResult> promise;
		auto future = promise.get_future();

		{
			std::lock_guard<std::mutex> lock(validation_mutex_);
			validation_queue_.push_back({
				order_intent,
				std::move(promise)
			});
		}

		return future;
	}

	void executeAction(const RiskResult& result, const Contract& contract) const {
		switch (result.action) {
			case RiskAction::NoAction:
				break;

			case RiskAction::Wait:
				LOG_TRACE_TAG(RISK, "Waiting for contract: {}. Reason: {}",
							  contract.symbol, result.reason);
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
		OrderIntent order_intent;
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
					RiskResult result = validateImpl<RiskPolicies...>(request.order_intent);

					request.promise.set_value(result);
				} catch (...) {
					request.promise.set_exception(std::current_exception());
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	template<typename Policy, typename... OtherPolicies>
	[[nodiscard]] RiskResult validateImpl(const OrderIntent& order_intent,
							int attempt = 1) const {
		const Order& order = order_intent.order;
		const Contract& contract = order_intent.contract;

		RiskResult res = Policy::check(order, contract, ctx_);

		if (res.decision == RiskDecision::Reject) {
			LOG_WARN_TAG(RISK, "Policy {} failed for order: {}, contract: {}. Action: {}",
				NAMEOF_TYPE(Policy), order.action, contract.symbol, NAMEOF_ENUM(res.action));
			return res;
		}

		if (res.decision == RiskDecision::Pending) {
			LOG_WARN_TAG(RISK, "Policy {} pending for order: {}, contract: {}. Attempt {}/{}. Action: {}",
				NAMEOF_TYPE(Policy),
				order.action,
				contract.symbol,
				attempt,
				res.retry_policy.max_attempts,
				NAMEOF_ENUM(res.action));

			executeAction(res, contract);

			if (res.retry_policy.max_attempts <= 0 ||
				attempt >= res.retry_policy.max_attempts) {
				res.decision = RiskDecision::Reject;
				LOG_WARN_TAG(RISK, "Max attempts reached for policy {} on order: {}, contract: {}. Rejecting order. "
									"Action: {}",
					NAMEOF_TYPE(Policy), order.action, contract.symbol, NAMEOF_ENUM(res.action));
				return res;
				}

			std::this_thread::sleep_for(res.retry_policy.retry_delay);

			return validateImpl<Policy, OtherPolicies...>(
				order_intent,
				attempt + 1
			);
		}

		if constexpr (sizeof...(OtherPolicies) > 0) {
			return validateImpl<OtherPolicies...>(order_intent, 1);
		} else {
			LOG_TRACE_TAG(RISK, "All policies passed for order: {}, contract: {}. Action: {}",
				order.action, contract.symbol, NAMEOF_ENUM(res.action));
			return RiskResult::pass();
		}
	}
private:
	Logger &logger_;
	EClientSocket &client_;
	ObjectHub &obj_;
	BlockingHub &hub_;
	PositionManager &position_;
	OrderManager &order_;
	DataManager &data_;
	ContractManager &contract_;

	RiskContext ctx_{obj_, logger_, order_, data_, position_, contract_};
	std::deque<ValidationRequest> validation_queue_;
	std::vector<OrderIntentBatch> order_intent_batch_vec_;
	std::mutex validation_mutex_;
	std::thread riskThread_;
	std::atomic<bool> riskRunning_{false};
};
