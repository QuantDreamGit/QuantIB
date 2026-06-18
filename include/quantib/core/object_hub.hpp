#pragma once

#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tags.hpp"
#include "quantib/utils/logger.hpp"

struct IHolder {
    virtual ~IHolder() = default;
    [[nodiscard]] virtual std::type_index type() const noexcept = 0;
};

template<typename T>
struct Holder final : IHolder {
    template<typename... Args>
    explicit Holder(Args&&... args)
        : ptr(std::make_unique<T>(std::forward<Args>(args)...)) {}

    [[nodiscard]] std::type_index type() const noexcept override {
        return typeid(T);
    }

    std::unique_ptr<T> ptr;
};

class ObjectHub {
public:
    explicit ObjectHub(const std::shared_ptr<Logger>& logger)
        : logger_(logger) {
        LOG_DEBUG_TAG(OBJ_HUB, "Initialized correctly.");
    }

    template<typename Tag, typename T, typename... Args>
    T& create(Args&&... args) {
        std::unique_lock lock(mutex_);

        if (auto* existing = try_get_unlocked<Tag, T>()) {
            LOG_DEBUG_TAG(OBJ_HUB, "Object already exists for tag {}.", typeid(Tag).name());
            return *existing;
        }

        auto holder = std::make_unique<Holder<T>>(std::forward<Args>(args)...);
        T& ref = *holder->ptr;
        objects_.emplace(typeid(Tag), std::move(holder));

        LOG_TRACE_TAG(OBJ_HUB, "Created object of type {} with tag {}.",
                      typeid(T).name(), typeid(Tag).name());

        return ref;
    }

    template<typename T, typename... Args>
    T& create(Args&&... args) {
        return create<T, T>(std::forward<Args>(args)...);
    }

    template<typename Tag, typename T>
    T& insert(T obj) {
        std::unique_lock lock(mutex_);

        auto holder = std::make_unique<Holder<T>>(std::move(obj));
        T& ref = *holder->ptr;
        objects_[typeid(Tag)] = std::move(holder);

        LOG_TRACE_TAG(OBJ_HUB, "Inserted object of type {} with tag {}.",
                      typeid(T).name(), typeid(Tag).name());

        return ref;
    }

    template<typename T>
    T& insert(T obj) {
        return insert<T, T>(std::move(obj));
    }

    template<typename Tag, typename T>
    void update_or_create(T value) {
        std::unique_lock lock(mutex_);

        if (auto* obj = try_get_unlocked<Tag, T>()) {
            *obj = std::move(value);
            LOG_TRACE_TAG(OBJ_HUB, "Updated object of type {} with tag {}.",
                          typeid(T).name(), typeid(Tag).name());
            return;
        }

        insert_unlocked<Tag, T>(std::move(value));
    }

    template<typename T>
    void update_or_create(T value) {
        update_or_create<T, T>(std::move(value));
    }

    template<typename Tag, typename T, typename... Args>
    T* get_or_create(Args&&... args) {
        std::unique_lock lock(mutex_);

        if (auto* obj = try_get_unlocked<Tag, T>()) {
            return obj;
        }

        return &create_unlocked<Tag, T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T* get_or_create(Args&&... args) {
        return get_or_create<T, T>(std::forward<Args>(args)...);
    }

    template<typename Tag, typename T>
    T* try_get() {
        std::shared_lock lock(mutex_);
        return try_get_unlocked<Tag, T>();
    }

    template<typename T>
    T* try_get() {
        return try_get<T, T>();
    }

    template<typename Tag, typename T>
    void try_append(T value) {
        std::unique_lock lock(mutex_);

        auto* vec = try_get_unlocked<Tag, std::vector<T>>();
        if (!vec) {
            vec = &create_unlocked<Tag, std::vector<T>>();
        }

        vec->emplace_back(std::move(value));
    }

    template<typename T>
    void try_append(T value) {
        try_append<T, T>(std::move(value));
    }

    template<typename Tag, typename Key, typename Value>
    void insert_or_assign(Key key, Value value) {
        std::unique_lock lock(mutex_);

        auto* map = try_get_unlocked<Tag, std::unordered_map<Key, Value>>();
        if (!map) {
            map = &create_unlocked<Tag, std::unordered_map<Key, Value>>();
        }

        map->insert_or_assign(std::move(key), std::move(value));
    }

    template<typename Tag, typename Key, typename Value>
    Value* find_in_map(const Key& key) {
        std::shared_lock lock(mutex_);

        auto* map = try_get_unlocked<Tag, std::unordered_map<Key, Value>>();
        if (!map) {
            return nullptr;
        }

        auto it = map->find(key);
        if (it == map->end()) {
            return nullptr;
        }

        return &it->second;
    }

    template<typename Tag, typename Key, typename Value>
    Value* find_or_create_in_map(const Key& key) {
        std::unique_lock lock(mutex_);

        auto* map = try_get_unlocked<Tag, std::unordered_map<Key, Value>>();
        if (!map) {
            map = &create_unlocked<Tag, std::unordered_map<Key, Value>>();
        }

        auto [it, inserted] = map->try_emplace(key);
        (void)inserted;

        return &it->second;
    }

    template<typename Tag, typename Key, typename Value>
    void delete_from_map(const Key& key) {
        std::unique_lock lock(mutex_);

        auto* map = try_get_unlocked<Tag, std::unordered_map<Key, Value>>();
        if (!map) {
            return;
        }

        map->erase(key);
    }

    template<typename Tag, typename Key, typename Value>
    void clear_map() {
        std::unique_lock lock(mutex_);

        auto* map = try_get_unlocked<Tag, std::unordered_map<Key, Value>>();
        if (!map) {
            return;
        }

        map->clear();
    }

    template<typename Tag, typename T>
    void clear_vec() {
        std::unique_lock lock(mutex_);

        auto* vec = try_get_unlocked<Tag, std::vector<T>>();
        if (!vec) {
            return;
        }

        vec->clear();
    }

    template<typename Tag>
    void set_bool(bool value) {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, bool>();
        if (!elem) {
            return;
        }

        *elem = value;
    }

    template<typename Tag>
    void set_or_create_bool(bool value) {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, bool>();
        if (!elem) {
            elem = &create_unlocked<Tag, bool>(value);
            return;
        }

        *elem = value;
    }

    template<typename Tag>
    int get_increment_int() {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, int>();
        if (!elem) {
            return 0;
        }

        return ++(*elem);
    }

    template<typename Tag>
    int get_decrement_int() {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, int>();
        if (!elem) {
            return 0;
        }

        return --(*elem);
    }

    template<typename Tag>
    int increment_get_int() {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, int>();
        if (!elem) {
            return 0;
        }

        return (*elem)++;
    }

    template<typename Tag>
    int decrement_get_int() {
        std::unique_lock lock(mutex_);

        auto* elem = try_get_unlocked<Tag, int>();
        if (!elem) {
            return 0;
        }

        return (*elem)--;
    }

    template<typename Tag>
    bool contains() const {
        std::shared_lock lock(mutex_);
        return objects_.contains(typeid(Tag));
    }

    template<typename Tag>
        requires(!std::is_base_of_v<PermanentTag, Tag>)
    void erase() {
        std::unique_lock lock(mutex_);
        objects_.erase(typeid(Tag));
    }

    void clear() {
        std::unique_lock lock(mutex_);
        objects_.clear();
    }

private:
    template<typename Tag, typename T>
    T* try_get_unlocked() const {
        const auto it = objects_.find(typeid(Tag));
        if (it == objects_.end()) {
            return nullptr;
        }

        if (it->second->type() != typeid(T)) {
            LOG_WARN_TAG(OBJ_HUB, "Object with tag {} has wrong type. Expected {}, found {}.",
                         typeid(Tag).name(), typeid(T).name(), it->second->type().name());
            return nullptr;
        }

        auto* holder = static_cast<Holder<T>*>(it->second.get());
        return holder->ptr.get();
    }

    template<typename Tag, typename T, typename... Args>
    T& create_unlocked(Args&&... args) {
        auto holder = std::make_unique<Holder<T>>(std::forward<Args>(args)...);
        T& ref = *holder->ptr;
        objects_[typeid(Tag)] = std::move(holder);

        LOG_TRACE_TAG(OBJ_HUB, "Created object of type {} with tag {}.",
                      typeid(T).name(), typeid(Tag).name());

        return ref;
    }

    template<typename Tag, typename T>
    T& insert_unlocked(T obj) {
        auto holder = std::make_unique<Holder<T>>(std::move(obj));
        T& ref = *holder->ptr;
        objects_[typeid(Tag)] = std::move(holder);

        LOG_TRACE_TAG(OBJ_HUB, "Inserted object of type {} with tag {}.",
                      typeid(T).name(), typeid(Tag).name());

        return ref;
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::type_index, std::unique_ptr<IHolder>> objects_;
    std::shared_ptr<Logger> logger_;
};