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