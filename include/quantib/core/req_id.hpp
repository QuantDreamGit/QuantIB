#pragma once

/* Request Id Logic */

// Simple Logic, start request id from 0 then increase
// Should be thread safe since will be used by IB class and wrapper that are oon
// the same thread. However, it won't impact too much at this stage thread
// safety. it's quite simple to implement.
#include <mutex>
#include "quantib/utils/logger.hpp"

#define LOCK_GUARD std::lock_guard<std::mutex> lk(mtx_);

class RequestId {
public:
	explicit RequestId(const std::shared_ptr<Logger> &logger) : logger_(logger) {
		LOG_DEBUG_TAG(REQ_ID, "Initialized correctly.");
	}

	inline int get() {
		LOCK_GUARD
		return id;
	}

	inline int getNew() {
		LOCK_GUARD
		return ++id;
	}

	inline int getAndIncr() {
		LOCK_GUARD
		return id++;
	}

private:
	int id = 0;
	std::mutex mtx_;
	std::shared_ptr<Logger> logger_;
};
