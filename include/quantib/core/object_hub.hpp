#pragma once

#include <memory>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include "quantib/utils/logger.hpp"

struct IHolder {
	virtual ~IHolder() = default;
};

template<typename T>
struct Holder : IHolder {
	template<typename... Args>
	explicit Holder(Args &&... args)
		: ptr(std::make_unique<T>(std::forward<Args>(args)...)) {
	}

	std::unique_ptr<T> ptr;
};

class ObjectHub {
public:
	explicit ObjectHub(const std::shared_ptr<Logger> &logger) : logger_(logger) {
		LOG_DEBUG_TAG(OBJ_HUB, "Initialized correctly.");
	}

	~ObjectHub() = default;

	template<typename Tag, typename T, typename... Args>
	T &create(Args &&... args) {
		if (objects_.contains(typeid(Tag))) {
			LOG_WARN_TAG(OBJ_HUB, "Cannot create: object already exists");
			return get<Tag, T>();
		}
		auto holder = std::make_unique<Holder<T> >(std::forward<Args>(args)...);
		T &ref = *holder->ptr;
		objects_.emplace(typeid(Tag), std::move(holder));
		LOG_TRACE_TAG(OBJ_HUB, "Created object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
		return ref;
	}

	template<typename T, typename... Args>
	T &create(Args &&... args) {
		return create<T, T>(std::forward<Args>(args)...);
	}

	template<typename Tag, typename T>
	T &insert(T obj) {
		auto holder = std::make_unique<Holder<T> >(std::move(obj));
		T &ref = *holder->ptr;
		objects_[typeid(Tag)] = std::move(holder);
		LOG_TRACE_TAG(OBJ_HUB, "Inserted object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());

		return ref;
	}

	template<typename T>
	T &insert(T obj) {
		return insert<T, T>(std::move(obj));
	}

	template<typename Tag, typename T>
	T &update(T value) {
		auto &obj = get<Tag, T>();
		obj = std::move(value);
		LOG_TRACE_TAG(OBJ_HUB, "Updated object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
		return obj;
	}

	template<typename T>
	T &update(T value) {
		return update<T, T>(std::move(value));
	}

	template<typename Tag, typename T>
	T &get_or_default() {
		const std::type_index key = typeid(Tag);
		const auto it = objects_.find(key);
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			return create<Tag, T>();
		}
		// Otherwise, cast it to required type
		auto *holder = dynamic_cast<Holder<T> *>(it->second.get());

		if (!holder) {
			LOG_CRITICAL_TAG(OBJ_HUB, "Cannot get ({}): stored object has different type. ", typeid(Tag).name());
			throw std::runtime_error("Cannot get: stored object has different type");
		}
		// LOG_TRACE_TAG(OBJ_HUB, "Got object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
		return *holder->ptr;
	}

	template<typename T>
	T &get_or_default() {
		return get_or_default<T, T>();
	}

	template<typename Tag, typename T, typename... Args>
	T &get_or_default(Args &&... args) {
		const std::type_index key = typeid(Tag);
		const auto it = objects_.find(key);
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			LOG_TRACE_TAG(OBJ_HUB, "Object with tag {} not found. Creating it using provided arguments.",
			              typeid(Tag).name());
			return create<Tag, T>(std::forward<Args>(args)...);
		}
		// Otherwise, cast it to required type
		auto *holder = dynamic_cast<Holder<T> *>(it->second.get());
		if (!holder) {
			LOG_CRITICAL_TAG(OBJ_HUB, "Cannot get ({}): stored object has different type. ", typeid(Tag).name());
			throw std::runtime_error("Cannot get: stored object has different type");
		}
		// LOG_TRACE_TAG(OBJ_HUB, "Got object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
		return *holder->ptr;
	}

	template<typename Tag, typename T>
	T &get() {
		const std::type_index key = typeid(Tag);
		const auto it = objects_.find(key);
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			LOG_CRITICAL_TAG(OBJ_HUB, "Cannot get ({}): stored object does not exist.", typeid(Tag).name());
		}
		// Otherwise, cast it to required type
		auto *holder = dynamic_cast<Holder<T> *>(it->second.get());
		if (!holder) {
			LOG_CRITICAL_TAG(OBJ_HUB, "Cannot get ({}): stored object has different type", typeid(Tag).name());
		}
		// LOG_TRACE_TAG(OBJ_HUB, "Got object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
		return *holder->ptr;
	}

	template<typename T>
	T &get() {
		return get<T, T>();
	}

	template<typename T>
	bool contains() const {
		return objects_.contains(typeid(T));
	}

	template<typename T>
	void erase() {
		objects_.erase(typeid(T));
	}

	void clear() {
		objects_.clear();
	}

private:
	std::unordered_map<std::type_index, std::unique_ptr<IHolder> > objects_;
	std::shared_ptr<Logger> logger_;
};
