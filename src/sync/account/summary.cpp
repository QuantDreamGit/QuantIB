#include "quantib/sync/ib.hpp"

void IB::accountSummarySub (
  const std::string &tags,
  const std::string &groups){
  	hub_->subscribe<AccountSummary>(typeid(accountSummaryTag), [&]() {
		  client_->reqAccountSummary(nextId_++, groups, tags);
	  });
  }

void ResponseWrapper::accountSummary(int reqId, const std::string &account,
  const std::string &tag, const std::string &value,
  const std::string &currency) {
	// Update object
	auto new_obj = obj_->get<AccountSummary>(typeid(accountSummaryTag));
	new_obj.account = account;
	new_obj.currency = currency;
	new_obj.attributes[tag] = value;
	// Send new version
	hub_->send<accountSummaryTag>(new_obj);
}
