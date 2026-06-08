#pragma once

#include "Contract.h"
#include "DefaultEWrapper.h"

#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

#include <iostream>
#include <memory>

#include "quantib/core/account.hpp"

// This class is used to override default wrapper functions
class ResponseWrapper : public DefaultEWrapper {
public:
	explicit ResponseWrapper(std::shared_ptr<BlockingHub> hub, std::shared_ptr<ObjectHub> obj, const
	                         std::shared_ptr<Logger> &logger) : hub_(hub), obj_(obj), logger_(logger) {
		LOG_DEBUG_TAG(WRAPPER, "Initialized correctly.");
	}

	~ResponseWrapper() override = default;

	// Deprecated
	// void connectAck() override { hub_->send(typeid(connectTag), true); }

	void nextValidId(const int orderId) override {
		if (hub_->containsKey<ConnectTag>()) {
			hub_->send<ConnectTag>(orderId);
		} else
			hub_->send<NextValidIdTag>(orderId);
	}

	void position(std::string account, Contract contract, Decimal pos,
	              double avgCost) {
		std::cout << account << std::endl;
	}

	/* Account Summary Subscription */
	void accountSummary(int reqId, const std::string &account, const std::string &tag, const std::string &value,
	                    const std::string &currency) override;
	void accountSummaryEnd(int reqId) override;

private:
	std::shared_ptr<BlockingHub> hub_;
	std::shared_ptr<ObjectHub> obj_;
	std::shared_ptr<Logger> logger_;
};
