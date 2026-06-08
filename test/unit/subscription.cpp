#include "quantib/sync/ib.hpp"
#include <iostream>

int main() {
	auto ib = new IB();
	auto ok = ib->connect();
	ib->accountSummarySub();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	ib->accountSummaryCancel();
	std::this_thread::sleep_for(std::chrono::seconds(2));
}
