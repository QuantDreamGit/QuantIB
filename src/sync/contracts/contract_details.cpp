#include "quantib/sync/ib.hpp"

std::optional<std::vector<ContractDetails>>
IB::getContractDetails(int reqId, const Contract& contract) const {
	auto result = hub_->wait_for<ContractDetailsTag, std::vector<ContractDetails>>([&]() {
		client_->reqContractDetails(reqId, contract);
	});

	// Clean object hub for next request
	obj_->update<ContractDetailsTag, std::vector<ContractDetails>>(std::vector<ContractDetails>());

	LOG_DEBUG_TAG(IB_STR, "Contract details request done.");

	return result;
}

void ResponseWrapper::contractDetails(int reqId, const ContractDetails &contractDetails) {
	auto received_contracts = obj_->get_or_default<ContractDetailsTag, std::vector<ContractDetails>>();
	received_contracts.push_back(contractDetails);
	obj_->update<ContractDetailsTag, std::vector<ContractDetails>>(received_contracts);
	LOG_TRACE_TAG(WRAPPER, "Received contract details for reqId: {}, symbol: {}, secType: {}, exchange: {}",
			  reqId, contractDetails.contract.symbol, contractDetails.contract.secType, contractDetails.contract.exchange);
}

void ResponseWrapper::contractDetailsEnd(int reqId) {
	hub_->send<ContractDetailsTag>(obj_->get<ContractDetailsTag, std::vector<ContractDetails>>());
	LOG_DEBUG_TAG(WRAPPER, "Finished receiving contract details for reqId: {}", reqId);
}