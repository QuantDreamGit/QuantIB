#include "quantib/sync/ib.hpp"
#include "quantib/core/tags.hpp"

void ResponseWrapper::openOrder(int orderId, const Contract &contract, const Order &order, const OrderState
                                &order_state) {
	orders_->updateOpenOrders(orderId, contract, order, order_state);
	LOG_TRACE_TAG(WRAPPER, "Received open order with id {} for contract {}.", orderId, contract.symbol);
}

void ResponseWrapper::orderStatus(int orderId, const std::string &status, Decimal filled, Decimal remaining,
                                  double avgFillPrice, long long permId, int parentId, double lastFillPrice,
                                  int clientId, const std::string &whyHeld, double mktCapPrice) {
	if (DecimalFunctions::decimalToDouble(filled) > 0) {
		orders_->updateOrderStatus(orderId, status, filled, remaining, avgFillPrice, permId, parentId, lastFillPrice,
								   clientId, whyHeld, mktCapPrice);
		LOG_TRACE_TAG(
		WRAPPER,
		"Received order status update for order with id {}: "
		"status={}, filled={}, remaining={}, avgFillPrice={}, permId={}, parentId={}, lastFillPrice={}, clientId={}, "
		"whyHeld={}, mktCapPrice={}",
		orderId,
		status, DecimalFunctions::decimalToString(filled), DecimalFunctions::decimalToString(remaining), avgFillPrice,
		permId, parentId, lastFillPrice, clientId, whyHeld,
		mktCapPrice);
	} else {
		LOG_TRACE_TAG(WRAPPER, "Order with id {} has been filled, moved to closed orders.", orderId);
		orders_->moveToClosedOrders(orderId);
	}
}

void ResponseWrapper::openOrderEnd() {
	LOG_DEBUG_TAG(WRAPPER, "Finished receiving open orders.");
	hub_->send<OpenOrdersTag, std::vector<OpenOrders>>(orders_->getOpenOrders());
}

std::optional<std::vector<OpenOrders>> IB::getOpenOrders() const {
	LOG_DEBUG_TAG(IB_STR, "Requesting open orders.");
	return hub_->wait_for<OpenOrdersTag, std::vector<OpenOrders>>([&]() {
		client_->reqAllOpenOrders();
	});
}

