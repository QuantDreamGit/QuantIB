#pragma once
#include <optional>
#include <string>
#include <unordered_map>

#include "Contract.h"

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

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
	ContractManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger);

	void registerContract(const Contract& contract) const;

	std::optional<ContractInfo> getContractInfo(const std::string& symbol) const;

	std::optional<Contract> getContract(int conId) const;

	std::optional<Contract> getContract(const std::string& symbol) const;

	std::optional<ContractDetails> getContractDetails(int conId) const;

	std::optional<ContractDetails> getContractDetails(const std::string& symbol) const;

	[[nodiscard]] int getNextId() const;
	[[nodiscard]] int getCurrentId() const;

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::unordered_map<int, ContractDetails>* contracts_;
	std::unordered_map<std::string, ContractInfo>* contract_info;
};
