#pragma once
#include <string>
#include <vector>

class EClientSocket;
class BlockingHub;
class ObjectHub;
class Logger;

struct Bulletin {
	int msg_id{};
	int msg_type{};
	std::string message;
	std::string orig_exchange;

	Bulletin() = default;

	explicit Bulletin(const int msg_id_, const int msg_type_, std::string message_, std::string orig_exchange_) :
		msg_id(msg_id_), msg_type(msg_type_), message(std::move(message_)), orig_exchange(std::move(orig_exchange_)) {}
};

class NewsManager {
public:
	NewsManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger);

	void startSubscription() const;

	void stopSubscription() const;

	void getBulletins() const;

	void getLastBulletin() const;

private:
	Logger& logger_;
	EClientSocket& client_;
	ObjectHub& obj_;
	BlockingHub& hub_;

	std::vector<Bulletin>* news_;
};
