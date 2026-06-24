#include "quantib/sync/ib.hpp"

void ResponseWrapper::accountSummary(const int reqId, const std::string& account, const std::string& tag,
                                     const std::string& value, const std::string& currency) {
	// Update object
	// Since it's a subscription we know that obj is already created
	auto* new_obj = obj_.get_or_create<AccountSummaryStoreTag, AccountSummary>();
	new_obj->reqId = reqId;
	new_obj->attributes[tag] = SummaryAttributes(account, value, currency);
	LOG_TRACE_TAG(IB_STR, "Account Summary update. [Tag: {}] [Value: {}].", tag, value);
	// Send new version
	obj_.update_or_create(new_obj);
}

void ResponseWrapper::accountSummaryEnd(int reqId) {
	LOG_TRACE_TAG(IB_STR, "Account Summary subscription ended. [ReqId: {}].", reqId);
}
