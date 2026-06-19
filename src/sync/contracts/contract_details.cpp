#include "quantib/sync/ib.hpp"

std::optional<std::vector<ContractDetails> >
IB::getContractDetailsSync(int reqId, const Contract &contract) const {
	auto result = hub_->wait_for<ContractDetailsTag, std::vector<ContractDetails> >([&]() {
		client_->reqContractDetails(reqId, contract);
	});

	// Clean object hub for next request
	obj_->erase<ContractDetailsTag>();
	LOG_DEBUG_TAG(IB_STR, "Contract details request done.");
	return result;
}

std::optional<ContractDetails> IB::getContractDetails(const Contract &contract) const {
	if (contract.conId != 0) return contract_->getContractDetails(contract.conId);
	if (contract.symbol != "") return contract_->getContractDetails(contract.symbol);
	return std::nullopt;
}

std::optional<ContractDetails> IB::getContractDetails(const std::string &symbol) const {
	return contract_->getContractDetails(symbol);
}

void ResponseWrapper::contractDetails(int reqId, const ContractDetails &contractDetails) {
	if (obj_.contains<ContractDetailsTag>()) obj_.try_append<ContractDetailsTag>(contractDetails);
	// Assign contract to contract manager
	// I'd like to register contracts also coming from sync request
	obj_.insert_or_assign<ContractStoreTag, int, ContractDetails>(contractDetails.contract.conId, contractDetails);
	obj_.insert_or_assign<ContractSymbolStoreTag, std::string, ContractInfo>(
		contractDetails.contract.symbol,
		ContractInfo(contractDetails.contract.conId, false, contractDetails));
	LOG_TRACE_TAG(WRAPPER, "Received contract details for reqId: {}, symbol: {}, secType: {}, exchange: {}",
	              reqId, contractDetails.contract.symbol, contractDetails.contract.secType,
	              contractDetails.contract.exchange);
}

void ResponseWrapper::contractDetailsEnd(int reqId) {
	if (obj_.contains<ContractDetailsTag>()) {
		const auto contracts = obj_.try_get<ContractDetailsTag, std::vector<ContractDetails> >();
		hub_.send<ContractDetailsTag>(contracts ? std::optional(*contracts) : std::nullopt);
	}

	LOG_DEBUG_TAG(WRAPPER, "Finished receiving contract details for reqId: {}", reqId);
}
