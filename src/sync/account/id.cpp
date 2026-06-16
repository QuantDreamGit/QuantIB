#include "quantib/sync/ib.hpp"

auto IB::getAccountIds() const -> std::optional<AccountSummary> {
	return hub_->wait_for<AccountIdTag, AccountSummary>([&]() {
		client_->reqManagedAccts();
	});

	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription done.");
}

void ResponseWrapper::managedAccounts( const std::string& accountsList) {
	// Update object or create it
	auto &account_obj = *obj_.get_or_create<AccountSummary>();
	account_obj.account_id = accountsList;
	LOG_TRACE_TAG(IB_STR, "Account ID update. [Account IDs: {}].", accountsList);
	// Update
	obj_.update_or_create<AccountSummary, AccountSummary>(account_obj);
	hub_.send<AccountIdTag>(account_obj);
}
