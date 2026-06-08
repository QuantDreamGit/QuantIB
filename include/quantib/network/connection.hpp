#include "quantib/core/object_hub.hpp"
#include "quantib/core/tags.hpp"
#include <any>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// This is the first type of connection hub
class BlockingHub {
public:
	using RequestFunc = std::function<void()>;

	explicit BlockingHub(std::shared_ptr<ObjectHub> obj, const std::shared_ptr<Logger> &logger) : obj_(std::move(obj)),
	                                                                                              logger_(logger) {
		LOG_DEBUG(CONN_HUB, "Initialized correctly.");
	}

	// Wait for the data
	template<typename T>
	std::optional<T> wait_for(std::type_index key,
	                          const std::function<void()> &request) {
		// First lock the mutex to change isReady_ boolean
		{
			std::lock_guard<std::mutex> lk(mtx_);
			isReady_[key] = false;
			pendingTag_.insert(key);
		}
		// Then do RequestFunc
		request();
		// Finally wait until data is ready
		std::unique_lock<std::mutex> lk(mtx_);
		cv_.wait(lk, [&] { return isReady_[key]; });
		auto result = std::any_cast<T>(value_[key]);

		// Clear old values for safety
		value_.erase(key);
		isReady_.erase(key);
		pendingTag_.erase(key);

		return result;
	}

	// Subscribe to data feed
	template<typename T>
	void subscribe(const std::type_index key, const std::function<void()> &request) {
		// First lock the mutex to change isReady_ boolean
		{
			std::lock_guard<std::mutex> lk(mtx_);
			// isReady[key] is set to false at the beginning so that when callback is
			// executed we know whether it's the first time that data are received or
			// not. After first initialization is set to false but key is not erased
			// from pendingTag. In this way we can initialize an object using object
			// hub or just update its content.
			// isReady_[key] = false;
			// Right now this idea is no further implemented since I want to start as
			// simple as possible.
			// Later implementations will take this into consideration!
			pendingTag_.insert(key);
			obj_->create<T>(key);
		}
		// Then do RequestFunc
		request();
	}

	// Send data
	template<typename Tag, typename T>
	void send(T value) {
		// Here we have to differentiate betweenubscription feeds or sync request.
		// Tags are derived from two base classes that indicates their types.
		auto key = std::type_index(typeid(Tag));
		if (std::is_base_of_v<RequestTag, Tag>) {
			// Sync request handling
			{
				std::lock_guard<std::mutex> lk(mtx_);
				value_[key] = std::move(value);
				isReady_[key] = true;
			}
			cv_.notify_all();
		} else if (std::is_base_of_v<SubscriptionTag, Tag>) {
			std::lock_guard<std::mutex> lk(mtx_);
			// Check if object has been created yet
			// if (isReady_[key] == false) {
			// Then Create object
			// obj_->insert<T>(key, std::move(value));
			// isReady_[key] = true;
			// } else {
			obj_->update(key, std::forward<T>(value));
			// }
		}
	}

	void insertKey(std::type_index key) {
		std::lock_guard<std::mutex> lk(mtx_);
		pendingTag_.insert(key);
	}

	inline bool containsKey(std::type_index key) {
		std::lock_guard<std::mutex> lk(mtx_);
		return pendingTag_.contains(key);
	}

	inline void removeKey(std::type_index key) {
		std::lock_guard<std::mutex> lk(mtx_);
		pendingTag_.erase(key);
	}

private:
	std::shared_ptr<Logger> logger_;
	std::mutex mtx_;
	std::condition_variable cv_;
	std::shared_ptr<ObjectHub> obj_;
	std::unordered_set<std::type_index> pendingTag_;
	std::unordered_map<std::type_index, bool> isReady_;
	std::unordered_map<std::type_index, std::any> value_;
};
