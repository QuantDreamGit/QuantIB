#include "quantib/sync/ib.hpp"
#include <iostream>
#include <thread>

int main() {
	// Do a subscription to account summary and account update, then cancel the subscription
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	// Account Summary Subscription
	ib->accountSummarySub();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	// Account Update Subscription
	ib->accountUpdateSub();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	// Cancel Account Summary Subscription
	ib->accountSummaryCancel();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	// Cancel Account Update Subscription
	ib->accountUpdateCancel();

	std::this_thread::sleep_for(std::chrono::seconds(10));
}
