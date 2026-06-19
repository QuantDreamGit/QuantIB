#pragma once

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

	[[nodiscard]] bool validate(const Order &order, const Contract &contract) const {
		return (RiskPolicies::check(order, contract, ctx_) && ...);
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
};
