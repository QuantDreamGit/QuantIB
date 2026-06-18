#include "quantib/sync/ib.hpp"

void IB::accountSummarySub(
	const std::string &tags,
	const std::string &groups) {

	hub_->subscribe<AccountSummary>([&]() {
		client_->reqAccountSummary(getNextId(), groups, tags);
	});

	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription done.");
}

void IB::accountSummaryCancel() const {
	auto *obj = obj_->try_get<AccountSummary>();
	if (!obj) {
		LOG_WARN_TAG(IB_STR, "Cannot cancel Account Summary subscription: subscription not found.");
		return;
	}
	client_->cancelAccountSummary(obj->reqId);
	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription cancelled.");
}

void ResponseWrapper::accountSummary(const int reqId, const std::string &account,
                                     const std::string &tag, const std::string &value,
                                     const std::string &currency) {
	// Update object
	// Since it's a subscription we know that obj is already created
	const auto new_obj = obj_.get_or_create<AccountSummary>();
	new_obj->reqId = reqId;
	new_obj->attributes[tag] = SummaryAttributes(account, value, currency);
	LOG_TRACE_TAG(IB_STR, "Account Summary update. [Tag: {}] [Value: {}].", tag, value);
	// Send new version
	obj_.update_or_create(new_obj);
}

void ResponseWrapper::accountSummaryEnd(int reqId) {
	LOG_TRACE_TAG(IB_STR, "Account Summary subscription ended. [ReqId: {}].", reqId);
}
