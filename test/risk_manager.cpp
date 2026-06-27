#include "quantib/sync/ib.hpp"
#include "../include/quantib/order/order_types.hpp"
#include <iostream>
#include <thread>

template <typename ProfileT>
void notional_policy(const IB<ProfileT>* ib, const Contract& contract) {
	// We can request contract details that have contract information AND other infos,
	// Otherwise, it's possible to directly fetch contracts
	// auto result = ib->getContractDetails(1, contract);
	auto result = ib->getContractsSync(1, contract);

	if (result.has_value() && result.value().size() == 1) {
		const auto final_contract = result.value()[0];

		// First we have to register a subscription to the contract
		ib->registerMarketData(final_contract);

		// Wait few seconds
		std::cin.get();
		auto large_request = TradeRequest(
			TradeRequestContext(contract, Side::Buy, "Testing notional policy (should fail with default settings"),
			MarketTradeSpec(100.0, OrderType::Market));
		// Then try to place orders
		std::cout << "Trying to place a big order: " << std::endl;
		ib->placeOrder(final_contract, large_request);

		auto small_request = TradeRequest(
			TradeRequestContext(contract, Side::Buy, "Testing Notional policy (should pass with default settings"),
			MarketTradeSpec(10.0, OrderType::Market));
		std::cout << "Trying to place a small order: " << std::endl;
		ib->placeOrder(final_contract, small_request);
	}
}

template <typename ProfileT>
void contract_ready(const IB<ProfileT>* ib, const Contract& contract) {
	// If risk manager detects a contract not subscribed it does using executeAction()
	// ib->registerInstrument(contract);
	// Try to immediately place order (should fail if not already registered!)
	/* Market Order Example
	auto request = TradeRequest(
		TradeRequestContext(contract, Side::Buy, "Testing contract ready policy"),
	    MarketTradeSpec(1.0, OrderType::Market));
	*/
	auto request = TradeRequest(
		TradeRequestContext(contract, Side::Buy, "Testing Bracket order"),
		BracketOrderSpec(1, 62, 63, 61));
	ib->placeOrder(request);
}

int main() {
	// Do a subscription to account summary and account update, then cancel the subscription
	auto ib = std::make_unique<IB<DefaultProfile>>();
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
