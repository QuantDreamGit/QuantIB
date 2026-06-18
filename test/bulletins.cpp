#include <memory>
#include "quantib/sync/ib.hpp"

int main() {
	auto ib = std::make_unique<IB>();

	auto ok = ib->connect();
	if (!ok) {
		std::cerr << "Failed to connect to IB." << std::endl;
		return 1;
	}

	// Subscribe to bulletins and print them
	ib->bulletinsSub(true);

	std::cin.get();

	auto bulletins = ib->getBulletins();
	if (bulletins.has_value()) {
		for (const auto &bulletin: *bulletins.value()) {
			std::cout << "Bulletin id: " << bulletin.msg_id
			          << ", type: " << bulletin.msg_type
			          << ", message: " << bulletin.message
			          << ", exchange: " << bulletin.orig_exchange << std::endl;
		}
	}
}
