#include "quantib/sync/ib.hpp"

void IB::accountSummarySub (
  const std::string &tags,
  const std::string &groups){
  	hub_->subscribe<AccountSummaryTag, AccountSummary>([&]() {
		  client_->reqAccountSummary(nextId_++, groups, tags);
	  });
  }

void ResponseWrapper::accountSummary(int reqId, const std::string &account,
  const std::string &tag, const std::string &value,
  const std::string &currency) {
	// Update object
	auto new_obj = obj_->get<AccountSummaryTag, AccountSummary>();
	new_obj.account = account;
	new_obj.currency = currency;
	new_obj.attributes[tag] = value;
	// Send new version
	hub_->send<AccountSummaryTag>(new_obj);
}
