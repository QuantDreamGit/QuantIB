#include <cassert>
#include <optional>

#include "quantib/sync/ib.hpp"

int main() {
	// create client
	auto ib = new IB();
	// Get an optional<int>
	std::optional<int> isConnect = ib->connect();
	// Assert
	assert(isConnect.has_value() == true);
	return 0;
}
