#pragma once
#include <string>

#include "EClientSocket.h"
#include "Contract.h"
#include "object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

struct ContractInfo {
	int con_id;
	bool pending_details = true;
	ContractDetails contract_details;

	ContractInfo() = default;

	ContractInfo(const int con_id_, const bool pending_details_, const ContractDetails& contract_details_) :
		con_id(con_id_), pending_details(pending_details_), contract_details(contract_details_) {}

	std::optional<bool> isInLiquidHours() const;
};

class ContractManager {
public:
	ContractManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger) : logger_(logger),
		client_(client), obj_(obj), hub_(hub) {
		contracts_ = &obj_.create<ContractStoreTag, std::unordered_map<int, ContractDetails>>();
		contract_info = &obj_.create<ContractSymbolStoreTag, std::unordered_map<std::string, ContractInfo>>();
	}

	void registerContract(const Contract& contract) const {
		// Request contract details
		client_.reqContractDetails(getNextId(), contract);
		LOG_DEBUG_TAG(IB_STR, "Contract details request done for symbol {}.", contract.symbol);
	}

	std::optional<ContractInfo> getContractInfo(const std::string& symbol) const {
		if (const auto con_info = obj_.find_in_map<ContractSymbolStoreTag, std::string, ContractInfo>(symbol)) return
			std::optional(*con_info);
		return std::nullopt;
	}

	std::optional<Contract> getContract(const int conId) const {
		auto con_details = getContractDetails(conId);
		if (con_details.has_value()) return con_details->contract;
		return std::nullopt;
	}

	std::optional<Contract> getContract(const std::string& symbol) const {
		return getContract((*contract_info)[symbol].con_id);
	}

	std::optional<ContractDetails> getContractDetails(const int conId) const {
		if (const auto contract = obj_.find_in_map<ContractStoreTag, int, ContractDetails>(conId)) return
			std::optional(*contract);
		return std::nullopt;
	}

	std::optional<ContractDetails> getContractDetails(const std::string& symbol) const {
		return getContractDetails((*contract_info)[symbol].con_id);
	}

	[[nodiscard]] int getNextId() const { return obj_.get_increment_int<NextIdTag>(); }
	[[nodiscard]] int getCurrentId() const { return *obj_.try_get<NextIdTag, int>(); }

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::unordered_map<int, ContractDetails>* contracts_;
	std::unordered_map<std::string, ContractInfo>* contract_info;
};
