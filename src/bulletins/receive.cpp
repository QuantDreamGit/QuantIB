#include "quantib/core/bulletin_manager.h"
#include "quantib/wrappers/base_wrapper.hpp"

void ResponseWrapper::updateNewsBulletin(int msgId, int msgType, const std::string &newsMessage, const std::string &originExch) {
	Bulletin bulletin(msgId, msgType, newsMessage, originExch);
	obj_.try_append<BulletinStoreTag>(bulletin);
}
