#pragma once
#include "Contract.h"
#include "EClientSocket.h"
#include "EReader.h"
#include "EReaderOSSignal.h"

#include "quantib/core/account.hpp"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/utils/logger.hpp"
#include "quantib/wrappers/base_wrapper.hpp"

#include <memory>
#include <optional>
#include <thread>

#include "../risk_manager/risk_manager.h"
#include "quantib/core/bulletin_manager.h"
#include "quantib/core/contract_manager.h"
#include "quantib/core/data_manager.h"
#include "quantib/core/order_manager.h"
#include "quantib/core/position_manager.h"
#include "quantib/core/strategy_manager.h"

template <typename ProfileT>
class IB {
public:
	using log_level = spdlog::level::level_enum;
	using Profile = ProfileT;

	using StrategyManagerT = typename Profile::Strategy;
	using RiskManagerT = typename Profile::RiskManager;
	// using DataFeedT = typename Profile::DataFeed;
	// DataFeed could be IB (default now), Polygon, ...
	// using ExecutionT = typename Profile::Execution;
	// Execution could be paper, backtest, live (default now)...

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
	};

	~IB() {
		isRunning_ = false;
		if (message_thread_.joinable()) message_thread_.join();

		if (riskRunning_) {
			riskRunning_ = false;
			if (risk_thread_.joinable()) risk_thread_.join();
		}

		if (client_ && client_->isConnected()) client_->eDisconnect();
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

			auto nextId_ = hub_->wait_for<ConnectTag, int>([&]() {}).value_or(-1);
			obj_->create<NextIdTag, int>(nextId_);

			initializeManagers();

			return nextId_;
		}

		return std::nullopt;
	}

	/* Account */
	void accountSummarySub(const std::string& tags = AccountSummaryTags::all(), const std::string& groups = "All");

	void accountSummaryCancel() const;

	[[nodiscard]] std::optional<AccountSummary> getAccountIds() const;

	void accountUpdateSub() const;

	void accountUpdateCancel() const;

	/* Contracts */
	[[nodiscard]] std::optional<std::vector<ContractDetails>>
	getContractDetailsSync(int reqId, const Contract& contract) const;

	[[nodiscard]] std::optional<ContractDetails>
	getContractDetails(const Contract& contract) const;

	[[nodiscard]] std::optional<ContractDetails>
	getContractDetails(const std::string& symbol) const;

	[[nodiscard]] std::optional<std::vector<Contract>>
	getContractsSync(int reqId, const Contract& contract) const;

	[[nodiscard]] std::optional<Contract>
	getContract(const Contract& contract) const;

	[[nodiscard]] std::optional<Contract>
	getContract(const std::string& symbol) const;

	void registerContract(const Contract& contract) const;

	void registerInstrument(const Contract& contract) const {
		registerContract(contract);
		registerMarketData(contract);
	}

	/* Orders */
	void placeOrder(const Contract& contract, const Order& order) const {
		auto future = risk_->validate(OrderIntent(contract, order));
		const RiskResult result = future.get();

		if (result.decision == RiskDecision::Reject) {
			LOG_WARN_TAG(RISK, "Order rejected for {}. Reason: {}", contract.symbol, result.reason);
			return;
		}

		if (result.decision == RiskDecision::Pending) {
			LOG_WARN_TAG(RISK, "Order still pending for {}. Reason: {}", contract.symbol, result.reason);
			return;
		}

		client_->placeOrder(getNextId(), contract, order);
	}

	[[nodiscard]] std::optional<std::unordered_map<int, OpenOrders>*>
	getOpenOrders() const;

	[[nodiscard]] std::optional<std::unordered_map<int, ClosedOrders>*>
	getClosedOrders() const;

	void PositionsSub() const { client_->reqPositions(); }

	[[nodiscard]] std::optional<std::unordered_map<int, Position>*>
	getPositions() const {
		if (auto* positions = obj_->try_get<PositionStoreTag, std::unordered_map<int, Position>>()) {
			return positions;
		}
		return std::nullopt;
	}

	void bulletinsSub(const bool value) const {
		if (value) bulletins_->startSubscription();
		else bulletins_->stopSubscription();
	}

	[[nodiscard]] std::optional<std::vector<Bulletin>*> getBulletins() const {
		if (auto* bulletins = obj_->try_get<BulletinStoreTag, std::vector<Bulletin>>()) { return bulletins; }
		return std::nullopt;
	}

	[[nodiscard]] EClientSocket& getClient() const { return *client_; }

	[[nodiscard]] int getNextId() const { return obj_->get_increment_int<NextIdTag>(); }

	[[nodiscard]] int getCurrentId() const { return *obj_->try_get<NextIdTag, int>(); }

	void registerMarketData(const Contract& contract, const std::string& tick_list = "", const bool snap = false,
	                        const bool reg_snap = false) const {
		data_->marketDataSub(contract, tick_list, snap, reg_snap);
	}

protected:
	void initializeManagers() {
		// Managers
		positions_ = std::make_unique<PositionManager>(*client_, *hub_, *obj_, *logger_);
		order_factory_ = std::make_unique<OrderFactory<RiskManagerT>>(*logger_);
		orders_ = std::make_unique<OrderManager<RiskManagerT>>(*client_, *hub_, *obj_, *logger_, *order_factory_);
		data_ = std::make_unique<DataManager>(*client_, *hub_, *obj_, *logger_);
		contract_ = std::make_unique<ContractManager>(*client_, *hub_, *obj_, *logger_);
		risk_ = std::make_unique<RiskManagerT>(*client_, *hub_, *obj_, *logger_, *positions_, *orders_, *data_,
		                                       *contract_);
		risk_->start();
		strategy_ = std::make_unique<StrategyManager<StrategyManagerT, RiskManagerT>>(
			*client_, *hub_, *obj_, *logger_, *risk_);
		// bulletins_ = std::make_unique<BulletinManager>(*client_, *hub_, *obj_,
		// *logger_);
	}

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
	std::unique_ptr<RiskManagerT> risk_;
	std::unique_ptr<OrderFactory<RiskManagerT>> order_factory_;
	std::unique_ptr<StrategyManager<StrategyManagerT, RiskManagerT>> strategy_;
	std::unique_ptr<OrderManager<RiskManagerT>> orders_;
	std::unique_ptr<PositionManager> positions_;
	std::unique_ptr<NewsManager> bulletins_;
	std::unique_ptr<DataManager> data_;
	std::unique_ptr<ContractManager> contract_;
	// std::unique_ptr<RequestId> requestId_;
	std::thread risk_thread_;
	std::atomic_bool riskRunning_{false};
};
