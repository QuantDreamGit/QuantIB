#pragma once

#include "retry.h"

class ObjectHub;
class Logger;
class DataManager;
class PositionManager;
class ContractManager;
template <typename RiskManagerT>
class OrderManager;

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