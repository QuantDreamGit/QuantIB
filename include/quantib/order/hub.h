#pragma once
#include <vector>

#include "Contract.h"
#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "quantib/core/tags.hpp"

struct OrderDetails {
	OrderDetails() = default;

	OrderDetails(const int order_id, const Contract &contract, const Order &order, const OrderState &order_state)
		: order_id(order_id), contract(contract), order(order), order_state(order_state) {
	}

	int order_id;
	Contract contract;
	Order order;
	OrderState order_state;
};

struct OrderStatus {
	OrderStatus() = default;

	OrderStatus(const int orderId, std::string status, const Decimal filled, const Decimal remaining, const double
	            avgFillPrice,
	            const int permId, const int parentId, const double lastFillPrice, const int clientId,
	            const std::string &whyHeld,
	            const double mktCapPrice)
		: orderId(orderId), status(std::move(status)), filled(filled), remaining(remaining), avgFillPrice(avgFillPrice),
		  permId(permId), parentId(parentId), lastFillPrice(lastFillPrice), clientId(clientId), whyHeld(whyHeld),
		  mktCapPrice(mktCapPrice) {
	}

	int orderId;
	std::string status;
	Decimal filled;
	Decimal remaining;
	double avgFillPrice;
	int permId;
	int parentId;
	double lastFillPrice;
	int clientId;
	std::string whyHeld;
	double mktCapPrice;
};

struct OpenOrders {
	OrderDetails orderDetails;
	OrderStatus orderStatus;
};

struct ClosedOrders {
	OrderDetails orderDetails;
	OrderStatus orderStatus;
	// Other details in the future
	Execution executionDetails;

	ClosedOrders() = default;
	explicit ClosedOrders(OpenOrders &open_order) : orderDetails(std::move(open_order.orderDetails)), orderStatus(
		                                                std::move(open_order.orderStatus)) {}
};

class OrderHub {
public:
	explicit OrderHub(std::shared_ptr<BlockingHub> hub,
	                  std::shared_ptr<Logger> logger) : hub_(std::move(hub)), logger_(std::move(logger)) {
		LOG_DEBUG_TAG(ORDER_HUB, "Initialized correctly.");
	}

	void setClient(EClientSocket *client) {
		client_ = client;
	}

	void updateOpenOrders(const int orderId, const Contract &contract, const Order &order, const OrderState
	                      &orderState) {
		openOrders_[orderId].orderDetails = OrderDetails(orderId, contract, order, orderState);
		LOG_DEBUG_TAG(ORDER_HUB, "Updated open order with id {} for contract {}.", orderId, contract.symbol);
	}

	void updateOrderStatus(const int orderId, const std::string &status, const Decimal filled, const Decimal remaining,
	                       const double avgFillPrice, const int permId, const int parentId, const double lastFillPrice,
	                       const int clientId, const std::string &whyHeld, const double mktCapPrice) {
		openOrders_[orderId].orderStatus = OrderStatus(orderId, status, filled, remaining, avgFillPrice, permId,
		                                               parentId, lastFillPrice, clientId, whyHeld, mktCapPrice);
		LOG_DEBUG_TAG(ORDER_HUB, "Updated order status for order with id {}: "
		              "status={}, filled={}, remaining={}, avgFillPrice={}, permId={}, parentId={}, lastFillPrice={}, clientId={}, whyHeld={}, mktCapPrice={}",
		              orderId, status, DecimalFunctions::decimalToString(filled),
		              DecimalFunctions::decimalToString(remaining), avgFillPrice, permId, parentId, lastFillPrice,
		              clientId, whyHeld, mktCapPrice);
	}

	void moveToClosedOrders(const int orderId) {
		if (openOrders_.contains(orderId)) {
			closedOrders_[orderId] = ClosedOrders(openOrders_[orderId]);
			/* In paper trading is not working!
			auto exec = hub_->wait_for<ExecutionDetailsTag, Execution>([&]() {
				auto filter = ExecutionFilter();
				filter.m_symbol = openOrders_[orderId].orderDetails.contract.symbol;
				filter.m_secType = openOrders_[orderId].orderDetails.contract.secType;
				filter.m_exchange = openOrders_[orderId].orderDetails.contract.exchange;
				client_->reqExecutions(orderId, filter);
				LOG_TRACE_TAG(ORDER_HUB, "Requested execution details for order with id {}.", orderId);
			});
			closedOrders_[orderId].executionDetails = exec.value();
			*/
			openOrders_.erase(orderId);
			LOG_DEBUG_TAG(ORDER_HUB, "Moved order with id {} to closed orders.", orderId);
		} else {
			LOG_WARN_TAG(ORDER_HUB, "Tried to move order with id {} to closed orders, but it was not found in open orders.", orderId);
		}
	}

	void updateOpenOrders() {
		client_->reqAllOpenOrders();
	}

	[[nodiscard]] std::vector<OpenOrders> getOpenOrders() {
		std::vector<OpenOrders> details;
		for (auto &val: openOrders_) {
			details.push_back(std::move(val.second));
		}
		return details;
	}

	[[nodiscard]] std::vector<ClosedOrders> getClosedOrders() {
		std::vector<ClosedOrders> details;
		for (auto &val: closedOrders_) {
			details.push_back(std::move(val.second));
		}
		return details;
	}

private:
	EClientSocket *client_;
	std::shared_ptr<BlockingHub> hub_;
	std::shared_ptr<Logger> logger_;

	std::unordered_map<int, OpenOrders> openOrders_;
	std::unordered_map<int, ClosedOrders> closedOrders_;
};
