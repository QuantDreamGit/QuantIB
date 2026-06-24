#pragma once
#include "quantib/server/server.h"

#include "quantib/order/order_types.hpp"
#include "nlohmann/json.hpp"

template <typename ProfileT>
void HttpServerIB<ProfileT>::start(std::string host, int port) {
	std::cout << "HTTP server running on http://" << host << ":" << port << "\n";
	server_.listen(host, port);
}

template <typename ProfileT>
void HttpServerIB<ProfileT>::setupRoutes() {
	server_.Get("/positions", [&](const httplib::Request&, httplib::Response& res) {
		auto positions = ib_.getPositions();

		if (!positions) {
			res.status = 404;
			res.set_content(R"({"error":"No positions available yet"})", "application/json");
			return;
		}

		std::string json = "[";
		bool first = true;
		nlohmann::json out = nlohmann::json::array();
		for (const auto& p : *positions.value()) {
			out.push_back({
				{"symbol", p.second.contract.symbol}, {"exchange", p.second.contract.exchange},
				{"position", DecimalFunctions::decimalToString(p.second.pos)}, {"avgCost", p.second.avgCost}
			});
		}

		res.set_content(out.dump(), "application/json");
	});

	server_.Post("/order", [&](const httplib::Request& req, httplib::Response& res) {
		auto body = nlohmann::json::parse(req.body);

		std::string symbol = body["symbol"];
		std::string side = body["side"];
		double quantity = body["quantity"];

		Contract contract;
		contract.symbol = symbol;
		contract.secType = "STK";
		contract.exchange = "SMART";
		contract.currency = "USD";

		auto result = ib_.getContractsSync(1, contract);

		if (!result.has_value() || result->size() != 1) {
			res.status = 400;
			res.set_content(R"({"ok":false,"error":"Contract not found"})", "application/json");
			return;
		}

		auto order = OrderSamples::MarketOrder(side, DecimalFunctions::doubleToDecimal(quantity));

		ib_.placeOrder(result->at(0), order);

		res.set_content(R"({"ok":true})", "application/json");
	});
}
