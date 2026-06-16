#pragma once
#include <string>

#include "EClientSocket.h"
#include "quantib/network/connection.hpp"

struct Bulletin {
	int msg_id;
	int msg_type;
	std::string message;
	std::string orig_exchange;

	Bulletin() = default;

	explicit Bulletin(const int msg_id_, const int msg_type_,
	                  std::string message_, std::string orig_exchange_) : msg_id(msg_id_),
	                                                                    msg_type(msg_type_),
	                                                                    message(std::move(message_)),
	                                                                    orig_exchange(std::move(orig_exchange_)) {
	}
};

class BulletinManager {
public:
	BulletinManager(EClientSocket &client, BlockingHub &hub, ObjectHub &obj, Logger &logger) : logger_(logger),
	                                                                                           client_(client),
	                                                                                           obj_(obj), hub_(hub) {
		// Create object where to store bulletin
		bulletins_ = obj_.try_get<BulletinStoreTag, std::vector<Bulletin> >();
	}

	void startSubscription() const {
		client_.reqNewsBulletins(true);
	}

	void stopSubscription() const {
		client_.reqNewsBulletins(false);
	}

	void getBulletins() const {
		if (bulletins_) {
			for (const auto &bulletin: *bulletins_) {
				LOG_INFO_TAG(BULLETIN, "Bulletin id: {}, type: {}, message: {}, exchange: {}.", bulletin.msg_id,
				             bulletin.msg_type, bulletin.message, bulletin.orig_exchange);
			}
		} else {
			LOG_INFO_TAG(BULLETIN, "No bulletins received yet.");
		}
	}

	void getLastBulletin() const {
		if (bulletins_ && !bulletins_->empty()) {
			const auto &bulletin = bulletins_->back();
			LOG_INFO_TAG(BULLETIN, "Last Bulletin id: {}, type: {}, message: {}, exchange: {}.", bulletin.msg_id,
			             bulletin.msg_type, bulletin.message, bulletin.orig_exchange);
		} else {
			LOG_INFO_TAG(BULLETIN, "No bulletins received yet.");
		}
	}

private:
	Logger &logger_;
	EClientSocket &client_;
	ObjectHub &obj_;
	BlockingHub &hub_;

	std::vector<Bulletin> *bulletins_;
};
