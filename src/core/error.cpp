#include "quantib/sync/ib.hpp"

inline const std::unordered_set<int> ignoredErrorCodes = {
	2104,
	2106,
	2158
};

void ResponseWrapper::error(int id, time_t errorTime, int errorCode, const std::string &errorString,
                            const std::string &advancedOrderRejectJson) {
	if (ignoredErrorCodes.contains(errorCode)) {
		return;
	}
	LOG_WARN_TAG(WRAPPER, "Received error with id {}: code={}, message={}, advancedOrderRejectJson={}, Time={}", id,
	             errorCode, errorString,
	             advancedOrderRejectJson, errorTime);
}
