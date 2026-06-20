#pragma once
#include <chrono>

struct RetryPolicy {
	int max_attempts = 0;
	std::chrono::milliseconds retry_delay{0};
};

struct NoRetry {
	static constexpr int max_attempts = 1;

	static std::chrono::milliseconds delayForAttempt(int) {
		return std::chrono::milliseconds{0};
	}
};

template<int Attempts, int DelayMs>
struct FixedRetry {
	static constexpr int max_attempts = Attempts;

	static std::chrono::milliseconds delayForAttempt(int) {
		return std::chrono::milliseconds{DelayMs};
	}
};

template<int Attempts, int BaseMs, int MaxMs>
struct ExpRetry {
	static constexpr int max_attempts = Attempts;

	static std::chrono::milliseconds delayForAttempt(int attempt) {
		int factor = 1 << std::max(0, attempt - 1);
		int delay = std::min(BaseMs * factor, MaxMs);
		return std::chrono::milliseconds{delay};
	}
};

