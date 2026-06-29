#include "quantib/core/bulletin_manager.h"

#include "EClientSocket.h"
#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

NewsManager::NewsManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger)
	: logger_(logger), client_(client), obj_(obj), hub_(hub) {
	news_ = obj_.try_get<BulletinStoreTag, std::vector<Bulletin>>();
}

void NewsManager::startSubscription() const {
	client_.reqNewsBulletins(true);
}

void NewsManager::stopSubscription() const {
	client_.reqNewsBulletins(false);
}

void NewsManager::getBulletins() const {
	if (news_) {
		for (const auto& bulletin : *news_) {
			LOG_INFO_TAG(BULLETIN, "Bulletin id: {}, type: {}, message: {}, exchange: {}.", bulletin.msg_id,
			             bulletin.msg_type, bulletin.message, bulletin.orig_exchange);
		}
	}
	else {
		LOG_INFO_TAG(BULLETIN, "No bulletins received yet.");
	}
}

void NewsManager::getLastBulletin() const {
	if (news_ && !news_->empty()) {
		const auto& bulletin = news_->back();
		LOG_INFO_TAG(BULLETIN, "Last Bulletin id: {}, type: {}, message: {}, exchange: {}.", bulletin.msg_id,
		             bulletin.msg_type, bulletin.message, bulletin.orig_exchange);
	}
	else {
		LOG_INFO_TAG(BULLETIN, "No bulletins received yet.");
	}
}
