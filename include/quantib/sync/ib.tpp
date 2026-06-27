#pragma once

template <typename ProfileT>
void IB<ProfileT>::accountSummarySub(const std::string& tags, const std::string& groups) {
	hub_->subscribe<AccountSummaryStoreTag, AccountSummary>([&]() {
		client_->reqAccountSummary(getNextId(), groups, tags);
	});

	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription done.");
}

template <typename ProfileT>
void IB<ProfileT>::accountSummaryCancel() const {
	auto* obj = obj_->try_get<AccountSummary>();
	if (!obj) {
		LOG_WARN_TAG(IB_STR, "Cannot cancel Account Summary subscription: subscription not found.");
		return;
	}
	client_->cancelAccountSummary(obj->reqId);
	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription cancelled.");
}

template <typename ProfileT>
void IB<ProfileT>::accountUpdateSub() const {
	// We have to first request account id
	// TODO: Now we suppose only one account id, but this should be generalized!
	auto account_summary = IB::getAccountIds();
	hub_->subscribe<AccountSummary>([&]() {
		if (account_summary.has_value()) client_->reqAccountUpdates(true, account_summary->account_id);
		else
			LOG_CRITICAL_TAG(IB_STR, "Account Id not found during AccountUpdatedCancel.");
	});

	LOG_DEBUG_TAG(IB_STR, "Account Updates subscription done.");
}

template <typename ProfileT>
void IB<ProfileT>::accountUpdateCancel() const {
	const auto account_summary = obj_->try_get<AccountSummary>();
	hub_->subscribe<AccountSummary>([&]() {
		// We have to first request account id
		// TODO: Now we suppose only one account id, but this should be generalized!
		if (account_summary && !account_summary->account_id.empty()) client_->reqAccountUpdates(
			false, account_summary->account_id);
		else
			LOG_CRITICAL_TAG(IB_STR, "Account Id not found during AccountUpdatedCancel.");
	});

	LOG_DEBUG_TAG(IB_STR, "Account Updates subscription cancelled.");
}

template <typename ProfileT>
auto IB<ProfileT>::getAccountIds() const -> std::optional<AccountSummary> {
	return hub_->wait_for<AccountIdTag, AccountSummary>([&]() { client_->reqManagedAccts(); });

	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription done.");
}

template <typename ProfileT>
std::optional<std::vector<Contract>> IB<ProfileT>::getContractsSync(int reqId, const Contract& contract) const {
	// Get contracts by requesting contract details and then extracting contracts from details

	if (auto contract_details = IB::getContractDetailsSync(reqId, contract); contract_details.has_value()) {
		std::vector<Contract> contracts;
		for (auto elem : contract_details.value()) { contracts.push_back(elem.contract); }

		return contracts;
	}

	LOG_WARN_TAG(IB_STR, "No contract details found for reqId: {}, symbol: {}, secType: {}, exchange: {}", reqId,
	             contract.symbol, contract.secType, contract.exchange);
	return std::nullopt;
}

template <typename ProfileT>
std::optional<Contract> IB<ProfileT>::getContract(const Contract& contract) const {
	if (contract.conId != 0) return contract_->getContract(contract.conId);
	if (contract.symbol != "") return contract_->getContract(contract.symbol);
	return std::nullopt;
}

template <typename ProfileT>
std::optional<Contract> IB<ProfileT>::getContract(const std::string& symbol) const {
	return contract_->getContract(symbol);
}

template <typename ProfileT>
void IB<ProfileT>::registerContract(const Contract& contract) const { contract_->registerContract(contract); }

template <typename ProfileT>
std::optional<std::vector<ContractDetails>>
IB<ProfileT>::getContractDetailsSync(int reqId, const Contract& contract) const {
	auto result = hub_->wait_for<ContractDetailsTag, std::vector<ContractDetails>>([&]() {
		client_->reqContractDetails(reqId, contract);
	});

	// Clean object hub for next request
	obj_->erase<ContractDetailsTag>();
	LOG_DEBUG_TAG(IB_STR, "Contract details request done.");
	return result;
}

template <typename ProfileT>
std::optional<ContractDetails> IB<ProfileT>::getContractDetails(const Contract& contract) {
	if (contract.conId != 0) return contract_->getContractDetails(contract.conId);
	if (contract.symbol != "") return contract_->getContractDetails(contract.symbol);
	return std::nullopt;
}

template <typename ProfileT>
std::optional<ContractDetails> IB<ProfileT>::getContractDetails(const std::string& symbol) const {
	return contract_->getContractDetails(symbol);
}

template <typename ProfileT>
std::optional<std::unordered_map<int, OpenOrders>*> IB<ProfileT>::getOpenOrders() const {
	LOG_DEBUG_TAG(IB_STR, "Requesting open orders.");
	return hub_->wait_for<OpenOrdersTag, std::unordered_map<int, OpenOrders>*>([&]() { client_->reqAllOpenOrders(); });
}

template <typename ProfileT>
std::optional<std::unordered_map<int, ClosedOrders>*>
IB<ProfileT>::getClosedOrders() const {
	LOG_DEBUG_TAG(IB_STR, "Requesting closed orders.");
	auto* value = *obj_->try_get<ClosedOrderStoreTag, std::unordered_map<int, ClosedOrders>*>();
	return value ? std::optional<std::unordered_map<int, ClosedOrders>*>(value) : std::nullopt;
}
