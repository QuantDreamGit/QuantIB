#include "quantib/sync/ib.hpp"
#include "../include/quantib/order/order_types.hpp"
#include <iostream>
#include <thread>

void notional_policy(const IB *ib, const Contract &contract) {
	// We can request contract details that have contract information AND other infos,
	// Otherwise, it's possible to directly fetch contracts
	// auto result = ib->getContractDetails(1, contract);
	auto result = ib->getContractsSync(1, contract);

	if (result.has_value() && result.value().size() == 1) {
		const auto big_order = OrderSamples::MarketOrder("BUY", DecimalFunctions::doubleToDecimal(100.0));
		const auto final_contract = result.value()[0];

		// First we have to register a subscription to the contract
		ib->registerMarketData(final_contract);

		// Wait few seconds
		std::cin.get();

		// Then try to place orders
		std::cout << "Trying to place a big order: " << std::endl;
		ib->placeOrder(final_contract, big_order);

		const auto small_order = OrderSamples::MarketOrder("BUY", DecimalFunctions::doubleToDecimal(10.0));
		std::cout << "Trying to place a small order: " << std::endl;
		ib->placeOrder(final_contract, small_order);
	}
}

void contract_ready(const IB *ib, const Contract &contract) {
	// If you want to test missing contract policy  using async method
	const auto small_order = OrderSamples::MarketOrder("BUY", DecimalFunctions::doubleToDecimal(1));

	ib->registerInstrument(contract);
	// Try to immediately place order (should fail if not already registered!)
	ib->placeOrder(contract, small_order);
}

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

	contract_ready(ib.get(), contract);
	// notional_policy(ib.get(), contract);
}
