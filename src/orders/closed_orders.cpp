#include "config.h"
#include "Execution.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::execDetails(int reqId, const Contract& contract, const Execution& execution) {
	// hub_->send<ExecutionDetailsTag, Execution>(execution);
	LOG_TRACE_TAG(WRAPPER, "Received execution details for order with id {}, execution " "id={} for contract {}.",
	              reqId, execution.orderId, contract.symbol);
}

void ResponseWrapper::execDetailsEnd(int reqId) {
	LOG_DEBUG_TAG(WRAPPER, "Finished receiving execution details. {}", reqId);
}
