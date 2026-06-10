#include "quantib/sync/ib.hpp"

std::optional<std::vector<Contract>>
IB::getContracts(int reqId, const Contract& contract) const {
	// Get contracts by requesting contract details and then extracting contracts from details

	if (auto contract_details = IB::getContractDetails(reqId, contract); contract_details.has_value()) {
		std::vector<Contract> contracts;
		for (auto elem : contract_details.value()) {
			contracts.push_back(elem.contract);
		}

		return contracts;
	}

	LOG_WARN_TAG(IB_STR, "No contract details found for reqId: {}, symbol: {}, secType: {}, exchange: {}",
			  reqId, contract.symbol, contract.secType, contract.exchange);
	return std::nullopt;
}
