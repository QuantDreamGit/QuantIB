#include "quantib/core/account.hpp"

std::string AccountSummaryTags::all() {
	return std::string(AccountType) + "," + std::string(NetLiquidation) + "," + std::string(TotalCashValue) + "," +
		std::string(SettledCash) + "," + std::string(AccruedCash) + "," + std::string(BuyingPower) + "," +
		std::string(EquityWithLoanValue) + "," + std::string(PreviousEquityWithLoanValue) + "," +
		std::string(GrossPositionValue) + "," + std::string(ReqTEquity) + "," + std::string(ReqTMargin) + "," +
		std::string(SMA) + "," + std::string(InitMarginReq) + "," + std::string(MaintMarginReq) + "," +
		std::string(AvailableFunds) + "," + std::string(ExcessLiquidity) + "," + std::string(Cushion) + "," +
		std::string(FullInitMarginReq) + "," + std::string(FullMaintMarginReq) + "," +
		std::string(FullAvailableFunds) + "," + std::string(FullExcessLiquidity) + "," +
		std::string(LookAheadNextChange) + "," + std::string(LookAheadInitMarginReq) + "," +
		std::string(LookAheadMaintMarginReq) + "," + std::string(LookAheadAvailableFunds) + "," +
		std::string(LookAheadExcessLiquidity) + "," + std::string(HighestSeverity) + "," +
		std::string(DayTradesRemaining) + "," + std::string(Leverage);
}
