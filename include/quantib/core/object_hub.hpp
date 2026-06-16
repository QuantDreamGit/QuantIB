#pragma once

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include "quantib/utils/logger.hpp"

struct IHolder {
	virtual ~IHolder() = default;

	[[nodiscard]] virtual std::type_index type() const noexcept = 0;
};

template<typename T>
// Final attribute let us avoid dynamic cast since compiler knows that this struct
// won't be subclassed.
struct Holder final : IHolder {
	// Store object of type T in a unique pointer
	template<typename... Args>
	explicit Holder(Args &&... args)
		: ptr(std::make_unique<T>(std::forward<Args>(args)...)) {
	}

	// Type util
	[[nodiscard]] std::type_index type() const noexcept override {
		return typeid(T);
	}

	std::unique_ptr<T> ptr;
};

// References must be sent correctly while pointers can be also nullptr.
// In this way I can build a more robust system that avoids runtime errors when using get or try_get.
// Create -> return reference
// Get -> return pointer
class ObjectHub {
public:
	explicit ObjectHub(const std::shared_ptr<Logger> &logger) : logger_(logger) {
		LOG_DEBUG_TAG(OBJ_HUB, "Initialized correctly.");
	}

	~ObjectHub() = default;

	// Create a new object, if already present return its reference
	template<typename Tag, typename T, typename... Args>
	T &create(Args &&... args) {
		if (objects_.contains(typeid(Tag))) {
			LOG_WARN_TAG(OBJ_HUB, "Cannot create: object already exists");
			return *get_or_create<Tag, T>(std::forward<Args>(args)...);
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

	// Insert an existing object
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

	// Update an existing object with a given value
	template<typename Tag, typename T>
	void update_or_create(T value) {
		auto *obj = try_get<Tag, T>();

		if (!obj) {
			LOG_WARN_TAG(OBJ_HUB, "Cannot update object with tag {}: object does not exist or has wrong type.",
				typeid(Tag).name());
			insert<Tag, T>(std::move(value));
			return;
		}
		*obj = std::move(value);
		LOG_TRACE_TAG(OBJ_HUB, "Updated object of type {} with tag {}.", typeid(T).name(), typeid(Tag).name());
	}

	template<typename T>
	void update_or_create(T value) {
		return update_or_create<T, T>(std::move(value));
	}

	// Get an existing object
	template<typename T>
	T *get_or_create() {
		return get_or_create<T, T>();
	}

	template<typename Tag, typename T, typename... Args>
	T *get_or_create(Args &&... args) {
		const auto it = objects_.find(typeid(Tag));
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			LOG_TRACE_TAG(OBJ_HUB, "Object with tag {} not found. Creating it using provided arguments.",
			              typeid(Tag).name());
			return &create<Tag, T>(std::forward<Args>(args)...);
		}
		// We have to cast from IHolder<T> to Holder<T> class
		auto *holder = static_cast<Holder<T> *>(it->second.get());
		return holder->ptr.get();
	}

	template<typename Tag, typename T>
	T *try_get() {
		const auto it = objects_.find(typeid(Tag));
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			LOG_WARN_TAG(OBJ_HUB, "Cannot get ({}): stored object does not exist, returning a nullptr.", typeid(Tag)
			             .name());
			return nullptr;
		}
		// We have to cast from IHolder<T> to Holder<T> class
		auto *holder = static_cast<Holder<T> *>(it->second.get());
		return holder->ptr.get();
	}

	template<typename T>
	T *try_get() {
		return try_get<T, T>();
	}

	// Append to an objects of type: std::vector<T>
	template<typename Tag, typename T>
	void try_append(T value) {
		const auto it = objects_.find(typeid(Tag));
		// If you can't find it create it using default constructor
		if (it == objects_.end()) {
			LOG_WARN_TAG(OBJ_HUB, "Cannot append ({}): stored object does not exist. It will "
			             "be created.", typeid(Tag).name());
			auto &obj = create<Tag, std::vector<T> >();
			obj.emplace_back(std::move(value));
			return;
		}
		// We have to cast from IHolder<T> to Holder<T> class
		auto* holder = static_cast<Holder<std::vector<T>>*>(it->second.get());
		// Check if std::vector<T> is stored
		holder->ptr->emplace_back(std::move(value));
	}

	template<typename T>
	void try_append(T value) {
		return try_append<T, T>(std::move(value));
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
