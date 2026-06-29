#include "quantib/core/contract_manager.h"

#include "EClientSocket.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

ContractManager::ContractManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: logger_(logger), client_(client), obj_(obj), hub_(hub) {
	contracts_ = &obj_.create<ContractStoreTag, std::unordered_map<int, ContractDetails>>();
	contract_info = &obj_.create<ContractSymbolStoreTag, std::unordered_map<std::string, ContractInfo>>();
}

void ContractManager::registerContract(const Contract& contract) const {
	client_.reqContractDetails(getNextId(), contract);
	LOG_DEBUG_TAG(IB_STR, "Contract details request done for symbol {}.", contract.symbol);
}

std::optional<ContractInfo> ContractManager::getContractInfo(const std::string& symbol) const {
	if (const auto con_info = obj_.find_in_map<ContractSymbolStoreTag, std::string, ContractInfo>(symbol)) return
		std::optional(*con_info);
	return std::nullopt;
}

std::optional<Contract> ContractManager::getContract(const int conId) const {
	auto con_details = getContractDetails(conId);
	if (con_details.has_value()) return con_details->contract;
	return std::nullopt;
}

std::optional<Contract> ContractManager::getContract(const std::string& symbol) const {
	return getContract((*contract_info)[symbol].con_id);
}

std::optional<ContractDetails> ContractManager::getContractDetails(const int conId) const {
	if (const auto contract = obj_.find_in_map<ContractStoreTag, int, ContractDetails>(conId)) return
		std::optional(*contract);
	return std::nullopt;
}

std::optional<ContractDetails> ContractManager::getContractDetails(const std::string& symbol) const {
	return getContractDetails((*contract_info)[symbol].con_id);
}

int ContractManager::getNextId() const {
	return obj_.get_increment_int<NextIdTag>();
}

int ContractManager::getCurrentId() const {
	return *obj_.try_get<NextIdTag, int>();
}
