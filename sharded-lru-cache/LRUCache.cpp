#ifndef SHARDED_LRU_CACHE_LRUCACHE_CPP
#define SHARDED_LRU_CACHE_LRUCACHE_CPP

#include "LRUCache.h"

template <typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(const std::size_t capacity) : capacity_(capacity) {
    if (capacity_ == 0U) {
        throw std::invalid_argument("LRUCache capacity must be greater than zero");
    }
}

template <typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(LRUCache&& other) noexcept
    : capacity_(other.capacity_), entries_(std::move(other.entries_)), index_(std::move(other.index_)) {}

template <typename Key, typename Value>
LRUCache<Key, Value>& LRUCache<Key, Value>::operator=(LRUCache&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    capacity_ = other.capacity_;
    entries_ = std::move(other.entries_);
    index_ = std::move(other.index_);
    return *this;
}

template <typename Key, typename Value>
std::optional<Value> LRUCache<Key, Value>::Get(const Key& key) {
    const auto found = index_.find(key);
    if (found == index_.end()) {
        return std::nullopt;
    }

    MoveToFront(found->second);
    return found->second->second;
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::Put(const Key& key, const Value& value) {
    const auto found = index_.find(key);
    if (found != index_.end()) {
        found->second->second = value;
        MoveToFront(found->second);
        return;
    }

    entries_.emplace_front(key, value);
    index_[key] = entries_.begin();

    if (index_.size() > capacity_) {
        const Key lru_key = entries_.back().first;
        entries_.pop_back();
        index_.erase(lru_key);
    }
}

template <typename Key, typename Value>
std::size_t LRUCache<Key, Value>::Size() const noexcept {
    return index_.size();
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::Clear() noexcept {
    index_.clear();
    entries_.clear();
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::MoveToFront(const NodeIterator iterator) noexcept {
    if (iterator == entries_.begin()) {
        return;
    }

    entries_.splice(entries_.begin(), entries_, iterator);
}

#endif  // SHARDED_LRU_CACHE_LRUCACHE_CPP
