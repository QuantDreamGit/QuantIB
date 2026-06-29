#include "quantib/core/position_manager.h"

#include "EClientSocket.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

PositionManager::PositionManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: logger_(logger), client_(client), obj_(obj), hub_(hub) {
	hub_.subscribe<PositionStoreTag, std::unordered_map<int, Position>>([&]() { client_.reqPositions(); });
	positions_ = obj_.get_or_create<PositionStoreTag, std::unordered_map<int, Position>>();
}

const std::unordered_map<int, Position>* PositionManager::getPositions() const {
	return positions_;
}
