#include "quantib/sync/ib.hpp"
#include <iostream>
#include <thread>

void printContractMsg(const Contract& contract) {
	printf("\tConId: %d\n", contract.conId);
	printf("\tSymbol: %s\n", contract.symbol.c_str());
	printf("\tSecType: %s\n", contract.secType.c_str());
	printf("\tLastTradeDateOrContractMonth: %s\n", contract.lastTradeDateOrContractMonth.c_str());
	printf("\tLastTradeDate: %s\n", contract.lastTradeDate.c_str());
	printf("\tStrike: %s\n", std::to_string(contract.strike).c_str());
	printf("\tRight: %s\n", contract.right.c_str());
	printf("\tMultiplier: %s\n", contract.multiplier.c_str());
	printf("\tExchange: %s\n", contract.exchange.c_str());
	printf("\tPrimaryExchange: %s\n", contract.primaryExchange.c_str());
	printf("\tCurrency: %s\n", contract.currency.c_str());
	printf("\tLocalSymbol: %s\n", contract.localSymbol.c_str());
	printf("\tTradingClass: %s\n", contract.tradingClass.c_str());
}

int main() {
	// Do a subscription to account summary and account update, then cancel the subscription
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	// We typically want to request a contract to get all details of it.
	// We find the complete contract starting of a sub-set of the infos.
	Contract contract;
	contract.symbol = "AAPL";
	contract.secType = "STK";
	contract.exchange = "SMART";
	// contract.currency = "USD";

	// We can request contract details that have contract information AND other infos,
	// Otherwise, it's possible to directly fetch contracts
	// auto result = ib->getContractDetails(1, contract);
	auto contracts = ib->getContracts(1, contract);

	if (contracts.has_value()) {
		std::cout << contracts.value().size() << " Contracts have been found!" << std::endl;
		for (const auto& contractDetails : contracts.value()) {
			std::cout << "==================================" << std::endl;
			printContractMsg(contractDetails);
			std::cout << "==================================" << std::endl;
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));
}