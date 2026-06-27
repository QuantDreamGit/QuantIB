#pragma once

#include "Contract.h"
#include "DefaultEWrapper.h"

#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

#include <set>
#include <string>
#include <vector>

#include "Execution.h"

// This class is used to override default wrapper functions
class ResponseWrapper : public DefaultEWrapper {
public:
	explicit ResponseWrapper(BlockingHub& hub, ObjectHub& obj, Logger& logger);

	~ResponseWrapper() override = default;

	// Deprecated
	// void connectAck() override { hub_->send(typeid(connectTag), true); }

	void nextValidId(int orderId) override;

	/* Account ids */
	void managedAccounts(const std::string& accountsList) override;

	/* Account Summary Subscription */
	void accountSummary(int reqId, const std::string& account, const std::string& tag, const std::string& value,
	                    const std::string& currency) override;

	void accountSummaryEnd(int reqId) override;

	/* Account Update Subscription */
	void updateAccountValue(const std::string& key, const std::string& val, const std::string& currency,
	                        const std::string& accountName) override;

	void updatePortfolio(const Contract& contract, Decimal position, double marketPrice, double marketValue,
	                     double averageCost, double unrealizedPNL, double realizedPNL,
	                     const std::string& accountName) override;

	void updateAccountTime(const std::string& timeStamp) override;

	void accountDownloadEnd(const std::string& accountName) override;

	void contractDetails(int reqId, const ContractDetails& contractDetails) override;

	void contractDetailsEnd(int reqId) override;

	void openOrder(int orderId, const Contract& contract, const Order& order, const OrderState& order_state) override;

	void orderStatus(int orderId, const std::string& status, Decimal filled, Decimal remaining, double avgFillPrice,
	                 long long permId, int parentId, double lastFillPrice, int clientId, const std::string& whyHeld,
	                 double mktCapPrice) override;

	void openOrderEnd() override;

	void execDetails(int reqId, const Contract& contract, const Execution& execution) override;

	void execDetailsEnd(int reqId) override;

	void position(const std::string& account, const Contract& contract, Decimal position, double avgCost) override;

	void positionEnd() override;

	void updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage,
	                        const std::string& originExch) override;


	void error(int id, time_t errorTime, int errorCode, const std::string& errorString,
	           const std::string& advancedOrderRejectJson) override;

	void tickPrice(int reqId, const TickType field, double price, const TickAttrib& attrib) override;

	void tickGeneric(int reqId, TickType tickType, double value) override;

	void tickString(int reqId, TickType tickType, const std::string& value) override;

	void tickSize(int reqId, TickType field, Decimal size) override;

	void tickOptionComputation(int reqId, TickType tickType, int tickAttrib, double impliedVol, double delta,
	                           double optPrice, double pvDividend, double gamma, double vega, double theta,
	                           double undPrice) override;

	void securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId,
	                                         const std::string& tradingClass, const std::string& multiplier,
	                                         const std::set<std::string>& expirations,
	                                         const std::set<double>& strikes) override;

	void securityDefinitionOptionalParameterEnd(int reqId) override;


private:
	BlockingHub& hub_;
	ObjectHub& obj_;
	Logger& logger_;
};
