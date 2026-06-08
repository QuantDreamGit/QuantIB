#pragma once
#include <string>
#include <unordered_map>

#define ACCOUNT_SUMMARY_TAG_ACCOUNT_TYPE "AccountType"
#define ACCOUNT_SUMMARY_TAG_NET_LIQUIDATION "NetLiquidation"
#define ACCOUNT_SUMMARY_TAG_TOTAL_CASH_VALUE "TotalCashValue"
#define ACCOUNT_SUMMARY_TAG_BUYING_POWER "BuyingPower"
#define ACCOUNT_SUMMARY_TAG_GROSS_POSITION_VALUE "GrossPositionValue"
#define ACCOUNT_SUMMARY_TAG_INIT_MARGIN_REQ "InitMarginReq"
#define ACCOUNT_SUMMARY_TAG_MAINT_MARGIN_REQ "MaintMarginReq"
#define ACCOUNT_SUMMARY_TAG_AVAILABLE_FUNDS "AvailableFunds"
#define ACCOUNT_SUMMARY_TAG_EXCESS_LIQUIDITY "ExcessLiquidity"
#define ACCOUNT_SUMMARY_TAG_CUSHION "Cushion"
#define ACCOUNT_SUMMARY_TAG_LEVERAGE "Leverage"

#define ACCOUNT_SUMMARY_ESSENTIAL_TAGS                                         \
  "AccountType,"                                                               \
  "NetLiquidation,"                                                            \
  "TotalCashValue,"                                                            \
  "BuyingPower,"                                                               \
  "GrossPositionValue,"                                                        \
  "InitMarginReq,"                                                             \
  "MaintMarginReq,"                                                            \
  "AvailableFunds,"                                                            \
  "ExcessLiquidity,"                                                           \
  "Cushion,"                                                                   \
  "Leverage"

class AccountSummary {
public:
	int reqId;
	std::string account;
	std::unordered_map<std::string, std::string> attributes;
	std::string currency;

	AccountSummary() = default;

	void update(const std::string &tag, const std::string &value) {
		attributes[tag] = value;
	}
};
