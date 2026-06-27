#include "quantib/core/contract_manager.h"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::contractDetails(int reqId, const ContractDetails& contractDetails) {
	if (obj_.contains<ContractDetailsTag>()) obj_.try_append<ContractDetailsTag>(contractDetails);
	// Assign contract to contract manager
	// I'd like to register contracts also coming from sync request
	obj_.insert_or_assign<ContractStoreTag, int, ContractDetails>(contractDetails.contract.conId, contractDetails);
	obj_.insert_or_assign<ContractSymbolStoreTag, std::string, ContractInfo>(
		contractDetails.contract.symbol, ContractInfo(contractDetails.contract.conId, false, contractDetails));
	LOG_TRACE_TAG(WRAPPER, "Received contract details for reqId: {}, symbol: {}, secType: {}, exchange: {}", reqId,
	              contractDetails.contract.symbol, contractDetails.contract.secType, contractDetails.contract.exchange);
}

void ResponseWrapper::contractDetailsEnd(int reqId) {
	if (obj_.contains<ContractDetailsTag>()) {
		const auto contracts = obj_.try_get<ContractDetailsTag, std::vector<ContractDetails>>();
		hub_.send<ContractDetailsTag>(contracts ? std::optional(*contracts) : std::nullopt);
	}

	LOG_DEBUG_TAG(WRAPPER, "Finished receiving contract details for reqId: {}", reqId);
}
