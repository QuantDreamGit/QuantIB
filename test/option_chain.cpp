#include "quantib/sync/ib.hpp"
#include <iostream>
#include <thread>

#include "../external/IBJts/samples/Cpp/TestCppClient/ContractSamples.h"

struct DefaultProfile;

int main() {
	Contract contract;
	contract.symbol = "AAPL";
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "USD";
	// test
	// Do a subscription to account summary and account update, then cancel the subscription
	auto ib = std::make_unique<IB<DefaultProfile>>();
	auto ok = ib->connect();
	ib->registerContract(contract);
	std::this_thread::sleep_for(std::chrono::seconds(5));

	// Get entire option chain
	std::optional<std::vector<OptionChain>> entireOptionChain = ib->getOptionChainSync(contract);
	// Get a specific exchange from above
	std::optional<OptionChain> optChain = OptionChain::getExchangeFromOptionChain(entireOptionChain.value(), "SMART");
	// Get a fixed number of exp and strike from exp date and current price
	std::pair<std::vector<std::string>, std::vector<double>> exp_strk;
	if (optChain.has_value()) {
		exp_strk = optChain.value().getFromNearestDate(1, 3, 282.5);
	}

	// Then register instrument in a single exchange to get greeks
	if (optChain.has_value() && !exp_strk.first.empty() && !exp_strk.second.empty()) {
		contract.secType = "OPT";
		contract.right = "C";
		contract.exchange = "SMART";
		contract.multiplier = "100";
		for (const auto &exp : exp_strk.first) {
			for (const auto strike : exp_strk.second) {
				contract.strike = strike;
				contract.lastTradeDateOrContractMonth = exp;
				ib->registerMarketData(contract);
			}
		}
	}


	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << entireOptionChain.has_value() << std::endl;
}
