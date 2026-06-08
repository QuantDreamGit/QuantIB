#include "quantib/sync/ib.hpp"

void IB::accountSummarySub(
	const std::string &tags,
	const std::string &groups) {

	hub_->subscribe<AccountSummaryTag, AccountSummary>([&]() {
		client_->reqAccountSummary(nextId_++, groups, tags);
	});

	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription done.");
}

void IB::accountSummaryCancel() const {
	const int reqId = obj_->get<AccountSummaryTag, AccountSummary>().reqId;
	client_->cancelAccountSummary(reqId);
	LOG_DEBUG_TAG(IB_STR, "Account Summary subscription cancelled.");
}

void ResponseWrapper::accountSummary(int reqId, const std::string &account,
                                     const std::string &tag, const std::string &value,
                                     const std::string &currency) {
	// Update object
	auto new_obj = obj_->get<AccountSummaryTag, AccountSummary>();
	new_obj.reqId = reqId;
	new_obj.account = account;
	new_obj.currency = currency;
	new_obj.attributes[tag] = value;
	LOG_TRACE_TAG(IB_STR, "Account Summary update. Tag: {} Value: {}.", tag, value);
	// Send new version
	hub_->send<AccountSummaryTag>(new_obj);
}

void ResponseWrapper::accountSummaryEnd(int reqId) {
	LOG_TRACE_TAG(IB_STR, "Account Summary update complete.");
}
