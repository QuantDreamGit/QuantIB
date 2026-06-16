#include "quantib/sync/ib.hpp"

void ResponseWrapper::position(const std::string &account, const Contract &contract, Decimal position, double avgCost) {
	LOG_TRACE_TAG(WRAPPER, "Received position update for account {} and contract {}: position={}, avgCost={}", account,
				  contract.symbol, DecimalFunctions::decimalToString(position), avgCost);
	Position pos{account, contract, position, avgCost};
	obj_->try_append<PositionStoreTag, Position>(std::move(pos));

}

void ResponseWrapper::positionEnd() {
	LOG_TRACE_TAG(WRAPPER, "Finished receiving position updates.");
}