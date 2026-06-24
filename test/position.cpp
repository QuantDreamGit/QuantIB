#include "quantib/sync/ib.hpp"
#include "../include/quantib/order/order_types.hpp"
#include <iostream>
#include <thread>

int main() {
	// Do a subscription to account summary and account update, then cancel the subscription
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	ib->PositionsSub();

	std::cin.get();

	auto result = ib->getPositions();

	std::cout << "Positions received: " << result.value().size() << std::endl;
}
