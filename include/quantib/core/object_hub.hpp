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

	// Overload implementation with type_index (Default)
	template<typename Tag, typename T, typename... Args>
	T &create(Args &&... args) {
		if (objects_.contains(typeid(Tag))) {
			throw std::runtime_error("Cannot create: object already exists");
		}
		auto holder = std::make_unique<Holder<T> >(std::forward<Args>(args)...);
		T &ref = *holder->ptr;

		objects_.emplace(typeid(Tag), std::move(holder));

		return ref;
	}

	template<typename Tag, typename T>
	T &insert(T obj) {
		auto holder = std::make_unique<Holder<T> >(std::move(obj));
		T &ref = *holder->ptr;

		objects_[typeid(Tag)] = std::move(holder);

		return ref;
	}

	template<typename Tag, typename T>
	T &update(T value) {
		auto &obj = get<Tag, T>();
		obj = std::move(value);
		return obj;
	}

	template<typename Tag, typename T>
	T &get() {
		const std::type_index key = typeid(Tag);
		const auto it = objects_.find(key);

		if (it == objects_.end()) {
			throw std::runtime_error("Cannot get: object does not exist");
		}

		auto *holder = dynamic_cast<Holder<T> *>(it->second.get());

		if (!holder) {
			throw std::runtime_error("Cannot get: stored object has different type");
		}

		return *holder->ptr;
	}

	template<typename Tag>
	bool contains() const {
		return objects_.contains<Tag>();
	}

	template<typename Tag>
	void erase() {
		objects_.erase(typeid(Tag));
	}

	void clear() {
		objects_.clear();
	}

private:
	std::unordered_map<std::type_index, std::unique_ptr<IHolder> > objects_;
	std::shared_ptr<Logger> logger_;
};
