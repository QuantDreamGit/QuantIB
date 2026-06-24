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

void sync_method() {
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	if (!ok) throw "Connection failed!";

	// We typically want to request a contract to get all details of it.
	// We find the complete contract starting of a sub-set of the infos.
	Contract contract;
	contract.symbol = "AAPL";
	contract.secType = "STK";
	contract.exchange = "SMART";
	// contract.currency = "USD";	// -> We expect to find more than one contract!

	// We can request contract details that have contract information AND other infos,
	// Otherwise, it's possible to directly fetch contracts
	// auto result = ib->getContractDetails(1, contract);
	auto contracts = ib->getContractsSync(1, contract);

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

void async_method() {
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	if (!ok) throw "Connection failed!";

	Contract contract;
	contract.symbol = "AAPL";
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "USD"; // -> We expect to find one contract!

	ib->registerContract(contract);

	std::cin.get();

	const auto contract_details = ib->getContractDetails(contract);
	const auto contract_result = ib->getContract(contract);

	std::cout << contract_details.has_value() << std::endl;
	std::cout << contract_result.has_value() << std::endl;
}

int main() {
	async_method();
	sync_method();
}
