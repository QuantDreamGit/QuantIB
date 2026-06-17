#include "quantib/server/server.h"

#include "quantib/order/order_types.hpp"

void HttpServerIB::start(std::string host, int port) {
	std::cout << "HTTP server running on http://" << host << ":" << port << "\n";
	server_.listen(host, port);
}

void HttpServerIB::setupRoutes() {
	server_.Get("/positions", [&](const httplib::Request&, httplib::Response& res) {
		auto positions = ib_.getPositions();

		if (!positions) {
			res.status = 404;
			res.set_content(R"({"error":"No positions available yet"})", "application/json");
			return;
		}

		std::string json = "[";
		bool first = true;
		for (const auto& p : *positions.value()) {
			if (!first) json += ",";
			first = false;

			json += "{";
			json += "\"symbol\":\"" + p.contract.symbol + "\",";
			json += "\"exchange\":" + p.contract.exchange + ",";
			json += "\"avgCost\":" + std::to_string(p.avgCost);
			json += "}";
		}

		json += "]";

		res.set_content(json, "application/json");
	});

	server_.Post("/test_order", [&](const httplib::Request&, httplib::Response& res) {
		Contract contract;
		contract.symbol = "EXW1";
		contract.secType = "STK";
		contract.exchange = "SMART";
		contract.currency = "EUR";

		// We can request contract details that have contract information AND other infos,
		// Otherwise, it's possible to directly fetch contracts
		// auto result = ib->getContractDetails(1, contract);
		auto result = ib_.getContracts(1, contract);

		if (result.has_value() && result.value().size() == 1) {
			const auto order = OrderSamples::MarketOrder("BUY", DecimalFunctions::doubleToDecimal(1.0));
			const auto final_contract = result.value()[0];
			ib_.placeOrder(final_contract, order);
		}

	res.set_content(R"({"ok":true,"message":"Test order sent"})", "application/json");
});
}
