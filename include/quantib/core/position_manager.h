#pragma once
#include <unordered_map>
#include <string>
#include <utility>

#include "Contract.h"

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

struct Position {
	std::string account;
	Contract contract;
	Decimal pos;
	double avgCost{};

	Position() = default;

	explicit Position(std::string account_, const Contract& contract_, const Decimal pos_, const double avgCost_)
		: account(std::move(account_)), contract(contract_), pos(pos_), avgCost(avgCost_) {}
};

class PositionManager {
public:
	PositionManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger);

	[[nodiscard]] const std::unordered_map<int, Position>* getPositions() const;

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::unordered_map<int, Position>* positions_;
};
