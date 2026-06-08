#include "quantib/sync/ib.hpp"
#include <iostream>
#include <thread>

int main() {
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();
	ib->accountSummarySub();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	ib->accountSummaryCancel();
	std::this_thread::sleep_for(std::chrono::seconds(2));
}
