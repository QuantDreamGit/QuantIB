#pragma once
#include <string>

#include "Contract.h"
#include "EClientSocket.h"
#include "object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "quantib/order/order_factory.h"

struct OrderDetails {
	OrderDetails() = default;

	OrderDetails(const int order_id_, const Contract &contract_, const Order &order_, const OrderState &order_state_)
		: order_id(order_id_), contract(contract_), order(order_), order_state(order_state_) {
	}

	int order_id;
	Contract contract;
	Order order;
	OrderState order_state;
};

struct OrderStatus {
	OrderStatus() = default;

	OrderStatus(const int orderId_, std::string status_, const Decimal filled_, const Decimal remaining_, const double
				avgFillPrice_,
				const long long permId_, const int parentId_, const double lastFillPrice_, const int clientId_,
				const std::string &whyHeld_,
				const double mktCapPrice_)
		: orderId(orderId_), status(std::move(status_)), filled(filled_), remaining(remaining_), avgFillPrice(avgFillPrice_),
		  permId(permId_), parentId(parentId_), lastFillPrice(lastFillPrice_), clientId(clientId_), whyHeld(whyHeld_),
		  mktCapPrice(mktCapPrice_) {
	}

	int orderId;
	std::string status;
	Decimal filled;
	Decimal remaining;
	double avgFillPrice;
	long long permId;
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
	explicit ClosedOrders(OpenOrders &open_order_) : orderDetails(std::move(open_order_.orderDetails)), orderStatus(
														std::move(open_order_.orderStatus)) {}
};

class OrderManager {
public:
	OrderManager(EClientSocket &client, BlockingHub &hub, ObjectHub &obj, Logger &logger, OrderFactory &factory) : logger_(logger),
																							   client_(client),
																							   obj_(obj), hub_(hub), factory_(factory) {
		// Create both objects and then store them
		openOrders_ = &obj_.create<OpenOrderStoreTag, std::unordered_map<int, OpenOrders> >();
		closedOrders_ = &obj_.create<ClosedOrderStoreTag, std::unordered_map<int, ClosedOrders> >();
	}

private:
	Logger &logger_;
	EClientSocket &client_;
	ObjectHub &obj_;
	BlockingHub &hub_;
	OrderFactory &factory_;

	std::unordered_map<int, OpenOrders> *openOrders_;
	std::unordered_map<int, ClosedOrders> *closedOrders_;
};
