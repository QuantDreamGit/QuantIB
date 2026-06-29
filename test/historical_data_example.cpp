#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "config.h"
#include "../external/IBJts/samples/Cpp/TestCppClient/ContractSamples.h"
#include "quantib/sync/ib.hpp"

int main() {
	auto ib = std::make_unique<IB<DefaultProfile>>();
	const auto connected = ib->connect();
	if (!connected) {
		std::cerr << "Failed to connect to IB/TWS.\n";
		return 1;
	}

	Contract contract;
	contract.symbol = "SPY";
	contract.secType = "STK";
	contract.currency = "USD";
	contract.exchange = "SMART";

	ib->registerContract(contract);
	// ib->registerMarketData(contract);

	// ib->requestHistoricalBars(contract, "", "1 M", "1 MIN", "TRADES");
	ib->subscribeHistoricalBars(contract, "", "1 M", "1 MIN", "TRADES");

	std::this_thread::sleep_for(std::chrono::seconds(30));
	return 0;
}
