#include "quantib/sync/ib.hpp"
#include "../include/quantib/order/order_types.hpp"
#include <iostream>
#include <thread>

int main() {
	// Do a subscription to account summary and account update, then cancel the subscription
	const auto ib = std::make_unique<IB>();
	auto ok = ib->connect();

	// We typically want to request a contract to get all details of it.
	// We find the complete contract starting of a sub-set of the infos.
	Contract contract;
	contract.symbol = "EXW1";
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "EUR";

	// We can request contract details that have contract information AND other infos,
	// Otherwise, it's possible to directly fetch contracts
	// auto result = ib->getContractDetails(1, contract);
	auto result = ib->getContractsSync(1, contract);

	if (result.has_value() && result.value().size() == 1) {
		const auto order = OrderSamples::MarketOrder("BUY", DecimalFunctions::doubleToDecimal(1.0));
		const auto final_contract = result.value()[0];
		ib->placeOrder(final_contract, order);
	}
	std::cin.get();
	std::optional<std::unordered_map<int, OpenOrders>*> open_orders = ib->getOpenOrders();
	if (open_orders.has_value()) {
		std::cout << "Open orders: " << open_orders.value()->size() << std::endl;
		for (const auto& order : *open_orders.value()) {
			std::cout << "Order id: " << order.second.orderDetails.order_id << std::endl;
			std::cout << "Contract symbol: " << order.second.orderDetails.contract.symbol << std::endl;
			std::cout << "Order action: " << order.second.orderDetails.order.action << std::endl;
			std::cout << "Order type: " << order.second.orderDetails.order.orderType << std::endl;
			std::cout << "Order quantity: " << DecimalFunctions::decimalToString(
				order.second.orderDetails.order.totalQuantity) << std::endl;
			std::cout << "Order limit price: " << order.second.orderDetails.order.lmtPrice << std::endl;
			std::cout << "Order status: " << order.second.orderStatus.status << std::endl;
			std::cout << "Order filled quantity: " << DecimalFunctions::decimalToString(order.second.orderStatus.filled)
				<< std::endl;
			std::cout << "Order remaining quantity: " << DecimalFunctions::decimalToString(
				order.second.orderStatus.remaining) << std::endl;
			std::cout << "Order average fill price: " << order.second.orderStatus.avgFillPrice << std::endl;
		}
	}

	std::cin.get();

	// Verify that the order has been filled and moved to closed orders
	std::optional<std::unordered_map<int, ClosedOrders>*> closed_orders = ib->getClosedOrders();

	if (closed_orders.has_value()) {
		std::cout << "Closed orders: " << closed_orders.value()->size() << std::endl;
		for (const auto& order : *closed_orders.value()) {
			std::cout << "Order id: " << order.second.orderDetails.order_id << std::endl;
			std::cout << "Average price fill: " << order.second.orderStatus.avgFillPrice << std::endl;
		}
	}
}
