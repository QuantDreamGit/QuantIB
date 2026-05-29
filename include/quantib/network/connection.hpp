
// Async
#include <any>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
struct Async {};

// // Notification Policies
class BlockingHub {
public:
  using RequestFunc = std::function<void()>;

  // Wait for the data
  template <typename T>
  std::optional<T> wait_for(std::type_index key,
                            std::function<void()> request) {
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

  // Send data
  template <typename T> void send(std::type_index key, T value) {
    {
      std::lock_guard<std::mutex> lk(mtx_);
      value_[key] = std::move(value);
      isReady_[key] = true;
    }
    cv_.notify_all();
  }

  inline void insertKey(std::type_index key) {
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
  std::mutex mtx_;
  std::condition_variable cv_;

  std::unordered_set<std::type_index> pendingTag_;
  std::unordered_map<std::type_index, bool> isReady_;
  std::unordered_map<std::type_index, std::any> value_;
};
// // Retry Policies
struct NoRetry {
  static constexpr bool retry = false;
};
