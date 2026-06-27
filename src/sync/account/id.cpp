#include "quantib/core/account.hpp"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::managedAccounts(const std::string& accountsList) {
	// Update object or create it
	auto& account_obj = *obj_.get_or_create<AccountSummary>();
	account_obj.account_id = accountsList;
	LOG_TRACE_TAG(IB_STR, "Account ID update. [Account IDs: {}].", accountsList);
	// Update
	obj_.update_or_create<AccountSummary, AccountSummary>(account_obj);
	hub_.send<AccountIdTag>(account_obj);
}
