#include "quantib/sync/ib.hpp"
#include <iostream>
int main() {
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	Contract contract1;
	contract1.symbol = "EXW1";
	contract1.secType = "STK";
	contract1.exchange = "SMART";
	contract1.currency = "EUR";

	ib->registerMarketData(contract1, "", false, false);

	Contract contract2;
	contract2.symbol = "AAPL";
	contract2.secType = "STK";
	contract2.exchange = "SMART";
	contract2.currency = "USD";

	ib->registerMarketData(contract2, "", false, false);

	std::cin.get();
}