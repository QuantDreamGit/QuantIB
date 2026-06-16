#pragma once
#include <string>

#include "Contract.h"

struct Position {
	std::string account;
	Contract contract;
	Decimal pos;
	double avgCost{};

	Position() = default;

	explicit Position(std::string account, const Contract& contract, const Decimal pos,
	                  const double avgCost) : account(std::move(account)),
	                                    contract(contract),
	                                    pos(pos), avgCost(avgCost) {
	}
};
