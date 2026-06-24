#include "quantib/server/server.h"
#include "quantib/sync/ib.hpp"
#include <memory>

int main() {
	auto ib = std::make_unique<IB>();
	auto ok = ib->connect();


	auto server = HttpServerIB(*ib);
	server.start();
}
