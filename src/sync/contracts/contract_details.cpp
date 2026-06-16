#include "quantib/sync/ib.hpp"

std::optional<std::vector<ContractDetails> >
IB::getContractDetails(int reqId, const Contract &contract) const {
	auto result = hub_->wait_for<ContractDetailsTag, std::vector<ContractDetails> >([&]() {
		client_->reqContractDetails(reqId, contract);
	});

	// Clean object hub for next request
	obj_->erase<ContractDetailsTag>();
	LOG_DEBUG_TAG(IB_STR, "Contract details request done.");
	return result;
}

void ResponseWrapper::contractDetails(int reqId, const ContractDetails &contractDetails) {
	obj_.try_append<ContractDetailsTag>(contractDetails);
	LOG_TRACE_TAG(WRAPPER, "Received contract details for reqId: {}, symbol: {}, secType: {}, exchange: {}",
	              reqId, contractDetails.contract.symbol, contractDetails.contract.secType,
	              contractDetails.contract.exchange);
}

void ResponseWrapper::contractDetailsEnd(int reqId) {
	auto *contracts =
			obj_.try_get<ContractDetailsTag, std::vector<ContractDetails> >();

	if (!contracts) {
		LOG_WARN_TAG(WRAPPER, "Contract details end received for reqId {}, but no contract details were stored.",
		             reqId);

		hub_.send<ContractDetailsTag>(std::vector<ContractDetails>{});
		return;
	}

	hub_.send<ContractDetailsTag>(*contracts);
	LOG_DEBUG_TAG(WRAPPER, "Finished receiving contract details for reqId: {}", reqId);
}
