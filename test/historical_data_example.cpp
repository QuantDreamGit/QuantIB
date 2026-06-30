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
	HistoricalSeriesSnapshot snapshot;
	/*
	Contract contract;
	contract.symbol = "SPY";
	contract.secType = "STK";
	contract.currency = "USD";
	contract.exchange = "SMART";
	*/
	Contract contract;
	contract.symbol = "BMW";
	contract.secType = "STK";
	contract.currency = "EUR";
	contract.exchange = "SMART";

	ib->setOnBarClosed([&](const int req_id, const OhlcBar& bar) {
		std::cout << "Historical data update COMPLETE for reqId: " << req_id << ", bar timestamp: " << bar.timestamp
		          << std::endl;
		// Append bar
		snapshot.bars.emplace_back(bar);
		snapshot.last_timestamp = bar.timestamp;
		snapshot.last_update = std::chrono::system_clock::now();

	});

	ib->setOnBarUpdate([&](int req_id, const OhlcBar& bar) {
		std::cout << "Historical data update for reqId: " << req_id << ", bar timestamp: " << bar.timestamp
		          << std::endl;
		snapshot.active_bar = bar;
	});

	ib->setOnBarSeriesComplete([&](const int id) {
		std::cout << "Historical data series COMPLETE for reqId: " << id << std::endl;
		const auto res = ib->getHistoricalSnapshot(id);
		if (res.has_value()) snapshot = res.value();
	});


	ib->registerContract(contract);
	// ib->registerMarketData(contract);
	int req_id = ib->subscribeHistoricalBars(contract, "", "1 W", "1 min", "TRADES");

	std::cin.get();

	return 0;
}
