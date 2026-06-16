#pragma once
#include "EClientSocket.h"
#include "EReader.h"
#include "EReaderOSSignal.h"
#include "Contract.h"

#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/wrappers/base_wrapper.hpp"
#include "quantib/core/account.hpp"
#include "quantib/utils/logger.hpp"

#include <memory>
#include <optional>
#include <thread>

#include "quantib/core/news_manager.h"
#include "quantib/core/order_manager.h"
#include "quantib/core/position_manager.h"

class IB {
public:
	using log_level = spdlog::level::level_enum;
	// Default Constructor
	explicit IB(log_level lvl = LOG_TRACE_LVL) {
		// Logger
		logger_ = std::make_unique<Logger>(lvl);
		// Object Hub
		obj_ = std::make_unique<ObjectHub>(logger_);
		// Connection Hub
		hub_ = std::make_unique<BlockingHub>(*obj_, *logger_);
		// Client
		wrapper_ = std::make_unique<ResponseWrapper>(*hub_, *obj_, *logger_);
		signal_ = std::make_unique<EReaderOSSignal>(2000);
		client_ = std::make_unique<EClientSocket>(wrapper_.get(), signal_.get());
		// Managers
		positions_ = std::make_unique<PositionManager>(*client_, *hub_, *obj_, *logger_);
		orders_ = std::make_unique<OrderManager>(*client_, *hub_, *obj_, *logger_);
		bulletins_ = std::make_unique<BulletinManager>(*client_, *hub_, *obj_, *logger_);

	};

	~IB() {
		isRunning_ = false;
		if (message_thread_.joinable())
			message_thread_.join();
		if (client_ && client_->isConnected())
			client_->eDisconnect();
	}

	/* Network */
	inline std::optional<int> connect() {
		hub_->insertKey<NextValidIdTag>();
		client_->asyncEConnect(false);
		bool ok = client_->eConnect("127.0.0.1", 7497, 0, false);

		if (ok) {
			// Start EReader
			reader_ = std::make_unique<EReader>(client_.get(), signal_.get());
			reader_->start();
			isRunning_ = true;
			// Then start the message thread
			message_thread_ = std::thread([this] {
				while (isRunning_) {
					signal_->waitForSignal();
					reader_->processMsgs();
				}
			});

			nextId_ = hub_->wait_for<ConnectTag, int>([&]() {
			}).value_or(-1);
			return nextId_;
		}

		return std::nullopt;
	}

	/* Account */
	void accountSummarySub(
		const std::string &tags = AccountSummaryTags::all(),
		const std::string &groups = "All");
	void accountSummaryCancel() const;
	[[nodiscard]] std::optional<AccountSummary> getAccountIds() const;
	void accountUpdateSub() const;
	void accountUpdateCancel() const;

	/* Contracts */
	[[nodiscard]] std::optional<std::vector<ContractDetails>> getContractDetails(int reqId, const Contract &contract) const;
	[[nodiscard]] std::optional<std::vector<Contract>> getContracts(int reqId, const Contract &contract) const;

	/* Orders */
	void placeOrder(const Contract &contract, const Order &order) {
		client_->placeOrder(nextId_++, contract, order);
		LOG_DEBUG_TAG(IB_STR, "Placed order with id {} for contract {}.", nextId_ - 1, contract.symbol);
	}

	[[nodiscard]] std::optional<std::unordered_map<int, OpenOrders>*> getOpenOrders() const;
	[[nodiscard]] std::optional<std::unordered_map<int, ClosedOrders>*> getClosedOrders() const;

	void PositionsSub() const {
		client_->reqPositions();
	}

	[[nodiscard]] std::optional<std::vector<Position>*> getPositions() const {

		if (auto* positions = obj_->try_get<PositionStoreTag, std::vector<Position>>()) {
			return positions;
		}
		return std::nullopt;
	}

	void bulletinsSub(const bool value) const {
		if (value) bulletins_->startSubscription();
		else bulletins_->stopSubscription();
	}

	[[nodiscard]] std::optional<std::vector<Bulletin>*> getBulletins() const {
		if (auto* bulletins = obj_->try_get<BulletinStoreTag, std::vector<Bulletin>>()) {
			return bulletins;
		}
		return std::nullopt;
	}

protected:
	std::shared_ptr<Logger> logger_;
	std::unique_ptr<ResponseWrapper> wrapper_;
	std::unique_ptr<EReaderOSSignal> signal_;
	std::unique_ptr<EClientSocket> client_;
	std::unique_ptr<EReader> reader_;
	std::thread message_thread_;
	std::atomic_bool isRunning_{false};
	// Mode is used to indicate whether commands should be sync or async
	std::unique_ptr<BlockingHub> hub_;
	std::unique_ptr<ObjectHub> obj_;
	std::unique_ptr<OrderManager> orders_;
	std::unique_ptr<PositionManager> positions_;
	std::unique_ptr<BulletinManager> bulletins_;
	// std::unique_ptr<RequestId> requestId_;
	int nextId_{};
};
