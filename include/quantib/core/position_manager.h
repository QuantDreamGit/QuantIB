#pragma once
#include <string>

#include "Contract.h"
#include "EClientSocket.h"
#include "object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

struct Position {
	std::string account;
	Contract contract;
	Decimal pos;
	double avgCost{};

	Position() = default;

	explicit Position(std::string account_, const Contract &contract_, const Decimal pos_,
	                  const double avgCost_) : account(std::move(account_)),
	                                          contract(contract_),
	                                          pos(pos_), avgCost(avgCost_) {
	}
};

class PositionManager {
public:
	PositionManager(EClientSocket &client, BlockingHub &hub, ObjectHub &obj, Logger &logger) :
		logger_(logger), client_(client), obj_(obj), hub_(hub) {

		// Create the subscription
		hub_.subscribe<PositionStoreTag, std::vector<Position> >([&]() {
			client_.reqPositions();
		});
		// Get the pointer to the position store
		positions_ = obj_.get_or_create<PositionStoreTag,std::vector<Position> >();
	}

	[[nodiscard]] const std::vector<Position> *getPositions() const {
		return positions_;
	}

private:
	Logger &logger_;
	EClientSocket &client_;
	ObjectHub &obj_;
	BlockingHub &hub_;

	std::vector<Position> *positions_;
};
