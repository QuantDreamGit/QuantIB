#pragma once
#include <string>
#include <unordered_map>
#include <string_view>

#include "Contract.h"
#include "Decimal.h"

struct SummaryAttributes {
	SummaryAttributes() = default;
	SummaryAttributes(const std::string &account_id_, const std::string &value_, const std::string &currency_)
		: account_id(account_id_), value(value_), currency(currency_) {}

	std::string account_id;
	std::string value;
	std::string currency;
};

struct UpdateAttribute {
	UpdateAttribute() = default;
	UpdateAttribute(const std::string &key_, const std::string &value_, const std::string &currency_, const std::string &account_name_)
		: key(key_), value(value_), currency(currency_), account_name(account_name_) {}

	std::string key;
	std::string value;
	std::string currency;
	std::string account_name;
};

struct PortfolioPosition {
	PortfolioPosition() = default;
	PortfolioPosition(const Contract &contract_, const Decimal position_, const double market_price_, const double
		market_value_,
	          const double avg_cost_, const double unrealized_pnl_, const double realized_pnl_)
		: contract(contract_), position(position_), market_price(market_price_), market_value(market_value_),
		  avg_cost(avg_cost_), unrealized_pnl(unrealized_pnl_), realized_pnl(realized_pnl_) {}

	Contract contract;
	Decimal position;
	double market_price;
	double market_value;
	double avg_cost;
	double unrealized_pnl;
	double realized_pnl;
};

struct AccountSummary {
	AccountSummary() = default;

	// %%% From AccountSummary %%%
	int reqId{-1};
	std::unordered_map<std::string, SummaryAttributes> attributes;

	// %%% From Managed Accounts %%%
	std::string account_id;

	// %%% From Account Updates (updateAccountValue) %%%
	std::unordered_map<std::string, UpdateAttribute> update_attributes;

	// %%% From Portfolio Updates (updatePortfolio) %%%
	std::vector<PortfolioPosition> portfolio;

	// %%% From Account Time (updateAccountTime) %%%
	std::string last_update_time;
};

struct AccountSummaryTags {
    inline static constexpr std::string_view AccountType                 = "AccountType";
    inline static constexpr std::string_view NetLiquidation             = "NetLiquidation";
    inline static constexpr std::string_view TotalCashValue             = "TotalCashValue";
    inline static constexpr std::string_view SettledCash                = "SettledCash";
    inline static constexpr std::string_view AccruedCash                = "AccruedCash";
    inline static constexpr std::string_view BuyingPower                = "BuyingPower";
    inline static constexpr std::string_view EquityWithLoanValue        = "EquityWithLoanValue";
    inline static constexpr std::string_view PreviousEquityWithLoanValue = "PreviousEquityWithLoanValue";
    inline static constexpr std::string_view GrossPositionValue         = "GrossPositionValue";
    inline static constexpr std::string_view ReqTEquity                 = "ReqTEquity";
    inline static constexpr std::string_view ReqTMargin                 = "ReqTMargin";
    inline static constexpr std::string_view SMA                        = "SMA";
    inline static constexpr std::string_view InitMarginReq              = "InitMarginReq";
    inline static constexpr std::string_view MaintMarginReq             = "MaintMarginReq";
    inline static constexpr std::string_view AvailableFunds             = "AvailableFunds";
    inline static constexpr std::string_view ExcessLiquidity            = "ExcessLiquidity";
    inline static constexpr std::string_view Cushion                    = "Cushion";
    inline static constexpr std::string_view FullInitMarginReq          = "FullInitMarginReq";
    inline static constexpr std::string_view FullMaintMarginReq         = "FullMaintMarginReq";
    inline static constexpr std::string_view FullAvailableFunds         = "FullAvailableFunds";
    inline static constexpr std::string_view FullExcessLiquidity        = "FullExcessLiquidity";
    inline static constexpr std::string_view LookAheadNextChange        = "LookAheadNextChange";
    inline static constexpr std::string_view LookAheadInitMarginReq     = "LookAheadInitMarginReq";
    inline static constexpr std::string_view LookAheadMaintMarginReq    = "LookAheadMaintMarginReq";
    inline static constexpr std::string_view LookAheadAvailableFunds    = "LookAheadAvailableFunds";
    inline static constexpr std::string_view LookAheadExcessLiquidity   = "LookAheadExcessLiquidity";
    inline static constexpr std::string_view HighestSeverity            = "HighestSeverity";
    inline static constexpr std::string_view DayTradesRemaining         = "DayTradesRemaining";
    inline static constexpr std::string_view Leverage                   = "Leverage";

    static std::string all() {
        return std::string(AccountType)
             + "," + std::string(NetLiquidation)
             + "," + std::string(TotalCashValue)
             + "," + std::string(SettledCash)
             + "," + std::string(AccruedCash)
             + "," + std::string(BuyingPower)
             + "," + std::string(EquityWithLoanValue)
             + "," + std::string(PreviousEquityWithLoanValue)
             + "," + std::string(GrossPositionValue)
             + "," + std::string(ReqTEquity)
             + "," + std::string(ReqTMargin)
             + "," + std::string(SMA)
             + "," + std::string(InitMarginReq)
             + "," + std::string(MaintMarginReq)
             + "," + std::string(AvailableFunds)
             + "," + std::string(ExcessLiquidity)
             + "," + std::string(Cushion)
             + "," + std::string(FullInitMarginReq)
             + "," + std::string(FullMaintMarginReq)
             + "," + std::string(FullAvailableFunds)
             + "," + std::string(FullExcessLiquidity)
             + "," + std::string(LookAheadNextChange)
             + "," + std::string(LookAheadInitMarginReq)
             + "," + std::string(LookAheadMaintMarginReq)
             + "," + std::string(LookAheadAvailableFunds)
             + "," + std::string(LookAheadExcessLiquidity)
             + "," + std::string(HighestSeverity)
             + "," + std::string(DayTradesRemaining)
             + "," + std::string(Leverage);
    }
};