#include "quantib/sync/ib.hpp"

void IB::accountUpdateSub() const {
	// We have to first request account id
	// TODO: Now we suppose only one account id, but this should be generalized!
	auto account_summary = IB::getAccountIds();
	hub_->subscribe<AccountSummary>([&]() {
		if (account_summary.has_value()) client_->reqAccountUpdates(true, account_summary->account_id);
		else LOG_CRITICAL_TAG(IB_STR, "Account Id not found during AccountUpdatedCancel.");
	});

	LOG_DEBUG_TAG(IB_STR, "Account Updates subscription done.");
}

void IB::accountUpdateCancel() const {
	const auto account_summary = obj_->get<AccountSummary>();
	hub_->subscribe<AccountSummary>([&]() {
		// We have to first request account id
		// TODO: Now we suppose only one account id, but this should be generalized!
		if (account_summary.account_id != "") client_->reqAccountUpdates(false, account_summary.account_id);
		else LOG_CRITICAL_TAG(IB_STR, "Account Id not found during AccountUpdatedCancel.");
	});

	LOG_DEBUG_TAG(IB_STR, "Account Updates subscription cancelled.");
}

void ResponseWrapper::updateAccountValue(const std::string &key, const std::string &val, const std::string &currency,
                                         const std::string &accountName) {
	auto new_obj = obj_->get<AccountSummary>();
	new_obj.update_attributes[key] = UpdateAttribute(key, val, currency, accountName);
	LOG_TRACE_TAG(IB_STR, "Account Summary update. [Key: {}, Value: {}, Currency: {}, Account Name: {}].", key, val,
	              currency, accountName);
	// Update
	obj_->update(new_obj);
}

void ResponseWrapper::updatePortfolio(const Contract &contract, const Decimal position, const double marketPrice,
                                      const double marketValue, const double averageCost, const double unrealizedPNL,
                                      const double realizedPNL, const std::string &accountName) {
	auto new_obj = obj_->get<AccountSummary>();
	// We suppose that the contract symbol is unique, but this should be generalized!

	// Linear scan of currently open positions
	for (auto it = new_obj.portfolio.begin(), end = new_obj.portfolio.end(); it != end; ++it) {
		if (it->contract.symbol == contract.symbol) {
			it->position = position;
			it->market_price = marketPrice;
			it->market_value = marketValue;
			it->avg_cost = averageCost;
			it->unrealized_pnl = unrealizedPNL;
			it->realized_pnl = realizedPNL;
			LOG_TRACE_TAG(
				IB_STR,
				"Position updated in Account Summary. [Contract: {}, Position: {}, Market Price: {}, Market Value: "
				"{}, Average Cost: {}, Unrealized PNL: {}, Realized PNL: {}, Account Name: {}].",
				contract.symbol, DecimalFunctions::decimalToString(position), marketPrice, marketValue,
				averageCost, unrealizedPNL,
				realizedPNL,
				accountName);
			 // Send new version
			hub_->send<AccountSummary, AccountSummary>(new_obj);
			return;
		}
	}

	// Otherwise insert new position
	new_obj.portfolio.emplace_back(contract, position, marketPrice, marketValue, averageCost, unrealizedPNL, realizedPNL);

	LOG_TRACE_TAG(
		IB_STR, "New position added to Account Summary. [Contract: {}, Position: {}, Market Price: {}, Market Value: "
				"{}, Average Cost: {}, Unrealized PNL: {}, Realized PNL: {}, Account Name: {}].",
		contract.symbol, DecimalFunctions::decimalToString(position), marketPrice, marketValue,
		averageCost, unrealizedPNL,
		realizedPNL,
		accountName);
	// Update
	obj_->update(new_obj);
}

void ResponseWrapper::updateAccountTime(const std::string& timeStamp) {
	auto new_obj = obj_->get<AccountSummary>();
	new_obj.last_update_time = timeStamp;
	LOG_TRACE_TAG(IB_STR, "Account Summary update. [Last Update Time: {}].", timeStamp);

	// Update
	obj_->update(new_obj);
}

void ResponseWrapper::accountDownloadEnd(const std::string &accountName) {
	LOG_DEBUG_TAG(IB_STR, "Account update finished. [Account Name: {}].", accountName);
}
