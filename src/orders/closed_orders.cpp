#include "quantib/sync/ib.hpp"

void ResponseWrapper::execDetails(int reqId, const Contract &contract, const Execution &execution) {
	// hub_->send<ExecutionDetailsTag, Execution>(execution);
	LOG_TRACE_TAG(WRAPPER, "Received execution details for order with id {} for contract {}.", execution.orderId, contract.symbol);
}

void ResponseWrapper::execDetailsEnd(int reqId) {
	LOG_DEBUG_TAG(WRAPPER, "Finished receiving execution details. {}", reqId);
}

std::optional<std::vector<ClosedOrders>> IB::getClosedOrders() const {
	LOG_DEBUG_TAG(IB_STR, "Requesting closed orders.");
	return orders_->getClosedOrders();
}
