#include "quantib/core/position_manager.h"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::position(const std::string& account, const Contract& contract, Decimal position, double avgCost) {
	LOG_TRACE_TAG(WRAPPER, "Received position update for account {} and contract {}: position={}, avgCost={}", account,
	              contract.symbol, DecimalFunctions::decimalToString(position), avgCost);
	Position pos{account, contract, position, avgCost};
	obj_.insert_or_assign<PositionStoreTag, int, Position>(contract.conId, pos);
}

void ResponseWrapper::positionEnd() { LOG_TRACE_TAG(WRAPPER, "Finished receiving position updates."); }
